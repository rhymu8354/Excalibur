/**
 * @file main.cpp
 *
 * This module holds the main() function, which is the entrypoint
 * to the program.
 *
 * © 2018 by Richard Walters
 */

#include "HexDumpNetworkConnectionDecorator.hpp"
#include "TimeKeeper.hpp"

#include <condition_variable>
#include <Http/Client.hpp>
#include <Http/Request.hpp>
#include <HttpNetworkTransport/HttpClientNetworkTransport.hpp>
#include <iostream>
#include <memory>
#include <mutex>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <StringExtensions/StringExtensions.hpp>
#include <SystemAbstractions/DiagnosticsStreamReporter.hpp>
#include <SystemAbstractions/File.hpp>
#include <SystemAbstractions/NetworkConnection.hpp>
#include <thread>
#include <TlsDecorator/TlsDecorator.hpp>
#include <Uri/Uri.hpp>
#include <WebSockets/WebSocket.hpp>

namespace {

    /**
     * This is the default port for HTTP over TLS.
     */
    constexpr uint16_t DEFAULT_HTTPS_PORT = 443;

    /**
     * This function prints to the standard error stream information
     * about how to use this program.
     */
    void PrintUsageInformation() {
        fprintf(
            stderr,
            (
                "Usage: WsTalk <URL>\n"
                "\n"
                "Connect to the server at URL (use wss: scheme please!) with a request to\n"
                "upgrade the connection to a WebSocket.  If the connection is successfully\n"
                "upgraded, begin an interactive mode where incoming messages are displayed\n"
                "and what the user types becomes content to send in a message.\n"
                "\n"
                "  URL     URL of the server to which to connect\n"
            )
        );
    }

    /**
     * This flag indicates whether or not the web client should shut down.
     */
    bool shutDown = false;

    /**
     * This contains variables set through the operating system environment
     * or the command-line arguments.
     */
    struct Environment {
        /**
         * This is the URL of the server to which to connect.
         */
        Uri::Uri url;

        /**
         * This holds extra SSL certificates the client should accept.
         */
        std::string extraCerts;
    };

    /**
     * This function is set up to be called when the SIGINT signal is
     * received by the program.  It just sets the "shutDown" flag
     * and relies on the program to be polling the flag to detect
     * when it's been set.
     *
     * @param[in] sig
     *     This is the signal for which this function was called.
     */
    void InterruptHandler(int) {
        shutDown = true;
    }

    /**
     * This function updates the program environment to incorporate
     * any applicable command-line arguments.
     *
     * @param[in] argc
     *     This is the number of command-line arguments given to the program.
     *
     * @param[in] argv
     *     This is the array of command-line arguments given to the program.
     *
     * @param[in,out] environment
     *     This is the environment to update.
     *
     * @param[in] diagnosticMessageDelegate
     *     This is the function to call to publish any diagnostic messages.
     *
     * @return
     *     An indication of whether or not the function succeeded is returned.
     */
    bool ProcessCommandLineArguments(
        int argc,
        char* argv[],
        Environment& environment,
        SystemAbstractions::DiagnosticsSender::DiagnosticMessageDelegate diagnosticMessageDelegate
    ) {
        std::string urlString;
        size_t state = 0;
        for (int i = 1; i < argc; ++i) {
            const std::string arg(argv[i]);
            switch (state) {
                case 0: { // next argument
                    if (arg == "--cert") {
                        state = 1;
                    } else {
                        if (!urlString.empty()) {
                            diagnosticMessageDelegate(
                                "WsTalk",
                                SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                                "multiple URLs given"
                            );
                            return false;
                        }
                        urlString = arg;
                        state = 0;
                    }
                } break;

                case 1: { // certificate to accept
                    SystemAbstractions::File certFile(arg);
                    if (!certFile.Open()) {
                        diagnosticMessageDelegate(
                            "WsTalk",
                            SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                            StringExtensions::sprintf(
                                "unable to open certificate file '%s'",
                                certFile.GetPath().c_str()
                            )
                        );
                        return false;
                    }
                    std::vector< uint8_t > certBuffer(certFile.GetSize());
                    if (certFile.Read(certBuffer) != certBuffer.size()) {
                        diagnosticMessageDelegate(
                            "WsTalk",
                            SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                            StringExtensions::sprintf(
                                "unable to read certificate file '%s'",
                                certFile.GetPath().c_str()
                            )
                        );
                        return false;
                    }
                    const std::string cert(
                        (const char*)certBuffer.data(),
                        certBuffer.size()
                    );
                    environment.extraCerts += cert;
                    state = 0;
                } break;
            }
        }
        if (state == 1) {
            diagnosticMessageDelegate(
                "WsTalk",
                SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                "certificate file path expected for --cert"
            );
            return false;
        }
        if (urlString.empty()) {
            diagnosticMessageDelegate(
                "WsTalk",
                SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                "no URL given"
            );
            return false;
        }
        if (!environment.url.ParseFromString(urlString)) {
            diagnosticMessageDelegate(
                "WsTalk",
                SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                "bad URL given"
            );
            return false;
        }
        const auto scheme = environment.url.GetScheme();
        if (scheme != "wss") {
            diagnosticMessageDelegate(
                "WsTalk",
                SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                "please use \"wss\" (secure WebSocket) scheme"
            );
            return false;
        }
        if (!environment.url.HasPort()) {
            environment.url.SetPort(DEFAULT_HTTPS_PORT);
        }
        return true;
    }

    /**
     * This function loads the trusted certificate authority (CA) certificate
     * bundle from the file system, where it's expected to be sitting
     * side-by-side the program's image, with the name "cert.pem".
     *
     * @param[out] caCerts
     *     This is where to store the loaded CA certificate bundle.
     *
     * @param[in] diagnosticMessageDelegate
     *     This is the function to call to publish any diagnostic messages.
     *
     * @return
     *     An indication of whether or not the function succeeded is returned.
     */
    bool LoadCaCerts(
        std::string& caCerts,
        SystemAbstractions::DiagnosticsSender::DiagnosticMessageDelegate diagnosticMessageDelegate
    ) {
        SystemAbstractions::File caCertsFile(
            SystemAbstractions::File::GetExeParentDirectory()
            + "/cert.pem"
        );
        if (!caCertsFile.Open()) {
            diagnosticMessageDelegate(
                "WsTalk",
                SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                StringExtensions::sprintf(
                    "unable to open root CA certificates file '%s'",
                    caCertsFile.GetPath().c_str()
                )
            );
            return false;
        }
        std::vector< uint8_t > caCertsBuffer(caCertsFile.GetSize());
        if (caCertsFile.Read(caCertsBuffer) != caCertsBuffer.size()) {
            diagnosticMessageDelegate(
                "WsTalk",
                SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                "unable to read root CA certificates file"
            );
            return false;
        }
        caCerts.assign(
            (const char*)caCertsBuffer.data(),
            caCertsBuffer.size()
        );
        return true;
    }

    /**
     * This function starts the client with the given transport layer.
     *
     * @param[in,out] client
     *     This is the client to start.
     *
     * @param[in] environment
     *     This contains variables set through the operating system
     *     environment or the command-line arguments.
     *
     * @param[in] caCerts
     *     This is the trusted certificate authority (CA) certificate bundle to
     *     use to verify certificates at the TLS layer.
     *
     * @param[in] diagnosticMessageDelegate
     *     This is the function to call to publish any diagnostic messages.
     *
     * @return
     *     An indication of whether or not the function succeeded is returned.
     */
    bool StartClient(
        Http::Client& client,
        const Environment& environment,
        const std::string& caCerts,
        SystemAbstractions::DiagnosticsSender::DiagnosticMessageDelegate diagnosticMessageDelegate
    ) {
        auto transport = std::make_shared< HttpNetworkTransport::HttpClientNetworkTransport >();
        transport->SubscribeToDiagnostics(diagnosticMessageDelegate);
        Http::Client::MobilizationDependencies deps;
        transport->SetConnectionFactory(
            [
                diagnosticMessageDelegate,
                caCerts
            ](
                const std::string& scheme,
                const std::string& serverName
            ) -> std::shared_ptr< SystemAbstractions::INetworkConnection > {
                const auto hexDumpNetworkConnectionUpperDecorator = std::make_shared< HexDumpNetworkConnectionDecorator >();
                const auto tlsDecorator = std::make_shared< TlsDecorator::TlsDecorator >();
                const auto hexDumpNetworkConnectionLowerDecorator = std::make_shared< HexDumpNetworkConnectionDecorator >();
                const auto connection = std::make_shared< SystemAbstractions::NetworkConnection >();
                const auto hexDumpLowerDelegate = [diagnosticMessageDelegate](const std::string& line){
                    diagnosticMessageDelegate("Wire", 3, line);
                };
                hexDumpNetworkConnectionLowerDecorator->Decorate(connection, hexDumpLowerDelegate);
                tlsDecorator->ConfigureAsClient(hexDumpNetworkConnectionLowerDecorator, caCerts, serverName);
                const auto hexDumpUpperDelegate = [diagnosticMessageDelegate](const std::string& line){
                    diagnosticMessageDelegate("TLS", 3, line);
                };
                hexDumpNetworkConnectionUpperDecorator->Decorate(tlsDecorator, hexDumpUpperDelegate);
                return hexDumpNetworkConnectionUpperDecorator;
            }
        );
        deps.transport = transport;
        deps.timeKeeper = std::make_shared< TimeKeeper >();
        client.Mobilize(deps);
        return true;
    }

    /**
     * This function uses the given web client to connect to the web server at
     * the given URL and request an upgrade to the WebSocket protocol.
     *
     * @param[in,out] client
     *     This is the client to use to connect to the server.
     *
     * @param[in] closeDelegate
     *     This is the delegate to provide to the WebSocket to be called
     *     whenever the WebSocket is closed.
     *
     * @param[in] url
     *     This is the URL of the server to which to connect.
     *
     * @param[in] diagnosticMessageDelegate
     *     This is the function to call to publish any diagnostic messages.
     *
     * @return
     *     If successful, the WebSocket object representing the client end
     *     of the connected WebSocket is returned.
     *
     * @retval nullptr
     *     This is returned if there is any problem connecting to the server
     *     or upgrading the connection to a WebSocket.
     */
    std::shared_ptr< WebSockets::WebSocket > ConnectToWebSocket(
        Http::Client& client,
        WebSockets::WebSocket::CloseReceivedDelegate closeDelegate,
        const Uri::Uri& url,
        SystemAbstractions::DiagnosticsSender::DiagnosticMessageDelegate diagnosticMessageDelegate
    ) {
        Http::Request request;
        request.method = "GET";
        request.target = url;
        diagnosticMessageDelegate(
            "WsTalk",
            3,
            "Connecting to '" + request.target.GenerateString() + "'..."
        );
        const auto ws = std::make_shared< WebSockets::WebSocket >();
        ws->SubscribeToDiagnostics(diagnosticMessageDelegate);
        ws->StartOpenAsClient(request);
        WebSockets::WebSocket::Delegates wsDelegates;
        wsDelegates.text = [diagnosticMessageDelegate](const std::string& data){
            diagnosticMessageDelegate(
                "WsTalk",
                1,
                "Text from WebSocket: " + data
            );
        };
        wsDelegates.ping = [diagnosticMessageDelegate](const std::string& data){
            diagnosticMessageDelegate(
                "WsTalk",
                0,
                "Ping from WebSocket: " + data
            );
        };
        wsDelegates.close = closeDelegate;
        ws->SetDelegates(std::move(wsDelegates));
        bool wsEngaged = false;
        const auto transaction = client.Request(
            request,
            false,
            [
                ws,
                &wsEngaged
            ](
                const Http::Response& response,
                std::shared_ptr< Http::Connection > connection,
                const std::string& trailer
            ){
                if (ws->FinishOpenAsClient(connection, response)) {
                    wsEngaged = true;
                }
            }
        );
        while (!shutDown) {
            if (transaction->AwaitCompletion(std::chrono::milliseconds(5000))) {
                switch (transaction->state) {
                    case Http::Client::Transaction::State::Completed: {
                        if (wsEngaged) {
                            diagnosticMessageDelegate(
                                "WsTalk",
                                3,
                                "Connection established."
                            );
                            return ws;
                        } else {
                            if (transaction->response.statusCode == 101) {
                                diagnosticMessageDelegate(
                                    "WsTalk",
                                    SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                                    "Connection upgraded, but failed to engage WebSocket"
                                );
                            } else {
                                diagnosticMessageDelegate(
                                    "WsTalk",
                                    SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                                    StringExtensions::sprintf(
                                        "Got back response: %u %s",
                                        transaction->response.statusCode,
                                        transaction->response.reasonPhrase.c_str()
                                    )
                                );
                            }
                            return nullptr;
                        }
                    } break;

                    case Http::Client::Transaction::State::UnableToConnect: {
                        diagnosticMessageDelegate(
                            "WsTalk",
                            SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                            "unable to connect"
                        );
                    } break;

                    case Http::Client::Transaction::State::Broken: {
                        diagnosticMessageDelegate(
                            "WsTalk",
                            SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                            "connection broken by server"
                        );
                    } break;

                    case Http::Client::Transaction::State::Timeout: {
                        diagnosticMessageDelegate(
                            "WsTalk",
                            SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                            "timeout waiting for response"
                        );
                    } break;

                    default: break;
                }
                return nullptr;
            }
        }
        diagnosticMessageDelegate(
            "WsTalk",
            SystemAbstractions::DiagnosticsSender::Levels::WARNING,
            "Fetch Canceled"
        );
        return nullptr;
    }

    /**
     * This function stops the client.
     *
     * @param[in,out] client
     *     This is the client to stop.
     */
    void StopClient(Http::Client& client) {
        client.Demobilize();
    }

}

/**
 * This function is the entrypoint of the program.
 *
 * The program is terminated after the SIGINT signal is caught.
 *
 * @param[in] argc
 *     This is the number of command-line arguments given to the program.
 *
 * @param[in] argv
 *     This is the array of command-line arguments given to the program.
 */
int main(int argc, char* argv[]) {
#ifdef _WIN32
    //_crtBreakAlloc = 18;
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif /* _WIN32 */
    // Set up a handler for SIGINT to set our "shutDown" flag.
    const auto previousInterruptHandler = signal(SIGINT, InterruptHandler);

    // Turn off standard output stream buffering.
    (void)setbuf(stdout, NULL);

    // Set up diagnostic message publisher that prints diagnostic messages
    // to the standard error stream.
    const auto diagnosticsPublisher = SystemAbstractions::DiagnosticsStreamReporter(stderr, stderr);

    // Process command line and environment variables.
    Environment environment;
    if (!ProcessCommandLineArguments(argc, argv, environment, diagnosticsPublisher)) {
        PrintUsageInformation();
        return EXIT_FAILURE;
    }

    // Load trusted certificate authority (CA) certificate bundle to use
    // at the TLS layer of web connections.
    std::string caCerts;
    if (!LoadCaCerts(caCerts, diagnosticsPublisher)) {
        return EXIT_FAILURE;
    }

    // Set up an HTTP client to be used to connect to the web server.
    Http::Client client;
    const auto diagnosticsSubscription = client.SubscribeToDiagnostics(diagnosticsPublisher);
    if (
        !StartClient(
            client,
            environment,
            caCerts + environment.extraCerts,
            diagnosticsPublisher
        )
    ) {
        return EXIT_FAILURE;
    }

    // Connect to the web server and request an upgrade to a WebSocket.
    bool wsClosed = false;
    std::mutex mutex;
    std::condition_variable condition;
    const auto closeDelegate = [
        diagnosticsPublisher,
        &wsClosed,
        &mutex,
        &condition
    ](
        unsigned int code,
        const std::string& reason
    ){
        std::lock_guard< std::mutex > lock(mutex);
        wsClosed = true;
        condition.notify_one();
        diagnosticsPublisher(
            "WsTalk",
            3,
            StringExtensions::sprintf(
                "WebSocket closed: %u %s",
                code,
                reason.c_str()
            )
        );
    };
    auto ws = ConnectToWebSocket(
        client,
        closeDelegate,
        environment.url,
        diagnosticsPublisher
    );
    if (ws == nullptr) {
        return EXIT_FAILURE;
    }

    // Shut down the client, since we no longer need it.
    StopClient(client);

    // Loop until interrupted with SIGINT.
    diagnosticsPublisher(
        "WsTalk",
        3,
        "Type messages or press <Ctrl>+<C> (and then <Enter>, if necessary) to exit."
    );
    while (
        !shutDown
        && !wsClosed
    ) {
        std::string line;
        std::getline(std::cin, line);
        if (shutDown) {
            break;
        }
        if (line.empty()) {
            continue;
        }
        diagnosticsPublisher(
            "WsTalk",
            1,
            "Sending text message: " + line
        );
        ws->SendText(line);
    }

    // Close our end of the WebSocket, and wait for the other end
    // to close.
    ws->Close(1000, "Kthxbye");
    {
        std::unique_lock< std::mutex > lock(mutex);
        if (
            !condition.wait_for(
                lock,
                std::chrono::milliseconds(1000),
                [&wsClosed]{ return wsClosed; }
            )
        ) {
            diagnosticsPublisher(
                "WsTalk",
                SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                "Timed out waiting for WebSocket to close on server end"
            );
        }
    }
    ws = nullptr;

    // We're all done!
    (void)signal(SIGINT, previousInterruptHandler);
    diagnosticsPublisher(
        "WsTalk",
        3,
        "Exiting."
    );
    return EXIT_SUCCESS;
}

/**
 * @file main.cpp
 *
 * This module holds the main() function, which is the entrypoint
 * to the program.
 *
 * © 2018 by Richard Walters
 */

#include "TimeKeeper.hpp"

#include <Aws/Config.hpp>
#include <Aws/SignApi.hpp>
#include <chrono>
#include <Http/Client.hpp>
#include <Http/Request.hpp>
#include <HttpNetworkTransport/HttpClientNetworkTransport.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <SystemAbstractions/DiagnosticsStreamReporter.hpp>
#include <SystemAbstractions/File.hpp>
#include <SystemAbstractions/NetworkConnection.hpp>
#include <SystemAbstractions/StringExtensions.hpp>
#include <thread>
#include <time.h>
#include <TlsDecorator/TlsDecorator.hpp>

namespace {

    /**
     * This function prints to the standard error stream information
     * about how to use this program.
     */
    void PrintUsageInformation() {
        fprintf(
            stderr,
            (
                "Usage: AwsPlay\n"
                "\n"
                "Do stuff with Amazon Web Services (AWS).\n"
            )
        );
    }

    /**
     * This contains variables set through the operating system environment
     * or the command-line arguments.
     */
    struct Environment {
    };

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
        Environment& environment
    ) {
        enum class State {
            // First command-line argument
            Initial,
        } state = State::Initial;
        for (int i = 1; i < argc; ++i) {
            const std::string arg(argv[i]);
            switch (state) {
                case State::Initial: { // next argument
                    state = State::Initial;
                } break;
            }
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
                "AwsPlay",
                SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                SystemAbstractions::sprintf(
                    "unable to open root CA certificates file '%s'",
                    caCertsFile.GetPath().c_str()
                )
            );
            return false;
        }
        std::vector< uint8_t > caCertsBuffer(caCertsFile.GetSize());
        if (caCertsFile.Read(caCertsBuffer) != caCertsBuffer.size()) {
            diagnosticMessageDelegate(
                "AwsPlay",
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
     * This function configures and mobilizes the client to work with
     * SystemAbstractions::NetworkConnection for making network connections,
     * decorated with TlsDecorator::TlsDecorator to make the connections
     * secure.
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
            ](const std::string& serverName) -> std::shared_ptr< SystemAbstractions::INetworkConnection > {
                const auto decorator = std::make_shared< TlsDecorator::TlsDecorator >();
                const auto connection = std::make_shared< SystemAbstractions::NetworkConnection >();
                decorator->ConfigureAsClient(connection, caCerts, serverName);
                return decorator;
            }
        );
        deps.transport = transport;
        deps.timeKeeper = std::make_shared< TimeKeeper >();
        client.Mobilize(deps);
        return true;
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

    /**
     * This function converts the given time from seconds since the UNIX epoch
     * to the ISO-8601 format YYYYMMDD'T'HHMMSS'Z' expected by AWS.
     *
     * @param[in] time
     *     This is the time in seconds since the UNIX epoch.
     *
     * @return
     *     The given time, formatted in the ISO-8601 format
     *     YYYYMMDD'T'HHMMSS'Z', is returned.
     */
    std::string AmzTimestamp(time_t time) {
        char buffer[17];
        (void)strftime(buffer, sizeof(buffer), "%Y%m%dT%H%M%SZ", gmtime(&time));
        return buffer;
    }

    /**
     * This function will print out the names of all S3 buckets
     * available to the user.
     *
     * @param[in,out] client
     *     This is the HTTP client to use to communicate with AWS.
     *
     * @param[in] region
     *     This is the AWS region to access.
     *
     * @param[in] accessKeyId
     *     This is the access key ID to use to authenticate with AWS.
     *
     * @param[in] secretAccessKey
     *     This is the secret access key to use to authenticate with AWS.
     *
     * @param[in] sessionToken
     *     This is an optional temporary security credential token to use to
     *     authenticate with AWS.
     *
     * @param[in] diagnosticMessageDelegate
     *     This is the function to call to publish any diagnostic messages.
     */
    void ListS3Buckets(
        Http::Client& client,
        const std::string& region,
        const std::string& accessKeyId,
        const std::string& secretAccessKey,
        const std::string& sessionToken,
        SystemAbstractions::DiagnosticsSender::DiagnosticMessageDelegate diagnosticMessageDelegate
    ) {
        const auto host = "s3." + region + ".amazonaws.com";
        const auto date = AmzTimestamp(time(NULL));
        Http::Request request;
        request.method = "GET";
        request.target.SetHost(host);
        request.target.SetPort(443);
        request.target.SetPath({""});
        request.headers.AddHeader("Host", host);
        request.headers.AddHeader("x-amz-date", date);
        const auto canonicalRequest = Aws::SignApi::ConstructCanonicalRequest(request.Generate());
        const auto payloadHashOffset = canonicalRequest.find_last_of('\n') + 1;
        const auto payloadHash = canonicalRequest.substr(payloadHashOffset);
        const auto stringToSign = Aws::SignApi::MakeStringToSign(
            region,
            "s3",
            canonicalRequest
        );
        const auto authorization = Aws::SignApi::MakeAuthorization(
            stringToSign,
            canonicalRequest,
            accessKeyId,
            secretAccessKey
        );
        request.headers.AddHeader("Authorization", authorization);
        request.headers.AddHeader("x-amz-content-sha256", payloadHash);
        if (!sessionToken.empty()) {
            request.headers.AddHeader("x-amz-security-token", sessionToken);
        }
        const auto rawRequest = request.Generate();
        (void)printf(
            (
                "Request: ---------------\n"
                "%s"
                "\n"
            ),
            rawRequest.c_str()
        );
        const auto transaction = client.Request(
            request,
            false
        );
        if (transaction->AwaitCompletion(std::chrono::milliseconds(5000))) {
            switch (transaction->state) {
                case Http::Client::Transaction::State::Completed: {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    (void)printf(
                        (
                            "Response: %u %s\n"
                            "Headers: ---------------\n"
                        ),
                        transaction->response.statusCode,
                        transaction->response.reasonPhrase.c_str()
                    );
                    for (const auto& header: transaction->response.headers.GetAll()) {
                        (void)printf(
                            "%s: %s\n",
                            ((std::string)header.name).c_str(),
                            header.value.c_str()
                        );
                    }
                    (void)printf("------------------------\n");
                    if (!transaction->response.body.empty()) {
                        (void)fwrite(
                            transaction->response.body.c_str(),
                            transaction->response.body.length(),
                            1,
                            stdout
                        );
                        (void)fwrite("\n", 1, 1, stdout);
                    }
                } break;

                case Http::Client::Transaction::State::UnableToConnect: {
                    diagnosticMessageDelegate(
                        "AwsPlay",
                        SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                        "unable to connect"
                    );
                } break;

                case Http::Client::Transaction::State::Broken: {
                    diagnosticMessageDelegate(
                        "AwsPlay",
                        SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                        "connection broken by server"
                    );
                } break;

                case Http::Client::Transaction::State::Timeout: {
                    diagnosticMessageDelegate(
                        "AwsPlay",
                        SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                        "timeout waiting for response"
                    );
                } break;

                default: break;
            }
            return;
        } else {
            diagnosticMessageDelegate(
                "AwsPlay",
                SystemAbstractions::DiagnosticsSender::Levels::ERROR,
                "timeout waiting for response"
            );
        }
    }

}

/**
 * This function is the entrypoint of the program.
 *
 * @param[in] argc
 *     This is the number of command-line arguments given to the program.
 *
 * @param[in] argv
 *     This is the array of command-line arguments given to the program.
 */
int main(int argc, char* argv[]) {
    // Set up to catch memory leaks.
#ifdef _WIN32
    //_crtBreakAlloc = 18;
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif /* _WIN32 */

    // Process command line and environment variables.
    Environment environment;
    (void)setbuf(stdout, NULL);
    if (!ProcessCommandLineArguments(argc, argv, environment)) {
        PrintUsageInformation();
        return EXIT_FAILURE;
    }

    // Get AWS configuration defaults.
    const auto awsConfigDefaults = Aws::Config::GetDefaults();

    // Set up diagnostic message publisher that prints diagnostic messages
    // to the standard error stream.
    const auto diagnosticsPublisher = SystemAbstractions::DiagnosticsStreamReporter(stderr, stderr);

    // Load trusted certificate authority (CA) certificate bundle to use
    // at the TLS layer of web connections.
    std::string caCerts;
    if (!LoadCaCerts(caCerts, diagnosticsPublisher)) {
        return EXIT_FAILURE;
    }

    // Set up an HTTP client to be used to talk to AWS.
    Http::Client client;
    const auto diagnosticsSubscription = client.SubscribeToDiagnostics(diagnosticsPublisher);
    if (!StartClient(client, environment, caCerts, diagnosticsPublisher)) {
        return EXIT_FAILURE;
    }

    // Let's try to talk to AWS S3 to learn what our buckets are,
    // for fun and profit.
    ListS3Buckets(
        client,
        awsConfigDefaults.region,
        awsConfigDefaults.accessKeyId,
        awsConfigDefaults.secretAccessKey,
        awsConfigDefaults.sessionToken,
        diagnosticsPublisher
    );

    // Done.
    StopClient(client);
    return EXIT_SUCCESS;
}

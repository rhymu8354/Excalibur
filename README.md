# Excalibur

Excalibur is just an "inspirational" name I picked at random for this solution,
which is a sort of "incubation chamber" for some libraries that I plan to
develop towards making my own web server framework mostly from scratch.

## Usage

There are several ways to use this solution:

1. Build the libraries in the solution and run their unit tests.
2. Build the WebServer program and its plugins, and configure and run the web
   server.
3. Run or build the "chatter" React-based web client app.

### Unit tests

Each library in the solution comes with unit tests.  There are two main ways to
run the tests of the C++ components:

1. Use [CTest](https://cmake.org/cmake/help/latest/module/CTest.html).
2. Run individual unit test runners directly (e.g.
   `build/Uri/test/Debug/UriTests.exe`).

For the Rust components, use `cargo test` to run their unit tests.

### WebServer

The web server is a stand-alone program hosting the Http::Server class,
configuring it, and loading plug-ins which register themselves as resource
handlers for the server.  Server and plug-in configuration is read from a JSON
file.

The following plug-ins are provided with the solution:

* ChatRoom -- implements a demonstration "chat room" which uses WebSockets to
  communicate with clients; designed to work with "chatter"
* Echo -- dynamically creates an HTML page showing the user the contents of
  their own HTTP request's headers
* StaticContent -- serves static content from the host file system

To run the WebServer, assemble the desired plug-ins into a folder and give the
path from the WebServer executable to that folder in the "plugins-image"
configuration item.  Also set the "plugins-runtime" configuration item to a
path from the WebServer executable to a temporary folder to be used to actually
load and run the plug-in modules.  Set other configuration items as desired,
and run the WebServer.

WebServer looks for its configuration file as named "config.json" and located
either in the current working directory or the directory containing the
WebServer executable.  To override the path/name of the configuration file,
provide it on the command line using the "-c" argument, as in:

```bash
WebServer -c path/to/config/server-config.json
```

### chatter

This is a web client app, based on the [React](https://reactjs.org/) JavaScript
framework.  It's used to demonstrate the capabilities of the ChatRoom plug-in
of the web server.  The user can connect to the chat room, select a nickname,
and interact with the chat room by sending answers to math questions posted by
"MathBot2000", a "bot" which is built into the chat room.

## Programs and libraries hosted in this solution

* Base64 -- Implementation of the Base64 encoding algorithm described in [RFC
  4648](https://tools.ietf.org/html/rfc4648), "The Base16, Base32, and Base64
  Data Encodings".
* Http -- Implementation of [RFC 7230](https://tools.ietf.org/html/rfc7230),
  "Hypertext Transfer Protocol (HTTP/1.1): Message Syntax and Routing".
* HttpNetworkTransport -- Implements the transport interfaces needed by the
  Http library, in terms of the network endpoint and connection abstractions
  provided by the SystemAbstractions library.
* Json -- Implementation of [RFC 7159](https://tools.ietf.org/html/rfc7159),
  "The JavaScript Object Notation (JSON) Data Interchange Format".
* MessageHeaders -- Used to handle the headers of a message on the internet,
  which is a common theme amongst several internet standards:
  * e-mail: [RFC 5322](https://tools.ietf.org/html/rfc5322)
  * HTTP (web): [RFC 7230](https://tools.ietf.org/html/rfc7320)
  * SIP (VoIP): [RFC 3261](https://tools.ietf.org/html/3261)
* Hash -- Implementation of various hash (message digest) functions, such as
  the Secure Hash Algorithm 1 (SHA-1) described in [RFC
  3174](https://tools.ietf.org/html/rfc3174).
* SystemAbstractions -- Library containing classes which are either common
  utilities or wrap common operating system services whose interfaces vary from
  one operating system to another.
* Uri -- Implementation of [RFC 3986](https://tools.ietf.org/html/rfc3986),
  "Uniform Resource Identifier (URI): Generic Syntax"
* Utf8 -- Implementation of [RFC 3629](https://tools.ietf.org/html/rfc3629),
  "UTF-8 (Unicode Transformation Format)"
* WebServer -- Stand-alone program hosting Http::Server and using
  HttpNetworkTransport::HttpServerNetworkTransport to bind the server to an
  actual network; comes with several plug-ins.
* WebSockets -- Implementation of [RFC
  6455](https://tools.ietf.org/html/rfc6455), "The WebSocket Protocol".

## Supported platforms / recommended toolchains

For the C++ components, the following toolchains are recommended for the
following platforms:

* Windows -- [Visual Studio](https://www.visualstudio.com/) (Microsoft Visual
  C++)
* Linux -- clang or gcc
* MacOS -- Xcode (clang)

For the Rust components, it's recommended to use [rustup](https://rustup.rs/)
to obtain and maintain the Rust toolchain.

## Building the C++ Components

There are two distinct steps in the build process:

1. Generation of the build system, using CMake
2. Compiling, linking, etc., using CMake-compatible toolchain

### Prerequisites

* [CMake](https://cmake.org/) version 3.8 or newer
* C++11 toolchain compatible with CMake for your development platform (e.g. [Visual Studio](https://www.visualstudio.com/) on Windows)

### Build system generation

Generate the build system using [CMake](https://cmake.org/) from the solution
root.  For example:

```bash
mkdir build
cd build
cmake -G "Visual Studio 15 2017" -A "x64" ..
```

### Compiling, linking, et cetera

Either use [CMake](https://cmake.org/) or your toolchain's IDE to build.  For
[CMake](https://cmake.org/):

```bash
cd build
cmake --build . --config Release
```

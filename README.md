# Excalibur

Excalibur is just an "inspirational" name I picked at random for this solution, which is a sort of "incubation chamber" for some libraries that I plan to develop towards making my own web server framework mostly from scratch.

## Usage

Right now, the way you use this solution is to build it and run the unit tests for the various libraries.  There are two main ways to run the tests:

1. Use [CTest](https://cmake.org/cmake/help/latest/module/CTest.html).
2. Run individual unit test runners directly (e.g. `build/Uri/test/Debug/UriTests.exe`).

## Libraries hosted in this solution

* Uri -- Implementation of [RFC 3986](https://tools.ietf.org/html/rfc3986),
"Uniform Resource Identifier (URI): Generic Syntax"

## Supported platforms / recommended toolchains

* Windows -- [Visual Studio](https://www.visualstudio.com/) (Microsoft Visual C++)
* Linux -- clang or gcc
* MacOS -- Xcode (clang)

## Building

There are two distinct steps in the build process:

1. Generation of the build system, using CMake
2. Compiling, linking, etc., using CMake-compatible toolchain

### Prerequisites

* [CMake](https://cmake.org/) version 3.8 or newer
* C++11 toolchain compatible with CMake for your development platform (e.g. [Visual Studio](https://www.visualstudio.com/) on Windows)

### Build system generation

Generate the build system using [CMake](https://cmake.org/) from the solution root.  For example:

```bash
mkdir build
cd build
cmake -G "Visual Studio 15 2017" -A "x64" ..
```

### Compiling, linking, etc.

Either use [CMake](https://cmake.org/) or your toolchain's IDE to build.
For [CMake](https://cmake.org/):

```bash
cd build
cmake --build . --config Release
```

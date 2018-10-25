# CMakeLists.txt for Excalibur
#
# © 2018 by Richard Walters

cmake_minimum_required(VERSION 3.8)
set(This Excalibur)
project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

get_directory_property(ParentDirectory PARENT_DIRECTORY)
if(ParentDirectory STREQUAL "")
    # Define option for linking with the runtime statically.
    if(MSVC)
        option(RUNTIME_STATIC_LINKAGE "Link with runtime library statically" OFF)
    endif(MSVC)

    # If the runtime static linkage option is turned on, replace
    # compiler flag that selects runtime library variant.
    if(MSVC AND RUNTIME_STATIC_LINKAGE)
        foreach(flag
            CMAKE_C_FLAGS
            CMAKE_CXX_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
        )
            if(${flag} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
            endif(${flag} MATCHES "/MD")
        endforeach(flag)
    endif(MSVC AND RUNTIME_STATIC_LINKAGE)

    # Keep Google Test's linkage of the runtime in sync with our own.
    if(MSVC)
        if(RUNTIME_STATIC_LINKAGE)
            set(gtest_force_shared_crt OFF CACHE BOOL "Use shared (DLL) run-time lib even when Google Test is built as static lib." FORCE)
        else(RUNTIME_STATIC_LINKAGE)
            set(gtest_force_shared_crt ON CACHE BOOL "Use shared (DLL) run-time lib even when Google Test is built as static lib." FORCE)
        endif(RUNTIME_STATIC_LINKAGE)
    endif(MSVC)

    # Organize applications, libraries, etc. in folders in the IDE.
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Ensure proper loading of side-loaded shared libraries
    # on all targets.
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    if(APPLE)
        set(CMAKE_INSTALL_RPATH "@loader_path")
    elseif(UNIX)
        set(CMAKE_INSTALL_RPATH "\$ORIGIN")
    endif()

    # Don't add the goofy "d" postfix for debug builds of libraries.
    set(CMAKE_DEBUG_POSTFIX "")

    # Suppress Microsoft warnings about functions being unsafe.
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

    # Specify where to find the AWS Signature Version 4 Test Suite.
    set(AWS_SIG_4_TEST_SUITE "${CMAKE_CURRENT_SOURCE_DIR}/aws-sig-v4-test-suite")

    # Google Test
    set(BUILD_GTEST ON CACHE BOOL "Builds the googletest subproject" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject" FORCE)
    add_subdirectory(googletest)
    set_target_properties(gtest gtest_main PROPERTIES
        FOLDER Libraries
    )

    # LibreSSL
    set(LIBRESSL_SKIP_INSTALL ON CACHE BOOL "Skip installation" FORCE)
    set(LIBRESSL_APPS OFF CACHE BOOL "Build apps" FORCE)
    set(LIBRESSL_TESTS OFF CACHE BOOL "Build tests" FORCE)
    set(ENABLE_NC OFF CACHE BOOL "Enable installing TLS-enabled nc(1)" FORCE)
    set(LIBRESSL_UNINSTALL OFF CACHE BOOL "Define uninstall target" FORCE)
    add_subdirectory(LibreSSL)
    set_target_properties(tls ssl crypto PROPERTIES
        FOLDER Libraries/LibreSSL
    )

    # zlib
    set(ZLIB_EXAMPLES OFF CACHE BOOL "Build zlib example programs" FORCE)
    add_subdirectory(zlib)
    foreach(Child zlib zlibstatic)
        set_target_properties(${Child} PROPERTIES FOLDER Libraries/zlib)
        target_include_directories(${Child} INTERFACE $<TARGET_PROPERTY:zlib,SOURCE_DIR> $<TARGET_PROPERTY:zlib,BINARY_DIR>)
    endforeach()

    # All other libraries can be pulled in without further configuration.
    add_subdirectory(Aws)
    add_subdirectory(Base64)
    add_subdirectory(Hash)
    add_subdirectory(Http)
    add_subdirectory(HttpNetworkTransport)
    add_subdirectory(Json)
    add_subdirectory(Lurker)
    add_subdirectory(MathBot2001)
    add_subdirectory(MessageHeaders)
    add_subdirectory(Rover)
    add_subdirectory(SystemAbstractions)
    add_subdirectory(TlsDecorator)
    add_subdirectory(Twitch)
    add_subdirectory(TwitchNetworkTransport)
    add_subdirectory(Uri)
    add_subdirectory(Utf8)
    add_subdirectory(WebServer)
    add_subdirectory(WebSockets)
endif(ParentDirectory STREQUAL "")

# Add subdirectories directly in this repository.
add_subdirectory(ZlibPlay)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

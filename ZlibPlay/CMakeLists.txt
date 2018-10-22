# CMakeLists.txt for ZlibPlay
#
# © 2018 by Richard Walters

cmake_minimum_required(VERSION 3.8)
set(This ZlibPlay)

set(Sources
    src/main.cpp
)

add_executable(${This} ${Sources})
set_target_properties(${This} PROPERTIES
    FOLDER Applications
)

target_link_libraries(${This} PUBLIC
    zlibstatic
)

if(UNIX AND NOT APPLE)
    target_link_libraries(${This} PRIVATE
        -static-libstdc++
    )
endif(UNIX AND NOT APPLE)

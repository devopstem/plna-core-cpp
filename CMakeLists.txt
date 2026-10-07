cmake_minimum_required(VERSION 3.22.1)

project("plna")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(core/include)

add_library(
        plna
        SHARED
        core/src/map_geometry.cpp
        core/src/map_scene.cpp
        core/src/socket_client.cpp
        platform/android/jni_map_bridge.cpp
        platform/android/jni_network_bridge.cpp
)

target_compile_options(plna PRIVATE -Wall -Wextra -O3)

find_library(log-lib log)

target_link_libraries(
        plna
        ${log-lib}
)

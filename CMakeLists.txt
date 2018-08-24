cmake_minimum_required(VERSION 3.12)
project(raptor_new)

set(CMAKE_CXX_STANDARD 17)
if (APPLE)
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif ()

add_subdirectory(cpr)

include_directories(${CPR_INCLUDE_DIRS} headers)

add_executable(raptor_new main.cpp)
target_link_libraries(raptor_new ${CPR_LIBRARIES})

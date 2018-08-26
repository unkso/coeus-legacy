cmake_minimum_required(VERSION 3.12)
project(coeus)

set(CMAKE_CXX_STANDARD 17)
if (APPLE)
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif ()

add_subdirectory(cpr)

include_directories(${CPR_INCLUDE_DIRS} headers)

add_executable(coeus main.cpp headers/utilities.hpp headers/headers.hpp utilities/utilities.cpp headers/bf1.hpp bf1/bf1.cpp headers/data_fetcher.hpp headers/bf4.hpp bf4/bf4.cpp headers/siege.hpp siege/siege.cpp headers/coeus.hpp raptor/coeus.cpp headers/interface.hpp interface/interface.cpp)
target_link_libraries(coeus ${CPR_LIBRARIES})

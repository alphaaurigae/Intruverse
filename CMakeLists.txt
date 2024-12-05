
cmake_minimum_required(VERSION 3.10)


project(Intruverse VERSION 888.0)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)


find_program(CXX_COMPILER NAMES g++-11 g++-12 g++-13 g++-14 PATHS /usr/bin /usr/local/bin)


if(NOT CXX_COMPILER)
    message(FATAL_ERROR "GCC 11.0 or higher is required")
endif()


execute_process(
    COMMAND ${CXX_COMPILER} --version
    OUTPUT_VARIABLE GCC_VERSION_OUTPUT
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
)


string(REGEX MATCH "([0-9]+\\.[0-9]+)" GCC_VERSION ${GCC_VERSION_OUTPUT})


if(GCC_VERSION VERSION_LESS "11.0.0")
    message(FATAL_ERROR "GCC 11.0 or higher is required, found version: ${GCC_VERSION}")
endif()


set(CMAKE_CXX_COMPILER ${CXX_COMPILER})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wpedantic -fPIC -pthread")


set(LIBS
uv
ssl
crypto
boost_system
boost_thread
boost_filesystem
)


set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(BUILD_DIR ${CMAKE_BINARY_DIR}/build)
set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin) 


file(GLOB_RECURSE SRCS ${SOURCE_DIR}/*.cpp)


include_directories(/usr/include)
include_directories(/usr/include/asio)

set(COMMON_SOURCES ${SOURCE_DIR}/calculateChecksum.cpp)


set(CLIENT_SOURCES ${SOURCE_DIR}/client.cpp ${COMMON_SOURCES})
set(SERVER_SOURCES ${SOURCE_DIR}/server.cpp ${COMMON_SOURCES})


add_executable(client ${CLIENT_SOURCES})
target_link_libraries(client PRIVATE ${LIBS} pthread)


add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE ${LIBS} pthread)


set_target_properties(client server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)


file(MAKE_DIRECTORY ${BUILD_DIR})
file(MAKE_DIRECTORY ${BIN_DIR})


add_custom_target(clear_build
    COMMAND rm -rf ${BUILD_DIR} ${BIN_DIR}
    COMMENT "Clean build and bin directories"
)



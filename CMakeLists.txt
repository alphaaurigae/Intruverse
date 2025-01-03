# Multi bin template


cmake_minimum_required(VERSION 3.28.3)
project(Intruverse VERSION 888.0)


set(MY_PROJECT_NAME "Intruverse")
#set(MY_PROJECT_BIN1_NAME "client")
#set(MY_PROJECT_BIN2_NAME "server")
set(MY_PROJECT_VERSION "888.0")


project(${MY_PROJECT_NAME} VERSION ${MY_PROJECT_VERSION} LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Compiler choice ---------------------------------------------------------------------------------------------------------------------------
option(USE_CLANG "Clang as compiler instead of g++" ON)

if(USE_CLANG)
    find_program(CXX_COMPILER NAMES clang++-14 clang++-15 clang++-16 clang++-17 clang++-18 PATHS /usr/bin /usr/local/bin)
else()
    find_program(CXX_COMPILER NAMES g++-11 g++-12 g++-13 g++-14 PATHS /usr/bin /usr/local/bin)
endif()


if(NOT CXX_COMPILER)
    message(FATAL_ERROR "Compiler not found! Please install Clang > 14 or GCC > 11.0 or higher.")
endif()


execute_process(
    COMMAND ${CXX_COMPILER} --version
    OUTPUT_VARIABLE COMPILER_VERSION_OUTPUT
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
)


string(REGEX MATCH "([0-9]+\\.[0-9]+)" COMPILER_VERSION ${COMPILER_VERSION_OUTPUT})


if(USE_CLANG)
    if(COMPILER_VERSION VERSION_LESS "14.0.0")
        message(FATAL_ERROR "Compiler version Clang >= 14.0.0 is required, found version: ${COMPILER_VERSION}")
    endif()
else()
    if(COMPILER_VERSION VERSION_LESS "11.0.0")
        message(FATAL_ERROR "Compiler version GCC >= 11.0.0 is required, found version: ${COMPILER_VERSION}")
    endif()
endif()


execute_process(
    COMMAND ${CXX_COMPILER} -v
    OUTPUT_VARIABLE COMPILER_OUTPUT
    ERROR_VARIABLE COMPILER_OUTPUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)


if(COMPILER_OUTPUT MATCHES "clang")
    set(CMAKE_CXX_COMPILER_ID "Clang")
elseif(COMPILER_OUTPUT MATCHES "gcc")
    set(CMAKE_CXX_COMPILER_ID "GNU")
else()
    message(FATAL_ERROR "Unknown compiler detected in: ${COMPILER_OUTPUT}")
endif()


set(CMAKE_CXX_COMPILER ${CXX_COMPILER})


# Compiler flags main -----------------------------------------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()


set(COMPILER_FLAGS_BASE "-Wall -Wextra -Wpedantic -fPIC -pthread")
set(COMPILER_FLAGS_DEBUG "-O0 -g")
set(COMPILER_FLAGS_RELEASE "-O3")
set(COMPILER_FLAGS_RELWITHDEBINFO "-O2 -g")
set(COMPILER_FLAGS_MINSIZEREL "-Os")


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${COMPILER_FLAGS_BASE} ${COMPILER_FLAGS_DEBUG}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${COMPILER_FLAGS_BASE} ${COMPILER_FLAGS_RELEASE}")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${COMPILER_FLAGS_BASE} ${COMPILER_FLAGS_RELWITHDEBINFO}")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(CMAKE_CXX_FLAGS "${COMPILER_FLAGS_BASE} ${COMPILER_FLAGS_MINSIZEREL}")
endif()


# Compiler flags ON/OFF ---------------------------------------------------------------------------------------------------------------------
option(ENABLE_DEPENDENCY_GENERATION "Enable dependency generation compile flags" OFF)
if(ENABLE_DEPENDENCY_GENERATION)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MMD -MP")
endif()

## generalized, needs update, maybe
option(ENABLE_SANITIZERS "Enable sanitizer compile flags for runtime debugging" ON)
# man gcc | grep -A 10 'sanitize'
# clang --help | grep -A 10 'sanitize'
if(ENABLE_SANITIZERS)
    set(COMMON_SANITIZERS
        "undefined"
        "shift"
        "shift-exponent"
        "shift-base"
        "integer-divide-by-zero"
        "vla-bound"
        "null"
        "return"
        "signed-integer-overflow"
        "bounds"
        "alignment"
        "object-size"
        "float-divide-by-zero"
        "float-cast-overflow"
        "nonnull-attribute"
        "returns-nonnull-attribute"
        "bool"
        "enum"
        "vptr"
        "pointer-overflow"
        "builtin"
        "address"
        "leak"
        "pointer-compare"
    )

    set(SANITIZER_FLAGS "")
    foreach(SANITIZER ${COMMON_SANITIZERS})
        list(APPEND SANITIZER_FLAGS "-fsanitize=${SANITIZER}")
    endforeach()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        list(APPEND SANITIZER_FLAGS
            "-fsanitize=shadow-call-stack"
            "-fno-sanitize-address-globals-dead-stripping"
            "-fno-sanitize-address-outline-instrumentation"
            "-fno-sanitize-address-poison-custom-array-cookie"
            "-fno-sanitize-address-use-after-scope"
            "-fno-sanitize-address-use-odr-indicator"
            "-fno-sanitize-cfi-cross-dso"
        )

    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(APPEND SANITIZER_FLAGS
            #"-fsanitize=kernel-address"
            "-fno-sanitize-address-use-after-scope"
            #"-fsanitize-address-use-odr-indicator"
            "-Wno-maybe-uninitialized"  # as, clang ignoring the warnings.
            "-fsanitize=leak"
            "-fsanitize=bounds-strict"
            "-fsanitize=pointer-compare"
        )
    endif()

    string(REPLACE ";" " " SANITIZER_FLAGS "${SANITIZER_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
endif()


option(ENABLE_COMPILE_COMMANDS "Enable generation of compile_commands.json" ON)
if(ENABLE_COMPILE_COMMANDS)
    set(CMAKE_EXPORT_COMPILE_COMMANDS YES CACHE BOOL "Export compile commands" FORCE)
endif()


option(ENABLE_VERBOSE_MAKEFILES "Enable verbose makefiles" ON)
if(ENABLE_VERBOSE_MAKEFILES)
    set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose makefile output" FORCE)
endif()


option(ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
if(ENABLE_WARNINGS_AS_ERRORS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()


option(ENABLE_LOGGING "Enable detailed logging" ON)
if(ENABLE_LOGGING)
    add_definitions(-DENABLE_LOGGING)
else()
    add_definitions(-DDISABLE_LOGGING)
endif()


# Directories -------------------------------------------------------------------------------------------------------------------------------
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(BUILD_DIR ${CMAKE_BINARY_DIR}/build)
set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)


include_directories(
#    /usr/include
    ${SRC_DIR}
    ${SRC_DIR}/shared
    ${SRC_DIR}/client
    ${SRC_DIR}/server
)


#file(GLOB_RECURSE SRCS ${SRC_DIR}/*.cpp)  # GLOB_RECURSE only matches files in the current directory (depth 1)
## file(GLOB_RECURSE SRCS ${SRC_DIR}/**/*.cpp)  # GLOB_RECURSE with '**' matches .cpp files at any depth of subdirectories

# Build -------------------------------------------------------------------------------------------------------------------------------------


set(LIBS_MAIN
uv
ssl
crypto
boost_system
boost_thread
boost_filesystem
pthread
)


#set(LIBS_CLIENT
#)


#set(LIBS_SERVER
#)


option(BUILD_CLIENT "client" ON)
option(BUILD_SERVER "server" ON)


function(add_feature_executable FEATURE_NAME FEATURE_SOURCES)
    string(TOUPPER ${FEATURE_NAME} FEATURE_UPPER)
    set(FEATURE_OPTION "BUILD_${FEATURE_UPPER}")

    if(${${FEATURE_OPTION}})
        add_executable(${FEATURE_NAME} ${FEATURE_SOURCES})
        target_link_libraries(${FEATURE_NAME} PRIVATE ${LIBS_MAIN})

        #if(FEATURE_NAME STREQUAL "client")
        #    target_link_libraries(${FEATURE_NAME} PRIVATE ${LIBS_CLIENT})
        #endif()
        #if(FEATURE_NAME STREQUAL "server")
        #    target_link_libraries(${FEATURE_NAME} PRIVATE ${LIBS_SERVER})
        #endif()

        target_compile_definitions(${FEATURE_NAME} PRIVATE ${FEATURE_UPPER})
        set_target_properties(${FEATURE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
    endif()
endfunction()


add_feature_executable(client ${SRC_DIR}/client/client.cpp)
add_feature_executable(server ${SRC_DIR}/server/server.cpp)


file(MAKE_DIRECTORY ${BUILD_DIR})
file(MAKE_DIRECTORY ${BIN_DIR})


# Clean -------------------------------------------------------------------------------------------------------------------------------------
add_custom_target(my_clean
    COMMAND rm -rf ${BUILD_DIR} ${BIN_DIR}
    COMMENT "Clean build and bin directories"
)


# Print settings info (before build) --------------------------------------------------------------------------------------------------------
message(STATUS "------------------------------------------------------------")
message(STATUS "Project: ${MY_PROJECT_BIN_NAME} (Version: ${MY_PROJECT_VERSION})")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
endif()
message(STATUS "CMake generator: ${CMAKE_GENERATOR}")
message(STATUS "Compiler: ${CXX_COMPILER}")
message(STATUS "Compiler Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Dependency Generation: ${ENABLE_DEPENDENCY_GENERATION}")
message(STATUS "Enable sanitizer compile flags for runtime debugging: ${ENABLE_SANITIZERS}")
message(STATUS "Warnings as Errors: ${ENABLE_WARNINGS_AS_ERRORS}")
message(STATUS "Verbose Makefiles: ${ENABLE_VERBOSE_MAKEFILES}")
message(STATUS "Compile Commands Export: ${ENABLE_COMPILE_COMMANDS}")
message(STATUS "------------------------------------------------------------")
set(ASCII_ART "
     _____                                 
    (, /                                   
      / __  _/_ __     _ _   _  __  _    _ 
  ___/__/ (_(__/ (_(_(_(/___(/_/ (_/_)__(/_
(__ /                                      
                                           
VERSION ${MY_PROJECT_VERSION}
")
message(STATUS "${ASCII_ART}")


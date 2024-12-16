
cmake_minimum_required(VERSION 3.28.3)
project(Intruverse VERSION 888.0)


set(MY_PROJECT_NAME "Intruverse")
#set(MY_PROJECT_BIN1_NAME "slient")
#set(MY_PROJECT_BIN2_NAME "server")
set(MY_PROJECT_VERSION "888.0")


project(${MY_PROJECT_NAME} VERSION ${MY_PROJECT_VERSION} LANGUAGES CXX)


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


option(ENABLE_DEPENDENCY_GENERATION "Enable dependency generation compile flags" OFF)
if(ENABLE_DEPENDENCY_GENERATION)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MMD -MP")
endif()


option(ENABLE_SANITIZERS "Enable sanitizer compile flags for runtime debugging" ON)
if(ENABLE_SANITIZERS)
    set(SANITIZER_FLAGS "-fsanitize=undefined,shift,integer-divide-by-zero,bounds,float-divide-by-zero -fsanitize=shift -fsanitize=shift-exponent -fsanitize=shift-base -fsanitize=integer-divide-by-zero -fsanitize=vla-bound \
  -fsanitize=null -fsanitize=return -fsanitize=signed-integer-overflow -fsanitize=bounds -fsanitize=bounds-strict -fsanitize=alignment -fsanitize=object-size \
  -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize=nonnull-attribute -fsanitize=returns-nonnull-attribute -fsanitize=bool -fsanitize=enum \
  -fsanitize=vptr -fsanitize=pointer-overflow -fsanitize=builtin   ") # -fsanitize=thread  -fsanitize=leak -fsanitize=pointer-compare  -fsanitize=address
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
endif()


option(ENABLE_COMPILE_COMMANDS "Enable generation of compile_commands.json" OFF)
if(ENABLE_COMPILE_COMMANDS)
    set(CMAKE_EXPORT_COMPILE_COMMANDS YES CACHE BOOL "Export compile commands" FORCE)
endif()


option(ENABLE_VERBOSE_MAKEFILES "Enable verbose makefiles" OFF)
if(ENABLE_VERBOSE_MAKEFILES)
    set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose makefile output" FORCE)
endif()


option(ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
if(ENABLE_WARNINGS_AS_ERRORS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()


option(ENABLE_LOGGING "Enable detailed logging" OFF)
if(ENABLE_LOGGING)
    add_definitions(-DENABLE_LOGGING)
else()
    add_definitions(-DDISABLE_LOGGING)
endif()


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


add_custom_target(clear_build
    COMMAND rm -rf ${BUILD_DIR} ${BIN_DIR}
    COMMENT "Clean build and bin directories"
)


message(STATUS "------------------------------------------------------------")
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
message(STATUS "Enable Morse: ${BUILD_CLIENT}")
message(STATUS "Enable Hex: ${BUILD_SERVER}")
set(ASCII_ART "
     _____                                 
    (, /                                   
      / __  _/_ __     _ _   _  __  _    _ 
  ___/__/ (_(__/ (_(_(_(/___(/_/ (_/_)__(/_
(__ /                                      
                                           
VERSION ${MY_PROJECT_VERSION}
")
message(STATUS "${ASCII_ART}")


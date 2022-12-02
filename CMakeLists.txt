CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
set(CSTAR_VERSION 0.1.0 CACHE STRING "The version of cstar")

set(CMAKE_C_COMPILER "gcc-11")
set(CMAKE_CXX_COMPILER "g++-11")

project(cstar
        VERSION ${CSTAR_VERSION}
        DESCRIPTION "C-star is a compiler for a language adding syntatic sugar to C"
        LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-writable-strings")

if (NOT CMAKE_BUILD_TYPE)
    # default build type is Debug
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(ENABLE_UNIT_TESTS    "Enable building of unit tests" ON)

# Configure path for loading project cmake scripts
set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include_directories(
        include
        ${CMAKE_CURRENT_BINARY_DIR})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions("-DCXY_DEBUG_EXEC_TRACE=0" "-DCXY_DEBUG_PRINT_STACK=1")
endif()

include(ExternalProject)

ExternalProject_Add(qbe
        DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}
        GIT_REPOSITORY git://c9x.me/qbe.git
        UPDATE_COMMAND ""
        SOURCE_DIR ${qbe_SOURCE_DIR}
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ""
        BUILD_COMMAND make -j4
        INSTALL_COMMAND make install DESTDIR=${CMAKE_BINARY_DIR}/qbe
        )

set(CXY_COMPILER_SOURCES
        src/compiler/ast.cpp
        src/compiler/builtin.cpp
        src/compiler/codegen.cpp
        src/compiler/encoding.cpp
        src/compiler/dump.cpp
        src/compiler/lexer.cpp
        src/compiler/log.cpp
        src/compiler/node.cpp
        src/compiler/parser.cpp
        src/compiler/source.cpp
        src/compiler/strings.cpp
        src/compiler/symbol.cpp
        src/compiler/token.cpp
        src/compiler/types.cpp
        src/compiler/utils.cpp)

add_library(cstar-lib STATIC
        ${CXY_COMPILER_SOURCES})

add_executable(cstar
        src/compiler/main.cpp)

add_dependencies(cstar cstar-lib)

include_directories(include)

if (ENABLE_UNIT_TESTS)
    include(Catch.cmake)
    add_executable(cstar-unit-test
            tests/main.cpp
            ${CXY_COMPILER_SOURCES})

    target_include_directories(cstar-unit-test PRIVATE src)
    add_dependencies(cstar-unit-test catch)

    add_executable(cstar-lang-test-lexer
            tests/lang/lexer.cpp)
    target_link_libraries(cstar-lang-test-lexer cstar-lib)
    target_compile_definitions(cstar-lang-test-lexer PRIVATE
            "-DCSTAR_LANG_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/tests/lang\"")

    add_executable(cstar-lang-test-parser
            tests/lang/parser.cpp)
    target_link_libraries(cstar-lang-test-parser cstar-lib)
    target_compile_definitions(cstar-lang-test-parser PRIVATE
            "-DCSTAR_LANG_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/tests/lang\"")
endif()

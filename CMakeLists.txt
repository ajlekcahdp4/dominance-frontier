cmake_minimum_required(VERSION 3.13)
project(dominance-frontier)

set (CMAKE_CXX_STANDARD 20)


option(INSOURCEBUILD OFF) # Prevent in-source style builds
if((${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR}) AND NOT
                                                              ${INSOURCEBUILD})
    message(
        FATAL_ERROR
            "In-source building disabled. Provide -DINSOURCEBUILD option if you are sure about it."
    )
endif()

option(SANITIZE OFF)
if(${SANITIZE})
    add_compile_options(-fsanitize=undefined,address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=undefined,address -fno-omit-frame-pointer)
endif()

option(THREAD_SANITIZE OFF)
if(${THREAD_SANITIZE})
    add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
    add_link_options(-fsanitize=thread -fno-omit-frame-pointer)
endif()

add_compile_options (-fno-rtti)

if(THREAD_SANITIZE AND SANITIZE)
    message(FATAL_ERROR "Thread and address sanitizer can't be used together")
endif()

include(cmake/functions.cmake)
include(cmake/dependencies.cmake)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

bison_target(parser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.y.cc DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/graph_parser.h)
flex_target(scanner src/scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.l.cc DEFINES_FILE)
add_flex_bison_dependency(scanner parser)

add_library(bison_parser STATIC ${BISON_parser_OUTPUTS} ${FLEX_scanner_OUTPUTS})
target_include_directories(bison_parser PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})

set(DOMFRONT_SOURCES
    src/main.cc 
    src/ReducibleGraph.cc
    src/Dominators.cc
    )

add_executable(domfront ${DOMFRONT_SOURCES})
target_enable_linter(domfront)
target_include_directories(domfront PUBLIC include)
target_compile_features(domfront PUBLIC cxx_std_20)
target_link_libraries(domfront PRIVATE bison_parser INTERFACE popl)
enable_warnings(domfront)

install (TARGETS domfront DESTINATION ${CMAKE_INSTALL_BINDIR})

option(BUILD_DOCS OFF)
if(BUILD_DOCS)
    find_package(Doxygen REQUIRED OPTIONAL_COMPONENTS mscgen dia)

    doxygen_add_docs(${CMAKE_PROJECT}_docs include ALL)
endif()

cmake_minimum_required(VERSION 3.13)
project(dominance-frontier)

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

if(THREAD_SANITIZE AND SANITIZE)
    message(FATAL_ERROR "Thread and address sanitizer can't be used together")
endif()

include(cmake/functions.cmake)
include(cmake/dependencies.cmake)

add_subdirectory(graphs)

set(DOMFRONT_SOURCES
    src/main.cc 
    src/ReducibleGraph.cc)

add_executable(domfront ${DOMFRONT_SOURCES})
target_enable_linter(domfront)
target_include_directories(domfront PUBLIC include)
target_link_libraries(domfront  graphs)
target_compile_features(domfront PUBLIC cxx_std_20)
enable_warnings(domfront)

option(BUILD_DOCS OFF)
if(BUILD_DOCS)
    find_package(Doxygen REQUIRED OPTIONAL_COMPONENTS mscgen dia)

    doxygen_add_docs(${CMAKE_PROJECT}_docs include ALL)
endif()

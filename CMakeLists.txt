cmake_minimum_required(VERSION 3.13)
project(dominance-frontier)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
llvm_map_components_to_libnames(llvm_libs support core irreader)

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
    src/reducible_graph.cc
    src/main.cc
)

function(add_llvm_executable TARGET_NAME SOURCE)
    add_executable(${TARGET_NAME} ${SOURCE})
    target_enable_linter(${TARGET_NAME})
    target_include_directories(${TARGET_NAME} PUBLIC ${LLVM_INCLUDE_DIRS} include)
    target_link_libraries(${TARGET_NAME} ${llvm_libs} graphs)
    target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)
    enable_warnings(${TARGET_NAME})
endfunction()

add_llvm_executable(domfront ${DOMFRONT_SOURCES})

option(BUILD_DOCS OFF)
if(BUILD_DOCS)
    find_package(Doxygen REQUIRED OPTIONAL_COMPONENTS mscgen dia)

    doxygen_add_docs(${CMAKE_PROJECT}_docs include ALL)
endif()

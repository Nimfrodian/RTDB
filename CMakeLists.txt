if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.12)
    project(RTDB LANGUAGES C CXX)
    set(RTDB_STANDALONE_DEFAULT ON)
else()
    set(RTDB_STANDALONE_DEFAULT OFF)
endif()

option(RTDB_BUILD_TESTS "Build RTDB unit tests" ${RTDB_STANDALONE_DEFAULT})
option(RTDB_ENABLE_COVERAGE "Enable coverage for RTDB and its tests" ${RTDB_STANDALONE_DEFAULT})

set(RTDB_VARS_INCLUDE_DIR "" CACHE PATH "Directory containing the consumer-provided rtdb_vars.h")
if (NOT RTDB_VARS_INCLUDE_DIR)
    if (RTDB_BUILD_TESTS AND RTDB_STANDALONE_DEFAULT)
        set(RTDB_VARS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/unit_tests/config")
        message(STATUS "Using the test-only rtdb_vars.h fixture")
    else()
        message(FATAL_ERROR "Set RTDB_VARS_INCLUDE_DIR to the directory containing the consumer-provided rtdb_vars.h")
    endif()
endif()

if (NOT EXISTS "${RTDB_VARS_INCLUDE_DIR}/rtdb_vars.h")
    message(FATAL_ERROR "${RTDB_VARS_INCLUDE_DIR} does not contain rtdb_vars.h")
endif()

# --------------------------------------------------
# RTDB library (production code)
# --------------------------------------------------

add_library(rtdb STATIC
    src/rtdb.cpp
)

target_include_directories(rtdb PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${RTDB_VARS_INCLUDE_DIR}
)
target_compile_features(rtdb PUBLIC cxx_std_17)

if (RTDB_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "RTDB coverage enabled")
    target_compile_options(rtdb PRIVATE --coverage -O0 -g)
endif()

if (NOT RTDB_BUILD_TESTS)
    return()
endif()

if (NOT RTDB_STANDALONE_DEFAULT)
    message(FATAL_ERROR "RTDB_BUILD_TESTS is intended for standalone RTDB builds")
endif()

# --------------------------------------------------
# Unity framework
# --------------------------------------------------

set(UNITY_SRC
    third_party/unity/unity.c
)

# --------------------------------------------------
# Test discovery
# --------------------------------------------------

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    unit_tests/*.cpp
)

enable_testing()

# --------------------------------------------------
# Helper function (one exe per test file)
# --------------------------------------------------

function(add_rtdb_test TEST_FILE)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME}
        ${TEST_FILE}
        ${UNITY_SRC}
    )

    target_link_libraries(${TEST_NAME}
        PRIVATE rtdb
    )

    target_include_directories(${TEST_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/unity
        ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests
    )

    # Ensure coverage flags apply to each test executable
    if (RTDB_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${TEST_NAME} PRIVATE --coverage -O0 -g)
        target_link_options(${TEST_NAME} PRIVATE --coverage)
    endif()

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# --------------------------------------------------
# Create one executable per test file
# --------------------------------------------------

foreach(TEST_FILE ${TEST_SOURCES})
    add_rtdb_test(${TEST_FILE})
endforeach()
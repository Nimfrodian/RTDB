cmake_minimum_required(VERSION 3.12)

project(RTDB_PLAYGROUND LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------
# Coverage option
# --------------------------------------------------

option(ENABLE_COVERAGE "Enable coverage reporting" ON)

if (ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Coverage enabled")

    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

# --------------------------------------------------
# RTDB library (production code)
# --------------------------------------------------

file(GLOB RTDB_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(rtdb STATIC
    ${RTDB_SOURCES}
)

target_include_directories(rtdb PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# IMPORTANT: ensure library is also instrumented
if (ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(rtdb PRIVATE --coverage -O0 -g)
    target_link_options(rtdb PRIVATE --coverage)
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
    if (ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
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
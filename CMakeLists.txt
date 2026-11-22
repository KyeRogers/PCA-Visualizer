
cmake_minimum_required(VERSION 3.20)

project(PCAVisualizer VERSION 1.0 LANGUAGES CXX)

# C++ configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependencies
include(FetchContent)

# Raylib
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)

# GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(googletest)

# Core library
add_library(pca_core
    src/dataset.cpp
    src/statistics.cpp
    src/matrix.cpp
    src/PCA.cpp
    src/PCAResult.cpp
)

target_include_directories(pca_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/lib
)

# Main application
add_executable(pca_visualizer
    src/main.cpp
)

target_link_libraries(pca_visualizer
    PRIVATE
        pca_core
        raylib
)

# Testing
include(CTest)

if(BUILD_TESTING)

    add_executable(pca_tests
        tests/matrix_tests.cpp
        tests/statistics_tests.cpp
        tests/PCA_tests.cpp
    )

    target_link_libraries(pca_tests
        PRIVATE
            pca_core
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(pca_tests)

endif()

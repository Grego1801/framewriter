cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(FrameWriter VERSION 1.0.0)

add_library(${PROJECT_NAME} SHARED src/main.cpp)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "GEODE_SDK not defined")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)
target_link_libraries(${PROJECT_NAME} geode-sdk)

# Add Eclipse include path
CPMAddPackage(
    NAME eclipse-menu
    GITHUB_REPOSITORY EclipseMenu/EclipseMenu
    GIT_TAG main
    DOWNLOAD_ONLY YES
)
if(eclipse-menu_ADDED)
    target_include_directories(${PROJECT_NAME} PRIVATE 
        ${eclipse-menu_SOURCE_DIR}/include)
endif()

setup_geode_mod(${PROJECT_NAME})

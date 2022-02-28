cmake_minimum_required(VERSION 3.16)

set(TRACKMAN_JUCE_LOCATION "${CMAKE_SOURCE_DIR}/libs/JUCE" CACHE STRING "Path to JUCE library source tree")
option(TRACKMAN_SYSTEM_JUCE "Use system installation of juce" OFF)
option(TRACKMAN_DEVENDORED_SYSTEM_JUCE "Use system libraries when using system installation of juce" OFF)

# global CMake options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT Trackman)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11 CACHE STRING "Minimum macOS version")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif ()

# set the project name
project(Trackman VERSION 0.1.0)

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
math(EXPR TARGET_BITNESS "${CMAKE_SIZEOF_VOID_P} * 8" OUTPUT_FORMAT DECIMAL)
message(STATUS "Targeting ${TARGET_BITNESS} bit configuration")

# Global compile options
add_compile_options(
    # Build with Multiple Processes on Visual Studio
    $<$<CXX_COMPILER_ID:MSVC>:/MP>
    # Set source and executable charsets to UTF-8. Required for building on CJK Windows.
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    )
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(WIN32 AND ${TARGET_BITNESS} EQUAL 32)
    add_link_options(
        # Windows 32-bit: Extended address space
        $<$<CXX_COMPILER_ID:MSVC>:/LARGEADDRESSAWARE>
        $<$<BOOL:${MINGW}>:LINKER:--large-address-aware>
        )
endif()

# add JUCE libraries
if(NOT TRACKMAN_SYSTEM_JUCE)
  message(STATUS "Using JUCE from ${TRACKMAN_JUCE_LOCATION}")
  add_subdirectory(${TRACKMAN_JUCE_LOCATION} ${CMAKE_BINARY_DIR}/JUCE EXCLUDE_FROM_ALL)
else()
  message(STATUS "Using system provided JUCE")
  find_package(JUCE COMPONENTS REQUIRED)
  if(TRACKMAN_DEVENDORED_SYSTEM_JUCE)
    message(STATUS "Using devendored system provided JUCE")
    include(FindPkgConfig)
    pkg_search_module(flac REQUIRED flac)
    pkg_search_module(ogg REQUIRED ogg)
    pkg_search_module(vorbis REQUIRED vorbis vorbisenc vorbisfile)
  endif()
endif()

add_subdirectory(libs)
add_subdirectory(src)

target_include_directories(Trackman PUBLIC "${PROJECT_BINARY_DIR}")

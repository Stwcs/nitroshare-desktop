# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.2.0)
# 项目信息
project(nitroshare)


# 指向自定义模块
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


# 定义 Linux 常量
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX 1)
endif()


# 项目相关信息
set(PROJECT_NAME "NitroShare")
set(PROJECT_DESCRIPTION "Cross-platform network file transfer application")
set(PROJECT_AUTHOR "Nathan Osman")
set(PROJECT_URL "https://nitroshare.net")


# 项目版本号
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 4)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# At the very minimum, Qt 5.4+ is required
find_package(Qt5Network 5.4 REQUIRED)
find_package(Qt5Widgets 5.4)

# CLI and client are always available, UI is available if Qt5Widgets is available
option(BUILD_CLI "Build the NitroShare CLI app" ON)
option(BUILD_NMH "Build native messaging host for Chrome" ON)
option(BUILD_CLIENT "Build the NitroShare client app" ON)
if(Qt5Widgets_FOUND)
    option(BUILD_UI "Build the NitroShare UI app" ON)
endif()
if(WIN32)
    option(BUILD_NITROSHELL "Build shell extension for Windows Explorer" ON)
endif()

option(ENABLE_TLS "Enable support for TLS" ON)

# Allow file installation directories to be customized
set(INSTALL_BIN_PATH bin CACHE STRING "Application installation directory")
set(INSTALL_LIB_PATH lib CACHE STRING "Shared library installation directory")
set(INSTALL_INCLUDE_PATH include CACHE STRING "Header installation directory")
set(INSTALL_PLUGIN_PATH "${INSTALL_LIB_PATH}/nitroshare/plugins" CACHE STRING "Plugin installation directory")

set(OUTPUT_PREFIX "${CMAKE_BINARY_DIR}/out")

# Ensure the output prefix supports multiple build configurations
if(CMAKE_CONFIGURATION_TYPES)
    set(OUTPUT_PREFIX "${OUTPUT_PREFIX}/$<CONFIG>")
endif()

# For macOS, the output directories form the app bundle; for all other
# platforms, the output paths mirror the installation paths
if(APPLE)
    set(CONTENTS_DIRECTORY "${OUTPUT_PREFIX}/NitroShare.app/Contents")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CONTENTS_DIRECTORY}/MacOS")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CONTENTS_DIRECTORY}/Frameworks")
    set(PLUGIN_OUTPUT_DIRECTORY "${CONTENTS_DIRECTORY}/PlugIns/nitroshare")
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_PREFIX}/${INSTALL_BIN_PATH}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_PREFIX}/${INSTALL_LIB_PATH}")
    set(PLUGIN_OUTPUT_DIRECTORY "${OUTPUT_PREFIX}/${INSTALL_PLUGIN_PATH}")
endif()

# Set appropriate compiler flags, etc. (MSVC is already set to W3 and Clang
# issues enough warnings as it is)
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Documentation is disabled by default
option(BUILD_DOC "Build Doxygen documentation" OFF)
if(BUILD_DOC)
    find_package(Doxygen REQUIRED)
    option(DOWNLOAD_TAGFILES "Download tagfiles for documentation" ON)
endif()

# Test suite is always disabled by default
option(BUILD_TESTS "Build test suite" OFF)
if(BUILD_TESTS)
    enable_testing()
    find_package(Qt5Test 5.4 REQUIRED)
endif()

# Add a dependency with a builtin fallback
function(add_dependency name title version)
    find_package(${name} ${version} QUIET)
    if(${name}_FOUND)
        message(STATUS "Found ${title} ${${name}_VERSION}")
    else()
        message(STATUS "${title} ${version}+ not found - using bundled version")
        set(_old_BUILD_DOC ${BUILD_DOC})
        set(BUILD_DOC OFF)
        add_subdirectory(deps/${name})
        set(BUILD_DOC ${_old_BUILD_DOC})
    endif()
endfunction()

# Attempt to find QHttpEngine on the system; use bundled version if missing
option(BUILD_API "Build local API plugin" ON)
if(BUILD_API)
    add_dependency(qhttpengine QHttpEngine 1.0.0)
endif()

# Attempt to find QMdnsEngine on the system; use bundled version if missing
option(BUILD_MDNS "Build mDNS discovery plugin" ON)
if(BUILD_MDNS)
    add_dependency(qmdnsengine QMdnsEngine 0.1.0)
endif()

# Enable libnotify support if GTK and libnotify are present
if(LINUX)
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(libnotify gtk+-2.0 libnotify)
    endif()
endif()

# Ensure that binaries are built with the RPATH set correctly
set(CMAKE_MACOSX_RPATH ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# The library and plugins are always built
add_subdirectory(dist)
add_subdirectory(libnitroshare)
add_subdirectory(plugins)

if(BUILD_CLI)
    add_subdirectory(cli)
endif()

if(BUILD_CLIENT)
    add_subdirectory(client)
endif()

if(BUILD_NMH)
    add_subdirectory(nmh)
endif()

if(BUILD_UI)
    add_subdirectory(ui)
endif()

if(WIN32 AND BUILD_NITROSHELL)
    add_subdirectory(shell)
endif()

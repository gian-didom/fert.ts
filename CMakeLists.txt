cmake_minimum_required (VERSION 3.10.0 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

# Define project
project(FERT
        VERSION 0.9.9
        DESCRIPTION "Fast Ephemeris Retrieval Tool"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
include(cmakeUtils)

# Compiler options
# Cross-platform SIMD support - let the abstraction layer handle architecture specifics
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    # On native x86_64, we can use AVX2 optimizations
    add_compile_options("-mavx2" "-mfma")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    # On ARM64 (Apple Silicon), we rely on the SIMD abstraction layer
    # No architecture-specific flags needed
endif()
set(CXX_FLAGS_RELEASE "-O2 -fPIC -funroll-loops -DNDEBUG -D_GLIBCXX_PARALLEL")
set(CXX_FLAGS_DEBUG "-g")

# Set the type of build
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for each cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
               "Debug" "Release")
endif()

# Project organized by folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set Intel Compiler if possible
#find_package(IntelDPCPP)
find_package(IntelDPCPP NO_MODULE QUIET)
if(IntelDPCPP_NOT_FOUND_MESSAGE)
  message(WARNING "Intel Environment set up, but the following error has occured:\n ${SYCL_REASON_FAILURE}")
  message(STATUS "Moving to default compiler\n")
endif()
if(NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    message(WARNING "Select Intel compiler for increased performances")
    add_compile_options("-Wno-ignored-attributes")
endif()

message(STATUS "CMAKE_SYSTEM_NAME          = ${CMAKE_SYSTEM_NAME}"         )
message(STATUS "CMAKE_CXX_COMPILER_ID      = ${CMAKE_CXX_COMPILER_ID}"     )
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_COMPILER_IS_GNUCXX   = ${CMAKE_COMPILER_IS_GNUCXX}"  )
message(STATUS "BUILD TYPE                 = ${CMAKE_BUILD_TYPE}"          )

# YAML-Cpp for file parsing
find_package(YamlCpp 0.6)
message(STATUS "YAMLCPP_FOUND              = ${YAMLCPP_FOUND}" )

# Add subdirectories:
add_subdirectory(modules)
# Commented out Intel-specific linker flags that cause issues on macOS
#if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
#   set_target_properties(fert PROPERTIES LINK_FLAGS "-Wl,-rpath=${SYCL_LIBRARY_DIR}:${SYCL_LIBRARY_DIR}/../compiler/lib/intel64_lin")
#endif()


# Add test subdirectory and enable tests
if(BUILD_TESTING)
    message(STATUS "Compiling with unit tests" )
    # find_package(CSPICE REQUIRED)
    # message(STATUS "CSPICE_FOUND               = ${CSPICE_FOUND}" )
    find_package(CPPUNIT 1.14.0)
    message(STATUS "CPPUNIT_FOUND              = ${CPPUNIT_FOUND}" )
    if(${CPPUNIT_FOUND})
    include(CTest)
        add_subdirectory(tests)
    else()
        message(SEND_ERROR "Cannot build unit tests, required dependency CPPunit not found.")
    endif()
endif()

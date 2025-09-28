cmake_minimum_required(VERSION 3.15)
project(fert-node)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Apply CMake configuration as specified
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif()
set(CMAKE_PREFIX_PATH "/usr/local")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64-v3")

# Find cmake-js include and source files
if(DEFINED CMAKE_JS_INC)
    include_directories(${CMAKE_JS_INC})
endif()

if(DEFINED CMAKE_JS_SRC)
    set(CMAKE_JS_SOURCES ${CMAKE_JS_SRC})
else()
    set(CMAKE_JS_SOURCES "")
endif()

# Include directories
include_directories(../modules/fert)
include_directories(../external)

# Check if parent fert library exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../build/modules/fert/libfert.dylib")
    # Link to existing library
    set(FERT_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/../build/modules/fert/libfert.dylib")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../modules/fert/CMakeLists.txt")
    # Build fert library as subdirectory
    add_subdirectory(../modules/fert ${CMAKE_CURRENT_BINARY_DIR}/fert)
    set(FERT_LIBRARY fert)
else()
    message(FATAL_ERROR "FERT library not found. Please build the main project first.")
endif()

# Define source files
file(GLOB SOURCE_FILES "src/*.cpp")

# Create the addon
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SOURCES})

# Link libraries
if(DEFINED CMAKE_JS_LIB)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${FERT_LIBRARY})
else()
    target_link_libraries(${PROJECT_NAME} ${FERT_LIBRARY})
endif()

# Set properties
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
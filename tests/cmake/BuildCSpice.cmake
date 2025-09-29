

include(ExternalProject)

set(CSPICE_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/../external/cspice)
# Debug: print the source directory
message(STATUS "CSPICE_SRC_DIR = ${CSPICE_SRC_DIR}")

set(CSPICE_LIB_DIR ${CSPICE_SRC_DIR}/lib)
set(CSPICE_INCLUDE_DIR ${CSPICE_SRC_DIR}/include)

if(WIN32)
    set(CSPICE_BUILD_COMMAND ${CSPICE_SRC_DIR}/src/mk_static.bat)
    set(CSPICE_LIBRARY ${CSPICE_LIB_DIR}/cspice.lib)
elseif(APPLE)
    # Set architecture-specific compiler options for macOS
    if(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64")
        set(CSPICE_COMPILE_OPTIONS "-m64 -arch x86_64 -c -ansi -O2 -fPIC -I../include -DNON_UNIX_STDIO -Wno-parentheses -Wno-shift-op-parentheses -Wno-logical-op-parentheses -Wno-bitwise-op-parentheses -Wno-dangling-else -Wno-format")
        set(CSPICE_LINK_OPTIONS "-m64 -arch x86_64 -lm")
    elseif(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
        set(CSPICE_COMPILE_OPTIONS "-c -ansi -O2 -fPIC -I../include -DNON_UNIX_STDIO -Wno-parentheses -Wno-shift-op-parentheses -Wno-logical-op-parentheses -Wno-bitwise-op-parentheses -Wno-dangling-else -Wno-format")
        set(CSPICE_LINK_OPTIONS "-lm")
    else()
        # Default options (let compiler decide architecture)
        set(CSPICE_COMPILE_OPTIONS "-m64 -c -ansi -O2 -fPIC -I../include -DNON_UNIX_STDIO -Wno-parentheses -Wno-shift-op-parentheses -Wno-logical-op-parentheses -Wno-bitwise-op-parentheses -Wno-dangling-else -Wno-format")
        set(CSPICE_LINK_OPTIONS "-m64 -lm")
    endif()
    
    set(CSPICE_BUILD_COMMAND cd src && env TKCOMPILEOPTIONS=${CSPICE_COMPILE_OPTIONS} TKLINKOPTIONS=${CSPICE_LINK_OPTIONS} csh mk_mac.csh)
    set(CSPICE_LIBRARY ${CSPICE_LIB_DIR}/libcspice.a)
elseif(UNIX)
    set(CSPICE_BUILD_COMMAND cd src && csh mk_linux.csh)
    set(CSPICE_LIBRARY ${CSPICE_LIB_DIR}/libcspice.a)
endif()

ExternalProject_Add(
    cspice_external
    SOURCE_DIR ${CSPICE_SRC_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CSPICE_BUILD_COMMAND}
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS ${CSPICE_LIBRARY}
)

add_library(cspice STATIC IMPORTED)
set_target_properties(cspice PROPERTIES
    IMPORTED_LOCATION "${CSPICE_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${CSPICE_INCLUDE_DIR}"
)
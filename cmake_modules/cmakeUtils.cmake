function(my_add_library)
    set(options)
    set(oneValueArgs LIB_NAME LIB_TYPE)
    set(multiValueArgs LINK_LIBRARIES INCLUDE_DIRS LIB_PUBLIC_HEADERS LIB_PRIVATE_HEADERS LIB_SOURCES LIB_PRIVATE_SOURCES)
    cmake_parse_arguments(_arg
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    # Set default value for LIBRARY TYPE to SHARED
    if(NOT DEFINED _arg_LIB_TYPE)
        message(STATUS "Creating library of type SHARED as no option LIB_TYPE was specified")
        set(_arg_LIB_TYPE SHARED)
    endif()

    add_library(${_arg_LIB_NAME} ${_arg_LIB_TYPE})

    target_link_libraries(${_arg_LIB_NAME}
        PUBLIC ${_arg_LINK_LIBRARIES}
    )

    set_target_properties(${_arg_LIB_NAME}
        PROPERTIES
            VERSION ${PROJECT_VERSION}
            PUBLIC_HEADER "${_arg_LIB_PUBLIC_HEADERS}"
            PRIVATE_HEADER "${_arg_LIB_PRIVATE_HEADERS}")

    foreach(tmp_LIB_SOURCE ${_arg_LIB_SOURCES})
        set(FULL_PATH_LIB_SOURCES ${FULL_PATH_LIB_SOURCES} $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${tmp_LIB_SOURCE}> )
    endforeach(tmp_LIB_SOURCE)

    target_sources(${_arg_LIB_NAME}
        PUBLIC ${FULL_PATH_LIB_SOURCES}
        PRIVATE ${_arg_LIB_PRIVATE_SOURCES})

    target_include_directories(${_arg_LIB_NAME}
        PUBLIC
            ${_arg_INCLUDE_DIRS}
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${add_interface_library_LIB_NAME}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )

    install(TARGETS  ${_arg_LIB_NAME}
            EXPORT   ${_arg_LIB_NAME}Targets
            LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${_arg_LIB_NAME}
            PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${_arg_LIB_NAME})


    install(EXPORT ${_arg_LIB_NAME}Targets
            FILE ${_arg_LIB_NAME}Targets.cmake
            NAMESPACE ${_arg_LIB_NAME}::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${_arg_LIB_NAME}
    )

endfunction(my_add_library)

function(add_shared_library)

    set(options)
    set(oneValueArgs LIB_NAME)
    set(multiValueArgs LINK_LIBRARIES INCLUDE_DIRS LIB_PUBLIC_HEADERS LIB_PRIVATE_HEADERS LIB_SOURCES LIB_PRIVATE_SOURCES)
    cmake_parse_arguments(_arg
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    my_add_library(LIB_NAME ${_arg_LIB_NAME}
        LIB_TYPE SHARED
        LINK_LIBRARIES ${_arg_LINK_LIBRARIES}
        INCLUDE_DIRS ${_arg_INCLUDE_DIRS}
        LIB_PUBLIC_HEADERS ${_arg_LIB_PUBLIC_HEADERS}
        LIB_PRIVATE_HEADERS ${_arg_LIB_PRIVATE_HEADERS}
        LIB_SOURCES ${_arg_LIB_SOURCES}
        LIB_PRIVATE_SOURCES ${_arg_LIB_PRIVATE_SOURCES})

endfunction(add_shared_library)


function(add_static_library)

    set(options)
    set(oneValueArgs LIB_NAME)
    set(multiValueArgs LINK_LIBRARIES INCLUDE_DIRS LIB_PUBLIC_HEADERS LIB_PRIVATE_HEADERS LIB_SOURCES LIB_PRIVATE_SOURCES)
    cmake_parse_arguments(_arg
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    my_add_library(LIB_NAME ${_arg_LIB_NAME}
        LIB_TYPE STATIC
        LINK_LIBRARIES ${_arg_LINK_LIBRARIES}
        INCLUDE_DIRS ${_arg_INCLUDE_DIRS}
        LIB_PUBLIC_HEADERS ${_arg_LIB_PUBLIC_HEADERS}
        LIB_PRIVATE_HEADERS ${_arg_LIB_PRIVATE_HEADERS}
        LIB_SOURCES ${_arg_LIB_SOURCES}
        LIB_PRIVATE_SOURCES ${_arg_LIB_PRIVATE_SOURCES})

endfunction(add_static_library)


function(add_interface_library)
    set(options)
    set(oneValueArgs LIB_NAME)
    set(multiValueArgs LINK_LIBRARIES INCLUDE_DIRS LIB_PUBLIC_HEADERS LIB_PRIVATE_HEADERS)
    cmake_parse_arguments(_arg
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    add_library(${_arg_LIB_NAME} INTERFACE)

    target_link_libraries(${_arg_LIB_NAME}
        INTERFACE ${_arg_LINK_LIBRARIES}
    )

    target_include_directories(${_arg_LIB_NAME}
        INTERFACE
            ${_arg_INCLUDE_DIRS}
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${_arg_LIB_NAME}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )


    if(CMAKE_VERSION GREATER_EQUAL 3.15)
        set_target_properties(${_arg_LIB_NAME}
            PROPERTIES
                PUBLIC_HEADER "${_arg_LIB_PUBLIC_HEADERS}"
                PRIVATE_HEADER "${_arg_LIB_PRIVATE_HEADERS}"
        )

        install(TARGETS  ${_arg_LIB_NAME}
                EXPORT   ${_arg_LIB_NAME}Targets
                LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
                ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
                PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${_arg_LIB_NAME}
                PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${_arg_LIB_NAME})
    else()

        install(TARGETS  ${_arg_LIB_NAME}
            EXPORT   ${_arg_LIB_NAME}Targets
            LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${_arg_LIB_NAME}
        )

        install(FILES ${_arg_LIB_PUBLIC_HEADERS} ${_arg_LIB_PRIVATE_HEADERS}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${_arg_LIB_NAME})
    endif()

endfunction(add_interface_library)

function(add_tool)
    set(options)
    set(oneValueArgs TOOL_NAME)
    set(multiValueArgs LINK_LIBRARIES INCLUDE_DIRS TOOL_SOURCES)
    cmake_parse_arguments(add_tool
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    add_executable(${add_tool_TOOL_NAME} ${add_tool_TOOL_SOURCES})

    target_include_directories(${add_tool_TOOL_NAME}
        PUBLIC
            ${add_tool_INCLUDE_DIRS}
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_link_libraries(${add_tool_TOOL_NAME}
        PUBLIC
            ${add_tool_LINK_LIBRARIES}
    )

    install(TARGETS  ${add_tool_TOOL_NAME}
            RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT ${CMAKE_INSTALL_INCLUDEDIR}
    )
endfunction(add_tool)


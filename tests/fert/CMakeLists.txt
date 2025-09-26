set(TestList 
    FertTest
    SPKChebTest
    SPK9Test
    SPK21Test
    PCKTest)

foreach(entry IN LISTS TestList)
    add_executable(${entry}
    ${TEST_RUNNER}
    "${entry}.cpp"
    )

    target_link_libraries(${entry}
        ${CPPUNIT_LIBRARIES}
        ${YAMLCPP_LIBRARY}
        ${CSPICE_LIBRARY}
        fert
        )

    target_include_directories(${entry}
        PUBLIC 
        ${CMAKE_SOURCE_DIR}/modules
        ${CSPICE_INCLUDE_DIR}
    )

    add_test(NAME ${entry} COMMAND ${entry} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/fert)
endforeach()
#set_tests_properties(fertTest PROPERTIES ENVIRONMENT "DATADIR=./data")

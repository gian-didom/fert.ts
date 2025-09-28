set(TEST_RUNNER ${CMAKE_CURRENT_SOURCE_DIR}/TestRunner.cpp)

include(cmake/BuildCSpice.cmake)
add_dependencies(cspice cspice_external)
add_subdirectory(fert)

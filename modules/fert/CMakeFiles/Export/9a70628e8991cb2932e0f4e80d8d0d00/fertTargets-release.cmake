#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "fert::fert" for configuration "Release"
set_property(TARGET fert::fert APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(fert::fert PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libfert.so.0.9.9"
  IMPORTED_SONAME_RELEASE "libfert.so.0.9.9"
  )

list(APPEND _cmake_import_check_targets fert::fert )
list(APPEND _cmake_import_check_files_for_fert::fert "${_IMPORT_PREFIX}/lib/libfert.so.0.9.9" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

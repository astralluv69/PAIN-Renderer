#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lib3mf::lib3mf" for configuration ""
set_property(TARGET lib3mf::lib3mf APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(lib3mf::lib3mf PROPERTIES
  IMPORTED_IMPLIB_NOCONFIG "${_IMPORT_PREFIX}/lib/lib3mf.dll.a"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_NOCONFIG "libzip::zip"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/bin/lib3mf.dll"
  )

list(APPEND _cmake_import_check_targets lib3mf::lib3mf )
list(APPEND _cmake_import_check_files_for_lib3mf::lib3mf "${_IMPORT_PREFIX}/lib/lib3mf.dll.a" "${_IMPORT_PREFIX}/bin/lib3mf.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

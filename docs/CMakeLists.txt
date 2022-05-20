include(GNUInstallDirs)

find_program ( DOXYGEN_EXECUTABLE
  NAMES doxygen
  DOC "Doxygen program" )

if ( NOT ( DOXYGEN_EXECUTABLE MATCHES "NOTFOUND" ) )
  message ( "Found doxygen:${DOXYGEN_EXECUTABLE}" )
  set ( top_srcdir "${CMAKE_SOURCE_DIR}" )
  configure_file (
	    "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
	    "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
  )

  file ( READ
  "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    cmake_doxyfile )

  if ( NOT ("${CPACK_PACKAGE_VERSION_PATCH}" STREQUAL "" ) )
    set ( VERSION_EXTRA ".${CPACK_PACKAGE_VERSION_PATCH}" )
  endif ()

  string ( REGEX REPLACE
    "PROJECT_NUMBER         = [0-9].[0-9]"
    "PROJECT_NUMBER         = ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}${VERSION_EXTRA}"
    cmake_doxyfile
    "${cmake_doxyfile}" )

  file ( WRITE
    "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_cmake"
    "${cmake_doxyfile}" )

  add_custom_target ( doxygen
    COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_cmake"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Generating doxygen documentation" )
else ()
  message ( "No doxygen command found. Disable API documentation generation" )
endif ()

# Only useful for make install/package under *nix OSes, but not macOS.
if ( UNIX AND NOT APPLE )
    INSTALL(FILES openxcom.6 DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)
endif ()

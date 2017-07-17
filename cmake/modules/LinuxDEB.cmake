option ( DEBIAN_DEPENDENCYS "Create RPM with dependency rules for Debian" OFF )
set ( CPACK_DEBIAN_PACKAGE_MAINTAINER "The OpenXcom project" )
set ( CPACK_DEBIAN_PACKAGE_SECTION "games" )

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
endif()
if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
endif()

if ( DEBIAN_DEPENDENCYS )
  set ( CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl1.2 (>= 1.2.14-6.1), libsdl-image1.2 (>= 1.2.12), libsdl-gfx1.2 (>= 2.0.20), libsdl-mixer1.2 (>= 1.2.8), libyaml-cpp0.5 (>= 0.5.1) ")
endif ()


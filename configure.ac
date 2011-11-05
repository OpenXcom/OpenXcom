# Process this file with autoconf to create configure.
AC_PREREQ([2.65])

# ====================
# Version informations
# ====================
m4_define([openxcom_version_major],[0])
m4_define([openxcom_version_minor],[3])
m4_define([openxcom_version_micro],[99])
m4_define([openxcom_version],[openxcom_version_major.openxcom_version_minor.openxcom_version_micro])

# =============
# Automake init
# =============
AC_INIT([openxcom],[openxcom_version],[https://github.com/SupSuper/OpenXcom/issues])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11 foreign dist-xz])
AM_SILENT_RULES([yes])
AC_LANG([C++])

# ===========================
# Find required base packages
# ===========================
AC_PROG_CXX
PKG_PROG_PKG_CONFIG([0.24])

# ================
# Check for cflags
# ================
AC_ARG_ENABLE([werror],
	[AS_HELP_STRING([--disable-werror], [Treat all warnings as errors, useful for development])],
	[enable_werror="$enableval"],
	[enable_werror=yes]
)
AS_IF([test x"$enable_werror" != "xno"], [
	CFLAGS="$CFLAGS -Werror"
	CXXFLAGS="$CXXFLAGS -Werror"
])
AS_IF([test x"$GCC" = xyes], [
	# Be tough with warnings and produce less careless code
	CFLAGS="$CXXFLAGS -Wall -pedantic -Weffc++"
	LDFLAGS="$LDFLAGS -Wl,--as-needed"
])

# =============
# Documentation
# =============
AC_ARG_WITH(docs,
	[AS_HELP_STRING([--without-docs], [Do not build documentation])],
	[with_docs="$withval"],
	[AS_IF([test "x$native_win32" = "xyes"], [with_docs=no], [with_docs=yes])]
)
AS_IF([test "x$with_docs" != "xno"], [
	AC_PATH_PROG(DOXYGEN, [doxygen])
	AS_IF([test -z "$DOXYGEN"], [
		AC_MSG_WARN([*** Could not find doxygen in your PATH.])
		AC_MSG_WARN([*** The documentation will not be built.])
		build_docs=no
	], [build_docs=yes])
], [build_docs=no])
AM_CONDITIONAL([WITH_OPENXCOM_DOCS], [test "x$build_docs" != "xno"])

# ==================
# Check dependencies
# ==================
PKG_CHECK_MODULES([SDL],[sdl SDL_mixer SDL_gfx])
PKG_CHECK_MODULES([YAML],[yaml-cpp])

AC_CONFIG_FILES([
	Makefile
	docs/Makefile
	docs/Doxyfile
])
AC_OUTPUT

# ==============================================
# Display final informations about configuration
# ==============================================
AC_MSG_NOTICE([
==============================================================================
Build configuration:
	docs:	${build_docs}
	werror:	${enable_werror}
==============================================================================
])

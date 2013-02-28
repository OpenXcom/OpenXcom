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
	CFLAGS="$CFLAGS -Wall -pedantic"
	LDFLAGS="$LDFLAGS -Wl,--as-needed"
])
AC_CHECK_HEADER(tr1/unordered_map,[AC_DEFINE([_USE_TR1],[1],["Have tr1/unordered_map"])],[])


# ============
# Debug switch
# ============
AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug], [Turn on debugging])],
	[enable_debug="$enableval"],
	[enable_debug=no]
)
AS_IF([test "x$enable_debug" = "xyes"], [
	DEBUG_CFLAGS="-D_DEBUG -g"
])
AC_SUBST(DEBUG_CFLAGS)

# =============
# Documentation
# =============
AC_ARG_WITH(docs,
	[AS_HELP_STRING([--without-docs], [Do not build documentation])],
	[with_docs="$withval"],
	[with_docs=yes]
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
PKG_CHECK_MODULES([SDL],[sdl >= 1.2.13 SDL_mixer >= 1.2.11 SDL_gfx >= 2.0.22 SDL_image >= 1.2])
PKG_CHECK_MODULES([YAML],[yaml-cpp <= 0.3.0])

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
	debug:	${enable_debug}
	docs:	${build_docs}
	werror:	${enable_werror}
==============================================================================
])

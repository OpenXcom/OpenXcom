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

# ==================
# Check dependencies
# ==================
PKG_CHECK_MODULES([SDL],[sdl SDL_mixer SDL_gfx])
PKG_CHECK_MODULES([YAML],[yaml-cpp])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

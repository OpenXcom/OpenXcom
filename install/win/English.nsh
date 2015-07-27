;NSIS OpenXcom Windows Installer Language

!define LANG "ENGLISH" ; Must be the lang name define by NSIS

;--------------------------------
;Pages

!insertmacro LANG_STRING SETUP_XCOM_FOLDER_TITLE "Choose X-COM Location"
!insertmacro LANG_STRING SETUP_XCOM_FOLDER_SUBTITLE "Choose the folders where you have X-COM installed."
!insertmacro LANG_STRING SETUP_XCOM_FOLDER "${GAME_NAME} ${GAME_VERSION} requires a copy of one of the following X-COM games. You can skip this step if you're upgrading an existing installation.$\n$\nSetup will copy the required files from the following folders. To copy from a different folder, click ... and select another folder. Click Next to continue."
!insertmacro LANG_STRING SETUP_UFO_FOLDER "UFO: Enemy Unknown / X-COM: UFO Defense"
!insertmacro LANG_STRING SETUP_TFTD_FOLDER "X-COM: Terror From The Deep"

;--------------------------------
;Installer Sections

!insertmacro LANG_STRING SETUP_GAME "Game Files"
!insertmacro LANG_STRING SETUP_GAME_DESC "Files required to run ${GAME_NAME}."
!insertmacro LANG_STRING SETUP_PATCH "Data Patch"
!insertmacro LANG_STRING SETUP_PATCH_DESC "Fixes errors in the X-COM 1 data. Recommended for first installations. (requires an internet connection)"
!insertmacro LANG_STRING SETUP_PORTABLE "Portable Installation"
!insertmacro LANG_STRING SETUP_PORTABLE_DESC "Puts all ${GAME_NAME} files in the same folder, making a portable installation."
!insertmacro LANG_STRING SETUP_STEAM "Launch from Steam"
!insertmacro LANG_STRING SETUP_STEAM_DESC "Launches ${GAME_NAME} from Steam in place of your X-COM games."
!insertmacro LANG_STRING SETUP_DESKTOP "Desktop Shortcut"
!insertmacro LANG_STRING SETUP_DESKTOP_DESC "Creates a shortcut in the desktop to play ${GAME_NAME}."

;--------------------------------
;Uninstaller Descriptions

!insertmacro LANG_STRING SETUP_UNDATA "Delete X-COM Data"
!insertmacro LANG_STRING SETUP_UNDATA_DESC "Deletes all ${GAME_NAME} data, including X-COM resources. Recommended for a clean reinstall."
!insertmacro LANG_STRING SETUP_UNUSER "Delete User Data"
!insertmacro LANG_STRING SETUP_UNUSER_DESC "Deletes all ${GAME_NAME} user data, including mods, savegames, screenshots and options. Only use this for a complete wipe."

;--------------------------------
;Shortcuts

!insertmacro LANG_STRING SETUP_SHORTCUT_CHANGELOG "Changelog"
!insertmacro LANG_STRING SETUP_SHORTCUT_README "Readme"
!insertmacro LANG_STRING SETUP_SHORTCUT_USER "User Folder"
!insertmacro LANG_STRING SETUP_SHORTCUT_UNINSTALL "Uninstall"

;--------------------------------
;Warnings

!insertmacro LANG_STRING SETUP_WARNING_UFO "X-COM 1 wasn't found in the folder. ${GAME_NAME} requires a valid installation of X-COM to run. Continue?"
!insertmacro LANG_STRING SETUP_WARNING_TFTD "X-COM 2 wasn't found in the folder. ${GAME_NAME} requires a valid installation of X-COM to run. Continue?"
!insertmacro LANG_STRING SETUP_WARNING_XCU "XcomUtil detected. ${GAME_NAME} is incompatible with XcomUtil and may experience weird behavior. Continue?"

;NSIS OpenXcom Windows Installer Language

!insertmacro LANGFILE_EXT "English" ; Must be the lang name define by NSIS

;--------------------------------
;Pages

${LangFileString} SETUP_XCOM_FOLDER_TITLE "Choose X-COM Location"
${LangFileString} SETUP_XCOM_FOLDER_SUBTITLE "Choose the folders where you have X-COM installed."
${LangFileString} SETUP_XCOM_FOLDER "${GAME_NAME} ${GAME_VERSION} requires a copy of one of the following X-COM games. You can skip this step if you're upgrading an existing installation.$\n$\nSetup will copy the required files from the following folders. To copy from a different folder, click ... and select another folder. Click Next to continue."
${LangFileString} SETUP_UFO_FOLDER "UFO: Enemy Unknown / X-COM: UFO Defense"
${LangFileString} SETUP_TFTD_FOLDER "X-COM: Terror From The Deep"

${LangFileString} SETUP_EXTRA_OPTIONS_TITLE "Choose Options"
${LangFileString} SETUP_EXTRA_OPTIONS_SUBTITLE "Choose which additional options of $(^NameDA) you want to install."
${LangFileString} SETUP_PORTABLE "Portable Installation"
${LangFileString} SETUP_PORTABLE_DESC "${GAME_NAME} will store user data in the installation folder. The folder must be user-writable."

;--------------------------------
;Installer Sections

${LangFileString} SETUP_GAME "Game Files"
${LangFileString} SETUP_GAME_DESC "Files required to run ${GAME_NAME}."
${LangFileString} SETUP_PATCH "Data Patch"
${LangFileString} SETUP_PATCH_DESC "Fixes errors in the original X-COM data. Recommended for first installations. (requires an internet connection)"
${LangFileString} SETUP_DESKTOP "Desktop Shortcut"
${LangFileString} SETUP_DESKTOP_DESC "Creates a shortcut in the desktop to play ${GAME_NAME}."

;--------------------------------
;Uninstaller Descriptions

${LangFileString} SETUP_UNDATA "Delete X-COM Data"
${LangFileString} SETUP_UNDATA_DESC "Deletes all ${GAME_NAME} install data, including X-COM resources. Recommended for a clean reinstall."
${LangFileString} SETUP_UNUSER "Delete User Data"
${LangFileString} SETUP_UNUSER_DESC "Deletes all ${GAME_NAME} user data, including mods, savegames, screenshots and options. Only use this for a complete wipe."

;--------------------------------
;Shortcuts

${LangFileString} SETUP_SHORTCUT_CHANGELOG "Changelog"
${LangFileString} SETUP_SHORTCUT_README "Readme"
${LangFileString} SETUP_SHORTCUT_USER "User Folder"
${LangFileString} SETUP_SHORTCUT_UNINSTALL "Uninstall"

;--------------------------------
;Warnings

${LangFileString} SETUP_WARNING_UFO "X-COM 1 wasn't found in the folder. ${GAME_NAME} requires a valid installation of X-COM to run. Continue?"
${LangFileString} SETUP_WARNING_TFTD "X-COM 2 wasn't found in the folder. ${GAME_NAME} requires a valid installation of X-COM to run. Continue?"
${LangFileString} SETUP_WARNING_XCU "XcomUtil detected. ${GAME_NAME} is incompatible with XcomUtil and may not work correctly. Continue?"
${LangFileString} SETUP_WARNING_PATCH "Failed to install Data Patch. ${GAME_NAME} will still work but with some minor bugs. Continue?"
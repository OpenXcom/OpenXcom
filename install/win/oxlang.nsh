;NSIS OpenXcom Windows Installer Language

;--------------------------------
;General

	LicenseLangString MUILicense ${LANG_ENGLISH} "gpl-en.txt"

;--------------------------------
;Pages
	
	LangString PAGE_UfoFolder ${LANG_ENGLISH} "${GAME_NAME} requires a copy of UFO: Enemy Unknown / X-Com: UFO Defense. You can skip this step if you're upgrading an existing installation.$\n$\nSetup will copy the required files from the following folder. To copy from a different folder, click Browse and select another folder. Click Next to continue."
	LangString PAGE_UfoFolder_TITLE ${LANG_ENGLISH} "Choose UFO Location"
	LangString PAGE_UfoFolder_SUBTITLE ${LANG_ENGLISH} "Choose the folder where you have UFO installed."
	LangString DEST_UfoFolder ${LANG_ENGLISH} "UFO Folder"

;--------------------------------
;Installer Sections	
	
	LangString NAME_SecMain ${LANG_ENGLISH} "Game Files"
	LangString DESC_SecMain ${LANG_ENGLISH} "Files required to run ${GAME_NAME}."
	LangString NAME_SecPatch ${LANG_ENGLISH} "Data Patch"
	LangString DESC_SecPatch ${LANG_ENGLISH} "Fixes errors in the original X-Com data. Recommended for first installations. (requires an internet connection)"
	LangString NAME_SecMusic ${LANG_ENGLISH} "DOS Music"
	LangString DESC_SecMusic ${LANG_ENGLISH} "Adlib/SoundBlaster music recording. Fixes Windows playback issues. (requires an internet connection)"
	LangString NAME_SecDesktop ${LANG_ENGLISH} "Desktop Shortcut"
	LangString DESC_SecDesktop ${LANG_ENGLISH} "Creates a shortcut in the desktop to play ${GAME_NAME}."
	
;--------------------------------
;Uninstaller Descriptions	

	LangString NAME_UnData ${LANG_ENGLISH} "Delete X-Com Data"
	LangString DESC_UnData ${LANG_ENGLISH} "Deletes all OpenXcom data, including mods and X-Com resources. Recommended for a clean reinstall."
	LangString NAME_UnUser ${LANG_ENGLISH} "Delete User Data"
	LangString DESC_UnUser ${LANG_ENGLISH} "Deletes all OpenXcom user data, like savegames, screenshots and options. Recommended for a complete wipe."

;--------------------------------
;Shortcuts	

	LangString LINK_DataFolder ${LANG_ENGLISH} "Data Folder"
	LangString LINK_Readme ${LANG_ENGLISH} "Readme"
	LangString LINK_UserFolder ${LANG_ENGLISH} "User Folder"
	LangString LINK_Uninstall ${LANG_ENGLISH} "Uninstall"
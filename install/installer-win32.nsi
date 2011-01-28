;NSIS OpenXcom Windows Installer

;--------------------------------
;Includes

	!include "MUI2.nsh"
	!include "FileFunc.nsh"

;--------------------------------
;General

	!define GAME_NAME "OpenXcom"
	!define GAME_VERSION "0.2"
	!define GAME_AUTHOR "OpenXcom Developers"

	;Name and file
	Name "${GAME_NAME} ${GAME_VERSION}"
	OutFile "openxcom-v${GAME_VERSION}-win32.exe"

	;Default installation folder
	InstallDir "$PROGRAMFILES\${GAME_NAME}"

	;Get installation folder from registry if available
	InstallDirRegKey HKLM "Software\${GAME_NAME}" ""

	;Request application privileges for Windows Vista
	RequestExecutionLevel admin
	
;--------------------------------
;Variables

  Var StartMenuFolder
  Var UFODIR

;--------------------------------
;Get UFO folder from Steam

Function .onInit
	StrCpy $StartMenuFolder "${GAME_NAME}"
	StrCpy $UFODIR ""
	ReadRegStr $R0 HKLM "Software\Valve\Steam" "InstallPath"
	IfErrors ufo_no
	StrCpy $R0 "$R0\steamapps\common\xcom ufo defense\XCOM"
	${DirState} $R0 $R1
	IntCmp $R1 -1 ufo_no
	StrCpy $UFODIR $R0
	ufo_no:
FunctionEnd

;--------------------------------
;Interface Settings

	!define MUI_ABORTWARNING

;--------------------------------
;Pages
	
	;Language strings
	LangString PAGE_UfoFolder ${LANG_ENGLISH} "${GAME_NAME} requires a copy of UFO: Enemy Unknown / X-Com: UFO Defense. You can skip this step if you're upgrading an existing installation.$\n$\nSetup will copy the required files from the following folder. To copy from a different folder, click Browse and select another folder. Click Next to continue."
	LangString PAGE_UfoFolder_TITLE ${LANG_ENGLISH} "Choose UFO Location"
	LangString PAGE_UfoFolder_SUBTITLE ${LANG_ENGLISH} "Choose the folder where you have UFO installed."
	LangString DEST_UfoFolder ${LANG_ENGLISH} "UFO Folder"
	
	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_LICENSE "gpl.txt"
	!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
	
	; UFO Folder Page Configuration
	!define MUI_PAGE_HEADER_TEXT $(PAGE_UfoFolder_TITLE)
	!define MUI_PAGE_HEADER_SUBTEXT $(PAGE_UfoFolder_SUBTITLE)
	!define MUI_DIRECTORYPAGE_TEXT_TOP $(PAGE_UfoFolder)
	!define MUI_DIRECTORYPAGE_TEXT_DESTINATION $(DEST_UfoFolder)
	!define MUI_DIRECTORYPAGE_VARIABLE $UFODIR
	!define MUI_DIRECTORYPAGE_VERIFYONLEAVE
	!insertmacro MUI_PAGE_DIRECTORY
	
	;Start Menu Folder Page Configuration
	!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM" 
	!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${GAME_NAME}" 
	!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

	!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
	
	!insertmacro MUI_PAGE_INSTFILES
	
	;Finish Page Configuration
	!define MUI_FINISHPAGE_RUN "$INSTDIR\OpenXcom.exe"
	!define MUI_FINISHPAGE_NOREBOOTSUPPORT
	
	!insertmacro MUI_PAGE_FINISH

	!insertmacro MUI_UNPAGE_WELCOME
	!insertmacro MUI_UNPAGE_COMPONENTS
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
	!insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

	!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Game Files" SecMain

	SectionIn RO

	SetOutPath "$INSTDIR"

	File "..\bin\Release\OpenXcom.exe"
	File "..\bin\libogg-0.dll"
	File "..\bin\libvorbis-0.dll"
	File "..\bin\libvorbisfile-3.dll"
	File "..\bin\mikmod.dll"
	File "..\bin\SDL.dll"
	File "..\bin\SDL_gfx.dll"
	File "..\bin\SDL_mixer.dll"
	File "..\bin\smpeg.dll"
	File "..\bin\msvcp100.dll"
	File "..\bin\msvcr100.dll"
	File "..\COPYING"
	File "..\README.txt"
	
	SetOutPath "$INSTDIR\DATA"
	
	File "..\bin\DATA\README.txt"
	
	;Copy UFO files
	${DirState} $UFODIR $R1
	IntCmp $R1 -1 ufo_no
	
	CreateDirectory "$INSTDIR\DATA\GEODATA"
	CopyFiles /SILENT "$UFODIR\GEODATA\*.*" "$INSTDIR\DATA\GEODATA" 361
	CreateDirectory "$INSTDIR\DATA\GEOGRAPH"
	CopyFiles /SILENT "$UFODIR\GEOGRAPH\*.*" "$INSTDIR\DATA\GEOGRAPH" 2770
	CreateDirectory "$INSTDIR\DATA\MAPS"
	CopyFiles /SILENT "$UFODIR\MAPS\*.*" "$INSTDIR\DATA\MAPS" 278
	CreateDirectory "$INSTDIR\DATA\ROUTES"
	CopyFiles /SILENT "$UFODIR\ROUTES\*.*" "$INSTDIR\DATA\ROUTES" 27
	CreateDirectory "$INSTDIR\DATA\SOUND"
	CopyFiles /SILENT "$UFODIR\SOUND\*.*" "$INSTDIR\DATA\SOUND" 2386
	CreateDirectory "$INSTDIR\DATA\TERRAIN"
	CopyFiles /SILENT "$UFODIR\TERRAIN\*.*" "$INSTDIR\DATA\TERRAIN" 620
	CreateDirectory "$INSTDIR\DATA\UFOGRAPH"
	CopyFiles /SILENT "$UFODIR\UFOGRAPH\*.*" "$INSTDIR\DATA\UFOGRAPH" 437
	CreateDirectory "$INSTDIR\DATA\UFOINTRO"
	CopyFiles /SILENT "$UFODIR\UFOINTRO\*.*" "$INSTDIR\DATA\UFOINTRO" 2736
	CreateDirectory "$INSTDIR\DATA\UNITS"
	CopyFiles /SILENT "$UFODIR\UNITS\*.*" "$INSTDIR\DATA\UNITS" 467
	
	ufo_no:
	
	SetOutPath "$INSTDIR\DATA\Language"
	
	File "..\bin\DATA\Language\English.lng"
	File "..\bin\DATA\Language\French.geo"
	File "..\bin\DATA\Language\French.lng"
	File "..\bin\DATA\Language\German.geo"
	File "..\bin\DATA\Language\German.lng"
	File "..\bin\DATA\Language\Italian.geo"
	File "..\bin\DATA\Language\Italian.lng"
	File "..\bin\DATA\Language\Spanish.geo"
	File "..\bin\DATA\Language\Spanish.lng"
	
	SetOutPath "$INSTDIR\USER"
	
	File "..\bin\USER\README.txt"

	;Store installation folder
	WriteRegStr HKLM "Software\${GAME_NAME}" "" $INSTDIR
	
	;Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "DisplayName" "${GAME_NAME} ${GAME_VERSION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "DisplayVersion" "${GAME_VERSION}.0.0"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "InstallLocation" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "Publisher" "${GAME_AUTHOR}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "UninstallString" '"$INSTDIR\Uninstall.exe"'
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "URLInfoAbout" "http://openxcom.ninex.info"
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "NoRepair" 1

	;Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	
	;Create shortcuts
	SetOutPath "$INSTDIR"
	
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
		CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\OpenXcom.lnk" "$INSTDIR\OpenXcom.exe"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Readme.lnk" "$INSTDIR\README.TXT"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
	!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section /o "Desktop Shortcut" SecDesktop

	SetOutPath "$INSTDIR"
	
	CreateShortCut "$DESKTOP\OpenXcom.lnk" "$INSTDIR\OpenXcom.exe"

SectionEnd

;--------------------------------
;Descriptions

	;Language strings
	LangString DESC_SecMain ${LANG_ENGLISH} "Files required to run ${GAME_NAME}."
	LangString DESC_SecDesktop ${LANG_ENGLISH} "Creates a shortcut in the desktop to play ${GAME_NAME}."

	;Assign language strings to sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
		!insertmacro MUI_DESCRIPTION_TEXT ${SecMain} $(DESC_SecMain)
		!insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} $(DESC_SecDesktop)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Sections

Section /o "un.Delete User Files" UnUser
	Delete "$INSTDIR\USER\*.*"
SectionEnd

Section "un.Delete X-Com Data" UnData
	RMDir /r "$INSTDIR\DATA"
SectionEnd

Section "-un.Main"
	
	SetOutPath "$TEMP"
	
	Delete "$INSTDIR\OpenXcom.exe"
	Delete "$INSTDIR\libogg-0.dll"
	Delete "$INSTDIR\libvorbis-0.dll"
	Delete "$INSTDIR\libvorbisfile-3.dll"
	Delete "$INSTDIR\mikmod.dll"
	Delete "$INSTDIR\SDL.dll"
	Delete "$INSTDIR\SDL_gfx.dll"
	Delete "$INSTDIR\SDL_mixer.dll"
	Delete "$INSTDIR\smpeg.dll"
	Delete "$INSTDIR\msvcp100.dll"
	Delete "$INSTDIR\msvcr100.dll"
	Delete "$INSTDIR\COPYING"
	Delete "$INSTDIR\README.txt"
	
	Delete "$INSTDIR\DATA\README.txt"
	Delete "$INSTDIR\DATA\Language\English.lng"
	Delete "$INSTDIR\DATA\Language\French.geo"
	Delete "$INSTDIR\DATA\Language\French.lng"
	Delete "$INSTDIR\DATA\Language\German.geo"
	Delete "$INSTDIR\DATA\Language\German.lng"
	Delete "$INSTDIR\DATA\Language\Italian.geo"
	Delete "$INSTDIR\DATA\Language\Italian.lng"
	Delete "$INSTDIR\DATA\Language\Spanish.geo"
	Delete "$INSTDIR\DATA\Language\Spanish.lng"
	RMDir "$INSTDIR\DATA\Language"
	RMDir "$INSTDIR\DATA"
	
	Delete "$INSTDIR\USER\README.txt"
	RMDir "$INSTDIR\USER"

	Delete "$INSTDIR\Uninstall.exe"
	RMDir "$INSTDIR"
	
	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
	Delete "$SMPROGRAMS\$StartMenuFolder\OpenXcom.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\Readme.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
	RMDir "$SMPROGRAMS\$StartMenuFolder"
	
	Delete "$DESKTOP\OpenXcom.lnk"
	
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}"
	DeleteRegKey /ifempty HKLM "Software\${GAME_NAME}"

SectionEnd

;--------------------------------
;Uninstaller Descriptions

	;Language strings
	LangString DESC_UnUser ${LANG_ENGLISH} "Deletes all savegames, screenshots, options, etc."
	LangString DESC_UnData ${LANG_ENGLISH} "Deletes the copied X-Com resources."

	;Assign language strings to sections
	!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
		!insertmacro MUI_DESCRIPTION_TEXT ${UnUser} $(DESC_UnUser)
		!insertmacro MUI_DESCRIPTION_TEXT ${UnData} $(DESC_UnData)
	!insertmacro MUI_UNFUNCTION_DESCRIPTION_END

;--------------------------------
;Version Information

	VIProductVersion "${GAME_VERSION}.0.0"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${GAME_NAME} Installer"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${GAME_VERSION}.0.0"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "${GAME_AUTHOR}"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright 2010 ${GAME_AUTHOR}"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${GAME_NAME} Installer"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${GAME_VERSION}.0.0"

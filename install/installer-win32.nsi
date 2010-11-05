;NSIS OpenXcom Windows Installer

;--------------------------------
;Includes

	!include "MUI2.nsh"
	!include "FileFunc.nsh"

;--------------------------------
;General

	!define GAME_NAME "OpenXcom"
	!define GAME_VERSION "0.2"

	;Name and file
	Name "${GAME_NAME} ${GAME_VERSION}"
	OutFile "openxcom-v${GAME_VERSION}-win32.exe"

	;Default installation folder
	InstallDir "$PROGRAMFILES\${GAME_NAME}"

	;Get installation folder from registry if available
	InstallDirRegKey HKCU "Software\${GAME_NAME}" ""

	;Request application privileges for Windows Vista
	RequestExecutionLevel user
	
;--------------------------------
;Variables

  Var StartMenuFolder
  Var UFODIR

;--------------------------------
;Get UFO folder from Steam

; Push $filenamestring (e.g. 'c:\this\and\that\filename.htm')
; Push "\"
; Call StrSlash
; Pop $R0
; ;Now $R0 contains 'c:/this/and/that/filename.htm'
Function StrSlash
	Exch $R3 ; $R3 = needle ("\" or "/")
	Exch
	Exch $R1 ; $R1 = String to replacement in (haystack)
	Push $R2 ; Replaced haystack
	Push $R4 ; $R4 = not $R3 ("/" or "\")
	Push $R6
	Push $R7 ; Scratch reg
	StrCpy $R2 ""
	StrLen $R6 $R1
	StrCpy $R4 "\"
	StrCmp $R3 "/" loop
	StrCpy $R4 "/"  
loop:
	StrCpy $R7 $R1 1
	StrCpy $R1 $R1 $R6 1
	StrCmp $R7 $R3 found
	StrCpy $R2 "$R2$R7"
	StrCmp $R1 "" done loop
found:
	StrCpy $R2 "$R2$R4"
	StrCmp $R1 "" done loop
done:
	StrCpy $R3 $R2
	Pop $R7
	Pop $R6
	Pop $R4
	Pop $R2
	Pop $R1
	Exch $R3
FunctionEnd

Function .onInit
	StrCpy $StartMenuFolder "${GAME_NAME}"
	StrCpy $UFODIR ""
	ReadRegStr $R0 HKCU "Software\Valve\Steam" "SteamPath"
	IfErrors ufo_no
	Push $R0
	Push "/"
	Call StrSlash
	Pop $R0
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
	LangString PAGE_UfoFolder ${LANG_ENGLISH} "${GAME_NAME} requires a copy of UFO: Enemy Unknown / X-Com: UFO Defense. Setup will copy the required files from the following folder. To copy from a different folder, click Browse and select another folder. Click Next to continue."
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
	!insertmacro MUI_PAGE_DIRECTORY
	
	;Start Menu Folder Page Configuration
	!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
	!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${GAME_NAME}" 
	!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

	!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
	
	!insertmacro MUI_PAGE_INSTFILES
	
	;Finish Page Configuration
	!define MUI_FINISHPAGE_RUN "$INSTDIR\OpenXcom.exe"
	!define MUI_FINISHPAGE_NOREBOOTSUPPORT
	
	!insertmacro MUI_PAGE_FINISH

	!insertmacro MUI_UNPAGE_WELCOME
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

	;Store installation folder
	WriteRegStr HKCU "Software\${GAME_NAME}" "" $INSTDIR
	
	;Write the uninstall keys for Windows
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "DisplayName" "${GAME_NAME}"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "DisplayVersion" "${GAME_VERSION}.0.0"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "InstallLocation" "$INSTDIR"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "UninstallString" '"$INSTDIR\Uninstall.exe"'
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "URLInfoAbout" "http://openxcom.ninex.info"
	WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "NoModify" 1
	WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "NoRepair" 1

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
;Uninstaller Section

Section "Uninstall"

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

	Delete "$INSTDIR\Uninstall.exe"
	
	RMDir /r "$INSTDIR\DATA"
	RMDir "$INSTDIR"
	
	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
	Delete "$SMPROGRAMS\$StartMenuFolder\OpenXcom.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\Readme.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
	RMDir "$SMPROGRAMS\$StartMenuFolder"
	
	Delete "$DESKTOP\OpenXcom.lnk"
	
	DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}"
	DeleteRegKey /ifempty HKCU "Software\${GAME_NAME}"

SectionEnd

;--------------------------------
;Version Information

	VIProductVersion "${GAME_VERSION}.0.0"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${GAME_NAME} Installer"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${GAME_VERSION}.0.0"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "OpenXcom Developers"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright 2010 OpenXcom Developers"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${GAME_NAME} Installer"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${GAME_VERSION}.0.0"

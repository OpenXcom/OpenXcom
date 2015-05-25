;NSIS OpenXcom Windows Installer

;--------------------------------
;Includes

	!include "MUI2.nsh"
	!include "x64.nsh"
	!include "Sections.nsh"

;--------------------------------
;Defines

	!define GAME_NAME "OpenXcom"
	!define GAME_VERSION "1.0"
	!define GAME_AUTHOR "OpenXcom Developers"	
	
;--------------------------------
;General

	;Name and file
	Name "${GAME_NAME} ${GAME_VERSION}"
	OutFile "openxcom-v${GAME_VERSION}-win.exe"

	;Default installation folder
	InstallDir "$PROGRAMFILES\${GAME_NAME}"

	;Get installation folder from registry if available
	InstallDirRegKey HKLM "Software\${GAME_NAME}" ""

	;Request application privileges for Windows Vista
	RequestExecutionLevel admin
	
;--------------------------------
;Variables

	Var StartMenuFolder
	Var STEAMDIR
	Var UFODIR

;--------------------------------
;Interface Settings

	!define MUI_HEADERIMAGE
	!define MUI_HEADERIMAGE_BITMAP logo.bmp
	!define MUI_WELCOMEFINISHPAGE_BITMAP side.bmp

;--------------------------------
;Language Selection Dialog Settings

	;Remember the installer language
	!define MUI_LANGDLL_REGISTRY_ROOT "HKLM" 
	!define MUI_LANGDLL_REGISTRY_KEY "Software\${GAME_NAME}" 
	!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
;Pages
	
	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_COMPONENTS
	!define MUI_PAGE_CUSTOMFUNCTION_PRE PreDirectory
	!insertmacro MUI_PAGE_DIRECTORY
	
	; UFO Folder Page Configuration
	!define MUI_PAGE_HEADER_TEXT $(PAGE_UfoFolder_TITLE)
	!define MUI_PAGE_HEADER_SUBTEXT $(PAGE_UfoFolder_SUBTITLE)
	!define MUI_DIRECTORYPAGE_TEXT_TOP $(PAGE_UfoFolder)
	!define MUI_DIRECTORYPAGE_TEXT_DESTINATION $(DEST_UfoFolder)
	!define MUI_DIRECTORYPAGE_VARIABLE $UFODIR
	!define MUI_DIRECTORYPAGE_VERIFYONLEAVE
	!define MUI_PAGE_CUSTOMFUNCTION_PRE PreUFO
	!define MUI_PAGE_CUSTOMFUNCTION_LEAVE ValidateUFO
	
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

	!insertmacro MUI_UNPAGE_COMPONENTS
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

	!insertmacro MUI_LANGUAGE "English" ;first language is the default language
	!insertmacro MUI_LANGUAGE "Czech"
	!insertmacro MUI_LANGUAGE "French"
	!insertmacro MUI_LANGUAGE "Finnish"
	!insertmacro MUI_LANGUAGE "German"
	!insertmacro MUI_LANGUAGE "Hungarian"
	!insertmacro MUI_LANGUAGE "Italian"
	!insertmacro MUI_LANGUAGE "Portuguese"
	!insertmacro MUI_LANGUAGE "PortugueseBR"
	!insertmacro MUI_LANGUAGE "Polish"
	!insertmacro MUI_LANGUAGE "Romanian"
	!insertmacro MUI_LANGUAGE "Russian"
	!insertmacro MUI_LANGUAGE "Slovak"
	!insertmacro MUI_LANGUAGE "Spanish"
	!insertmacro MUI_LANGUAGE "SpanishInternational"
	!insertmacro MUI_LANGUAGE "Turkish"
	!insertmacro MUI_LANGUAGE "Ukrainian"

	!include "installerlang.nsh" ; Language strings

;--------------------------------
;Reserve Files
  
	;If you are using solid compression, files that are required before
	;the actual installation should be stored first in the data block,
	;because this will make your installer start faster.

	!insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
;Installer Sections

Section "$(NAME_SecMain)" SecMain

	SectionIn RO

	SetOutPath "$INSTDIR"
	
	IfFileExists "$INSTDIR\dosbox.exe" 0 no_steam
	CreateDirectory "$INSTDIR\bak"
	Rename "$INSTDIR\dosbox.exe"  "$INSTDIR\bak\dosbox.exe"
	Rename "$INSTDIR\dosbox.conf" "$INSTDIR\bak\dosbox.conf"
	Rename "$INSTDIR\readme.txt"  "$INSTDIR\bak\readme.txt"
	Rename "$INSTDIR\SDL.dll"     "$INSTDIR\bak\SDL.dll"
	Rename "$INSTDIR\SDL_net.dll" "$INSTDIR\bak\SDL_net.dll"
	
	no_steam:

${If} ${RunningX64}
	File "..\..\bin\x64\Release\OpenXcom.exe"
	File "..\..\bin\x64\*.dll"
${Else}
	File "..\..\bin\Win32\Release\OpenXcom.exe"
	File "..\..\bin\Win32\*.dll"
${EndIf}
	File "..\..\LICENSE.txt"
	File "..\..\CHANGELOG.txt"
	File "..\..\README.md"
	
	;Copy UFO files
	IfFileExists "$UFODIR\*.*" 0 ufo_no
	
	CreateDirectory "$INSTDIR\UFO\GEODATA"
	CopyFiles /SILENT "$UFODIR\GEODATA\*.*" "$INSTDIR\UFO\GEODATA" 361
	CreateDirectory "$INSTDIR\UFO\GEOGRAPH"
	CopyFiles /SILENT "$UFODIR\GEOGRAPH\*.*" "$INSTDIR\UFO\GEOGRAPH" 2770
	CreateDirectory "$INSTDIR\UFO\MAPS"
	CopyFiles /SILENT "$UFODIR\MAPS\*.*" "$INSTDIR\UFO\MAPS" 278
	CreateDirectory "$INSTDIR\UFO\ROUTES"
	CopyFiles /SILENT "$UFODIR\ROUTES\*.*" "$INSTDIR\UFO\ROUTES" 27
	CreateDirectory "$INSTDIR\UFO\SOUND"
	CopyFiles /SILENT "$UFODIR\SOUND\*.*" "$INSTDIR\UFO\SOUND" 2386
	CreateDirectory "$INSTDIR\UFO\TERRAIN"
	CopyFiles /SILENT "$UFODIR\TERRAIN\*.*" "$INSTDIR\UFO\TERRAIN" 620
	CreateDirectory "$INSTDIR\UFO\UFOGRAPH"
	CopyFiles /SILENT "$UFODIR\UFOGRAPH\*.*" "$INSTDIR\UFO\UFOGRAPH" 437
	CreateDirectory "$INSTDIR\UFO\UFOINTRO"
	CopyFiles /SILENT "$UFODIR\UFOINTRO\*.*" "$INSTDIR\UFO\UFOINTRO" 2736
	CreateDirectory "$INSTDIR\UFO\UNITS"
	CopyFiles /SILENT "$UFODIR\UNITS\*.*" "$INSTDIR\UFO\UNITS" 467
	
	ufo_no:
	
	SetOutPath "$INSTDIR"
	
	File "..\..\bin\TFTD\README.txt"
	File "..\..\bin\UFO\README.txt"
	File /r "..\..\bin\common"
	File /r "..\..\bin\standard"

	;Store installation folder
	WriteRegStr HKLM "Software\${GAME_NAME}" "" $INSTDIR
	
	;Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "DisplayName" "${GAME_NAME} ${GAME_VERSION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "DisplayIcon" '"$INSTDIR\OpenXcom.exe",0'
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "DisplayVersion" "${GAME_VERSION}.0.0"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "InstallLocation" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "Publisher" "${GAME_AUTHOR}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "UninstallString" '"$INSTDIR\Uninstall.exe"'
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "URLInfoAbout" "http://openxcom.org"
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "NoRepair" 1

	;Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	
	;Create shortcuts
	SetOutPath "$INSTDIR"
	
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
		CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(LINK_DataFolder).lnk" "$INSTDIR"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\${GAME_NAME}.lnk" "$INSTDIR\OpenXcom.exe"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(LINK_Readme).lnk" "$INSTDIR\README.TXT"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(LINK_Uninstall).lnk" "$INSTDIR\Uninstall.exe"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(LINK_UserFolder).lnk" "$DOCUMENTS\OpenXcom"
  
	!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section "$(NAME_SecPatch)" SecPatch
	
	;(uses NSISdl.dll)
	NSISdl::download "http://openxcom.org/download/extras/universal-patch.zip" "$TEMP\universal-patch.zip"
	Pop $0
	StrCmp $0 success success1
		SetDetailsView show
		DetailPrint "download failed: $0"
		Abort
	success1:

	;(uses nsisunz.dll)
	nsisunz::UnzipToLog "$TEMP\universal-patch.zip" "$INSTDIR\UFO"
	Pop $0
	StrCmp $0 "success" success2
		SetDetailsView show
		DetailPrint "unzipping failed: $0"
		Abort
	success2:

	Delete "$TEMP\universal-patch.zip"

SectionEnd

Section /o "$(NAME_SecPortable)" SecPortable

	CreateDirectory "$INSTDIR\user"
	
SectionEnd

Section /o "$(NAME_SecSteam)" SecSteam

	SectionIn RO
	
	Rename "$INSTDIR\OpenXcom.exe" "$INSTDIR\dosbox.exe"
	
SectionEnd

Section /o "$(NAME_SecDesktop)" SecDesktop

	SetOutPath "$INSTDIR"
	
	CreateShortCut "$DESKTOP\${GAME_NAME}.lnk" "$INSTDIR\OpenXcom.exe"

SectionEnd

;--------------------------------
;Descriptions

	;Assign language strings to sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
		!insertmacro MUI_DESCRIPTION_TEXT ${SecMain} $(DESC_SecMain)
		!insertmacro MUI_DESCRIPTION_TEXT ${SecPatch} $(DESC_SecPatch)
		!insertmacro MUI_DESCRIPTION_TEXT ${SecPortable} $(DESC_SecPortable)
		!insertmacro MUI_DESCRIPTION_TEXT ${SecSteam} $(DESC_SecSteam)
		!insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} $(DESC_SecDesktop)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Installer functions

Function .onInit
${If} ${RunningX64}
	StrCpy $INSTDIR "$PROGRAMFILES64\${GAME_NAME}"
${Else}
	StrCpy $INSTDIR "$PROGRAMFILES32\${GAME_NAME}"
${EndIf}
	StrCpy $StartMenuFolder "${GAME_NAME}"
	
	; Get UFO folder from Steam
	StrCpy $STEAMDIR ""
	StrCpy $UFODIR ""
	ReadRegStr $R1 HKLM "Software\Valve\Steam" "InstallPath"
	IfErrors ufo_no
	StrCpy $R0 "$R1\steamapps\common\xcom ufo defense\XCOM"
	IfFileExists "$R0\*.*" ufo_yes ufo_no
	ufo_yes:
	StrCpy $STEAMDIR "$R1\steamapps\common\xcom ufo defense"
	StrCpy $UFODIR $R0
	SectionSetFlags ${SecSteam} 0
	ufo_no:
	
	!insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

;--------------------------------
;Skip pages for Steam installation

Function PreDirectory
	${If} ${SectionIsSelected} ${SecSteam}
		StrCpy $INSTDIR $STEAMDIR
		Abort
	${EndIf}
FunctionEnd

Function PreUFO
	${If} ${SectionIsSelected} ${SecSteam}
		Abort
	${EndIf}
FunctionEnd

;--------------------------------
;Validate UFO folder

Function ValidateUFO
	StrCmp $UFODIR "" validate_yes
	IfFileExists "$UFODIR\GEODATA\*.*" 0 confirm_ufo
	IfFileExists "$UFODIR\GEOGRAPH\*.*" 0 confirm_ufo
	IfFileExists "$UFODIR\MAPS\*.*" 0 confirm_ufo
	IfFileExists "$UFODIR\ROUTES\*.*" 0 confirm_ufo
	IfFileExists "$UFODIR\SOUND\*.*" 0 confirm_ufo
	IfFileExists "$UFODIR\TERRAIN\*.*" 0 confirm_ufo
	IfFileExists "$UFODIR\UFOGRAPH\*.*" 0 confirm_ufo
	IfFileExists "$UFODIR\UFOINTRO\*.*" 0 confirm_ufo
	IfFileExists "$UFODIR\UNITS\*.*" 0 confirm_ufo
	IfFileExists "$UFODIR\XcuSetup.bat" confirm_xcu
	Goto validate_yes
	confirm_ufo:
	MessageBox MB_ICONEXCLAMATION|MB_YESNO $(WARN_UFOMissing) /SD IDYES IDYES validate_yes IDNO validate_no
	confirm_xcu:
	MessageBox MB_ICONEXCLAMATION|MB_YESNO $(WARN_XCUDetected) /SD IDYES IDYES validate_yes IDNO validate_no
	validate_no:
	Abort	
	validate_yes:
FunctionEnd

;--------------------------------
;Uninstaller Functions

Function un.onInit

	!insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd

;--------------------------------
;Uninstaller Sections

Section /o "un.$(NAME_UnData)" UnData
	RMDir /r "$INSTDIR\TFTD"
	RMDir /r "$INSTDIR\UFO"
SectionEnd

Section /o "un.$(NAME_UnUser)" UnUser
	RMDir /r "$INSTDIR\user"
	RMDir /r "$DOCUMENTS\OpenXcom"
SectionEnd

Section "-un.Main"
	
	SetOutPath "$TEMP"
	
	Delete "$INSTDIR\OpenXcom.exe"
	Delete "$INSTDIR\*.dll"
	Delete "$INSTDIR\LICENSE.txt"
	Delete "$INSTDIR\README.md"
	Delete "$INSTDIR\CHANGELOG.txt"
	
	RMDir /r "$INSTDIR\common"
	RMDir /r "$INSTDIR\standard"

	Delete "$INSTDIR\Uninstall.exe"
	RMDir "$INSTDIR"	
	
	IfFileExists "$INSTDIR\bak\*.*" 0 no_backup
	Delete "$INSTDIR\dosbox.exe"
	CopyFiles "$INSTDIR\bak\*.*" "$INSTDIR"
	RMDir "$INSTDIR\bak"
	
	no_backup:
	
	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
	Delete "$SMPROGRAMS\$StartMenuFolder\*.*"
	RMDir "$SMPROGRAMS\$StartMenuFolder"
	
	Delete "$DESKTOP\${GAME_NAME}.lnk"
	
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}"
	DeleteRegKey /ifempty HKLM "Software\${GAME_NAME}"

SectionEnd

;--------------------------------
;Uninstaller Descriptions

	;Assign language strings to sections
	!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
		!insertmacro MUI_DESCRIPTION_TEXT ${UnData} $(DESC_UnData)
		!insertmacro MUI_DESCRIPTION_TEXT ${UnUser} $(DESC_UnUser)
	!insertmacro MUI_UNFUNCTION_DESCRIPTION_END

;--------------------------------
;Version Information

	VIProductVersion "${GAME_VERSION}.0.0"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${GAME_NAME} Installer"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${GAME_VERSION}.0.0"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "${GAME_AUTHOR}"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright 2010-2014 ${GAME_AUTHOR}"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${GAME_NAME} Installer"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${GAME_VERSION}.0.0"

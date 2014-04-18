;NSIS OpenXcom Windows Installer

;--------------------------------
;Defines

	!define GAME_NAME "OpenXcom"
	!define GAME_VERSION "0.9"
	!define GAME_AUTHOR "OpenXcom Developers"

;--------------------------------
;Includes

	!include "MUI2.nsh"
	!include "ZipDLL.nsh"
	!include "FileFunc.nsh"
	!include "x64.nsh"
	!include "oxlang.nsh" ; Language strings

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
  Var UFODIR

;--------------------------------
;Get UFO folder from Steam

Function .onInit
${If} ${RunningX64}
	StrCpy $INSTDIR "$PROGRAMFILES64\${GAME_NAME}"
${Else}
	StrCpy $INSTDIR "$PROGRAMFILES32\${GAME_NAME}"
${EndIf}
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

	!define MUI_HEADERIMAGE
	!define MUI_HEADERIMAGE_BITMAP logo.bmp
	!define MUI_HEADERIMAGE_UNBITMAP logo.bmp
	!define MUI_WELCOMEFINISHPAGE_BITMAP side.bmp
	!define MUI_UNWELCOMEFINISHPAGE_BITMAP side.bmp
	!define MUI_ABORTWARNING

;--------------------------------
;Pages
	
	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_LICENSE "$(MUILicense)"
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

Section "$(NAME_SecMain)" SecMain

	SectionIn RO

	SetOutPath "$INSTDIR"

${If} ${RunningX64}
	File "..\..\bin\x64\Release\OpenXcom.exe"
	File "..\..\bin\x64\*.dll"
${Else}
	File "..\..\bin\Win32\Release\OpenXcom.exe"
	File "..\..\bin\Win32\*.dll"
${EndIf}
	File "..\..\COPYING"
	File "..\..\CHANGELOG.txt"
	File "..\..\README.txt"
	
	SetOutPath "$INSTDIR\data"
	
	File "..\..\bin\data\README.txt"
	
	;Copy UFO files
	${DirState} $UFODIR $R1
	IntCmp $R1 -1 ufo_no
	
	CreateDirectory "$INSTDIR\data\GEODATA"
	CopyFiles /SILENT "$UFODIR\GEODATA\*.*" "$INSTDIR\data\GEODATA" 361
	CreateDirectory "$INSTDIR\data\GEOGRAPH"
	CopyFiles /SILENT "$UFODIR\GEOGRAPH\*.*" "$INSTDIR\data\GEOGRAPH" 2770
	CreateDirectory "$INSTDIR\data\MAPS"
	CopyFiles /SILENT "$UFODIR\MAPS\*.*" "$INSTDIR\data\MAPS" 278
	CreateDirectory "$INSTDIR\data\ROUTES"
	CopyFiles /SILENT "$UFODIR\ROUTES\*.*" "$INSTDIR\data\ROUTES" 27
	CreateDirectory "$INSTDIR\data\SOUND"
	CopyFiles /SILENT "$UFODIR\SOUND\*.*" "$INSTDIR\data\SOUND" 2386
	CreateDirectory "$INSTDIR\data\TERRAIN"
	CopyFiles /SILENT "$UFODIR\TERRAIN\*.*" "$INSTDIR\data\TERRAIN" 620
	CreateDirectory "$INSTDIR\data\UFOGRAPH"
	CopyFiles /SILENT "$UFODIR\UFOGRAPH\*.*" "$INSTDIR\data\UFOGRAPH" 437
	CreateDirectory "$INSTDIR\data\UFOINTRO"
	CopyFiles /SILENT "$UFODIR\UFOINTRO\*.*" "$INSTDIR\data\UFOINTRO" 2736
	CreateDirectory "$INSTDIR\data\UNITS"
	CopyFiles /SILENT "$UFODIR\UNITS\*.*" "$INSTDIR\data\UNITS" 467
	
	ufo_no:
	
	SetOutPath "$INSTDIR\data\Language"
	
	File "..\..\bin\data\Language\*.*"
	
	SetOutPath "$INSTDIR\data\Ruleset"
	
	File "..\..\bin\data\Ruleset\Xcom1Ruleset.rul"
	File "..\..\bin\data\Ruleset\XcomUtil_*.rul"
	File "..\..\bin\data\Ruleset\UFOextender_*.rul"
	
	SetOutPath "$INSTDIR\data\Resources\BulletSprites"
	
	File "..\..\bin\data\Resources\BulletSprites\*.*"
	
	SetOutPath "$INSTDIR\data\Resources\Pathfinding"
	
	File "..\..\bin\data\Resources\Pathfinding\*.*"
	
	SetOutPath "$INSTDIR\data\Resources\UI"
	
	File "..\..\bin\data\Resources\UI\*.*"
	
	SetOutPath "$INSTDIR\data\Resources\Weapons"
	
	File "..\..\bin\data\Resources\Weapons\*.*"
	
	SetOutPath "$INSTDIR\data\SoldierName"
	
	File "..\..\bin\data\SoldierName\*.*"
	
	SetOutPath "$INSTDIR\data\Shaders"
	
	File "..\..\bin\data\Shaders\*.*"

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
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(LINK_DataFolder).lnk" "$INSTDIR\data"
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

	;(uses ZipDLL.dll)
	!insertmacro ZIPDLL_EXTRACT "$TEMP\universal-patch.zip" "$INSTDIR\data" "<ALL>"
	Pop $0
	StrCmp $0 success success2
		SetDetailsView show
		DetailPrint "unzipping failed: $0"
		Abort
	success2:

	Delete "$TEMP\universal-patch.zip"

SectionEnd

Section "$(NAME_SecMusic)" SecMusic
	
	AddSize 31112
	
	;(uses NSISdl.dll)
	NSISdl::download "http://openxcom.org/download/extras/original-music-ogg-128.zip" "$TEMP\original-music-ogg-128.zip"
	Pop $0
	StrCmp $0 success success1
		SetDetailsView show
		DetailPrint "download failed: $0"
		Abort
	success1:

	;(uses ZipDLL.dll)
	!insertmacro ZIPDLL_EXTRACT "$TEMP\original-music-ogg-128.zip" "$INSTDIR\data\sound" "<ALL>"
	Pop $0
	StrCmp $0 success success2
		SetDetailsView show
		DetailPrint "unzipping failed: $0"
		Abort
	success2:

	Delete "$TEMP\original-music-ogg-128.zip"

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
		!insertmacro MUI_DESCRIPTION_TEXT ${SecMusic} $(DESC_SecMusic)
		!insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} $(DESC_SecDesktop)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Sections

Section /o "un.$(NAME_UnData)" UnData
	RMDir /r "$INSTDIR\data"
SectionEnd

Section /o "un.$(NAME_UnUser)" UnUser
	RMDir /r "$DOCUMENTS\OpenXcom"
SectionEnd

Section "-un.Main"
	
	SetOutPath "$TEMP"
	
	Delete "$INSTDIR\OpenXcom.exe"
	Delete "$INSTDIR\*.dll"
	Delete "$INSTDIR\COPYING"
	Delete "$INSTDIR\README.txt"
	Delete "$INSTDIR\CHANGELOG.txt"
	
	Delete "$INSTDIR\data\README.txt"
	Delete "$INSTDIR\data\Language\*.*"
	RMDir "$INSTDIR\data\Language"
	Delete "..\..\bin\data\Ruleset\Xcom1Ruleset.rul"
	Delete "..\..\bin\data\Ruleset\XcomUtil_*.rul"
	Delete "..\..\bin\data\Ruleset\UFOextender_*.rul"
	RMDir "$INSTDIR\Ruleset"
	Delete "..\..\bin\data\Resources\BulletSprites\*.*"
	RMDir "$INSTDIR\data\Resources\BulletSprites"
	Delete "..\..\bin\data\Resources\Pathfinding\*.*"
	RMDir "$INSTDIR\data\Resources\Pathfinding"
	Delete "..\..\bin\data\Resources\UI\*.*"
	RMDir "$INSTDIR\data\Resources\UI"
	Delete "..\..\bin\data\Resources\Weapons\*.*"
	RMDir "$INSTDIR\data\Resources\Weapons"
	Delete "$INSTDIR\data\SoldierName\*.*"
	RMDir "$INSTDIR\data\SoldierName"
	Delete "$INSTDIR\data\Shaders\*.*"
	RMDir "$INSTDIR\data\Shaders"
	RMDir "$INSTDIR\data"

	Delete "$INSTDIR\Uninstall.exe"
	RMDir "$INSTDIR"
	
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

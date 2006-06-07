
;------------------------------------------
; nsis-script for generating setup.exe
;------------------------------------------

!include "MUI.nsh"
!include "Sections.nsh"

Name "${NAME}"
OutFile "${DIST}"
BrandingText " "

InstallDir "$PROGRAMFILES\${NAME}"
InstallDirRegKey HKLM "Software\${NAME}" "Install_Dir"


!define MUI_ICON ${NSISDIR}\Contrib\Graphics\Icons\classic-install.ico
!define MUI_UNICON ${NSISDIR}\Contrib\Graphics\Icons\classic-uninstall.ico

!define MUI_ABORTWARNING

;--------------------------------

!insertmacro MUI_PAGE_LICENSE "COPYING"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------

!insertmacro MUI_LANGUAGE "English"

;--------------------------------

Section "!${NAME}"

  SectionIn RO
  
  SetOutPath $INSTDIR

  ;SetOverwrite on  
  File ${BIN}
  File README.TXT
  File /r winlibs/lib/*dll
  File /r /x .svn /x .svg img
  File /r /x .svn wav
  File /r /x .svn mp3
  
  WriteRegStr HKLM "SOFTWARE\${NAME}" "Install_Dir" "$INSTDIR"
  
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "DisplayName" "Game"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
  ;
  ; Create menu shortcut
  ;

  CreateDirectory "$SMPROGRAMS\${NAME}"
  CreateShortCut "$SMPROGRAMS\${NAME}\${NAME}.lnk" "$INSTDIR\${BIN}"
  CreateShortCut "$SMPROGRAMS\${NAME}\README.TXT.lnk" "$INSTDIR\README.TXT"
  CreateShortCut "$SMPROGRAMS\${NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"

  ;
  ; Create desktop shortcut
  ;

  CreateShortCut "$DESKTOP\${NAME}.lnk" "$INSTDIR\${BIN}" "" "$INSTDIR\${BIN}" 0
  
SectionEnd


;--------------------------------


Section "Uninstall"
  
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
  DeleteRegKey HKLM "SOFTWARE\${NAME}"

  Delete $INSTDIR\${BIN}
  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\*.dll
  Delete $INSTDIR\stdout.txt
  Delete $INSTDIR\stderr.txt
  Delete $INSTDIR\README.TXT
  Rmdir /r $INSTDIR\img
  Rmdir /r $INSTDIR\wav

  Delete "$SMPROGRAMS\${NAME}\*.*"
  Delete "$DESKTOP\${NAME}.lnk"

  RMDir "$SMPROGRAMS\${NAME}"
  RMDir "$INSTDIR"

SectionEnd

;--------------------------------

;This file is encoded with ANSI
;This file is for english.

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_ENGLISH}

LangString RestartExplorer ${CURLANG} "To install new dll, the installer will kill explorer. Check a operation that is copying or deleting then press OK."
LangString UninstallQuestion ${CURLANG} "Uninstall $(^Name)?"
LangString UninstallCompleted ${CURLANG} "$(^Name) is uninstalled."
LangString TEXT_REINSTALL ${CURLANG} "Installed ZViewer is found. It will be installed on same folder. If you don't want to install this folder, please uninstall ZViewer before installation."

!verbose 4

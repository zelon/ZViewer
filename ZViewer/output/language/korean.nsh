;This file is encoded with ANSI
;This file is for korean.

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_KOREAN}

LangString RestartExplorer ${CURLANG} "dll 설치를 위해 모든 탐색기를 종료합니다. 파일 작업 중이라면 작업이 취소되므로, 작업이 끝난 후 확인을 누르세요."
LangString UninstallQuestion ${CURLANG} "$(^Name) (을)를 제거하시겠습니까?"
LangString UninstallCompleted ${CURLANG} "$(^Name) (이)가 제거되었습니다."
LangString TEXT_REINSTALL ${CURLANG} "이미 설치된 ZViewer 를 찾았습니다. 같은 폴더에 설치합니다. 다른 폴더에 설치하려면 ZViewer 를 설치 제거 후 다시 설치해주세요."

!verbose 4

#include "stdafx.h"

#include "../commonSrc/LogManager.h"
#include "../commonSrc/MessageManager.h"
#include "../commonSrc/minidump/MiniDumper.h"
#include "../commonSrc/ZOption.h"

#include "src/Cache/CacheController.h"
#include "src/MainWindow.h"
#include "src/ZFileExtDlg.h"
#include "src/ZMain.h"
#include "src/ZResourceManager.h"
#include "ZImage.h"

void FreeImageMsg(FREE_IMAGE_FORMAT /*fif*/, const char * szMsg)
{
    tstring strMsg = getWStringFromString(szMsg);
    DebugPrintf(strMsg.c_str());
}

/// Entry point
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE /*hPrevInstance*/,
                     _In_ LPTSTR    lpszCmdParam,
                     _In_ int       nCmdShow)
{
  // 기본적인 언어팩은 프로젝트에 있는 영어이다.
  ZResourceManager::GetInstance().SetHandleInstance(hInstance);

  HINSTANCE hLang = NULL;

  // always English Mode if debug mode ( for test )
#ifndef _DEBUG
  if ( GetSystemDefaultLangID() == 0x0412 )
  {
    CMessageManager::getInstance().SetLanguage(eLanguage_KOREAN);

    hLang = LoadLibrary(TEXT("language/korean.dll"));

    if ( hLang )
    {
      ZResourceManager::GetInstance().SetHandleInstance(hLang);
    }
    else
    {
      assert(hLang != NULL);
    }
  }
#endif

  /// 기본 옵션을 불러온다.
  ZOption::GetInstance().LoadOption();

  std::unique_ptr<MiniDumper> dumper_object_holder;
  
  if ( ZOption::GetInstance().IsUseDebug() )
  {
    tstring strDumpFilename = GetDumpFilename();
    TCHAR szDumpMsg[_MAX_PATH];
    SPrintf(szDumpMsg, _MAX_PATH, TEXT("%s\r\n\r\nFile : %s\r\nHomepage : %s"), GetMessage(TEXT("CRASH_MSG")), strDumpFilename.c_str(), g_strHomepage.c_str());

    /// 아래의 객체는 프로그램이 끝날 때에 삭제되어야 한다. 그전에 삭제되면 크래시되었을 때 제대로 덤프를 만들지 못한다.
    dumper_object_holder = std::make_unique<MiniDumper>(strDumpFilename.c_str(), szDumpMsg);
  }

  tstring strCmdString;
  if (lpszCmdParam != nullptr)
  {
    /// 파일 확장자를 연결하라는 거면
    if ( _tcscmp(lpszCmdParam, TEXT("/fileext")) == 0 )	
    {
      int message_box_result = MessageBox(HWND_DESKTOP, GetMessage(TEXT("REG_FILE_TYPE")), TEXT("ZViewer"), MB_YESNO);

      if (message_box_result == IDYES )
      {
        ZFileExtDlg::GetInstance().SaveExtEnv();
      }
      return 0;
    }
    if ( _tcscmp(lpszCmdParam, TEXT("/freezvieweragent")) == 0 )	// uninstall 할 때 ZViewerAgent 를 unload 한다.
    {
      CoFreeUnusedLibraries();
      return 0;
    }

    // 쉘에서 보낼 때는 따옴표로 둘러싸서 준다. 그래서 따옴표를 제거한다.
    if ( _tcslen(lpszCmdParam) > 0 )
    {
      // 만약 따옴표를 포함하고 있으면(바탕화면에서 보냈을 때)
      size_t iLen = _tcslen(lpszCmdParam);
      for ( unsigned int i=0; i<iLen; ++i)
      {
        if ( lpszCmdParam[i] == '\"')
        {
          continue;
        }
        strCmdString.push_back(lpszCmdParam[i]);
      }
    }
  }

#ifdef _DEBUG
  if ( strCmdString.empty() )
  {
    strCmdString = GetProgramFolder();
    strCmdString += TEXT("..\\SampleImages\\GwangAn.jpg");
  }
#endif

  // ~1 형식으로 긴 파일명이 온다면, 원래 긴 패스를 얻는다.
  TCHAR szTemp[MAX_PATH] = { 0 };
  GetLongPathName(strCmdString.c_str(), szTemp, MAX_PATH);

  strCmdString = szTemp;

  tstring strInitArg = strCmdString;

  ZImage::StartupLibrary();

  ZMain::GetInstance().SetInitArg(strInitArg);
  ZMain::GetInstance().SetInstance(hInstance);

  CMainWindow aWindow;
#ifdef _DEBUG
  FreeImage_SetOutputMessage(FreeImageMsg);
#endif

  aWindow.Create(hInstance, HWND_DESKTOP, nCmdShow);

  WPARAM wParam = aWindow.DoModal();

  //
  if ( hLang )
  {
    if ( FALSE == FreeLibrary(hLang) )
    {
      assert(!"Can't free language dll.");
    }
  }

  CacheController::GetInstance().Shutdown();

  CLogManager::getInstance().CleanUp();
  ZImage::CleanupLibrary();

  return (int)wParam;
}

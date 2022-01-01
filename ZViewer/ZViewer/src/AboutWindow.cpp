#include "stdafx.h"
#include "AboutWindow.h"

#include "src/Cache/CacheController.h"
#include "src/ZMain.h"
#include "src/ZResourceManager.h"
#include "ZImage.h"

LRESULT CALLBACK AboutWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

CAboutWindow::CAboutWindow()
{
}


CAboutWindow::~CAboutWindow()
{
}


void CAboutWindow::SetWndProc()
{
  m_wndProc = (WNDPROC)AboutWndProc;
}

void CreateLabel(const HWND hWnd, const TCHAR * msg, int & y)
{
  CreateWindow(TEXT("static"), msg, WS_VISIBLE | WS_CHILD, 20, y, 500, 30,
    hWnd, nullptr, nullptr, nullptr);

  y += 30;
}

LRESULT CALLBACK AboutWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
  switch(iMessage)
  {
    case WM_CREATE:
    {
      int y = 20;
      TCHAR szTemp[MAX_PATH];

      SPrintf(szTemp, MAX_PATH, TEXT("ZViewer Version : %s"), g_strVersion.c_str());
      CreateLabel(hWnd, szTemp, y);

      y += 20;

      const auto& cache_controller = CacheController::GetInstance();
      const int64_t request_count = cache_controller.cache_hit_count() + cache_controller.cache_miss_count();
      int64_t cache_hit_rate = 0;
      if (request_count > 0) {
          cache_hit_rate = 100 * cache_controller.cache_hit_count() / request_count;
      }
      SPrintf(szTemp, MAX_PATH, TEXT("CacheHitRate : %zd%%"), cache_hit_rate);
      CreateLabel(hWnd, szTemp, y);

      SPrintf(szTemp, MAX_PATH, TEXT("%zd"), CacheController::GetInstance().GetCachedKBytes());

      SPrintf(szTemp, MAX_PATH, TEXT("CachedMemory : %sKB, Num of Cached Image : %u"), szTemp, static_cast<unsigned int>(CacheController::GetInstance().GetCachedCount()));
      CreateLabel(hWnd, szTemp, y);

      SPrintf(szTemp, MAX_PATH, TEXT("ProgramPath : %s"), GetProgramFolder().c_str());
      CreateLabel(hWnd, szTemp, y);

      SPrintf(szTemp, MAX_PATH, TEXT("Library Version : %s"), ZImage::GetLibraryVersion().c_str());
      CreateLabel(hWnd, szTemp, y);

      y += 20;

      SPrintf(szTemp, MAX_PATH, TEXT("Homepage : http://zviewer.wimy.com/"));
      CreateLabel(hWnd, szTemp, y);
    }
    return TRUE;

    case WM_DESTROY:
    {
      EnableWindow( ZMain::GetInstance().GetHWND(), TRUE );
      break;
    }
  }
  return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}


void CAboutWindow::DoResource(HWND hParentHWND)
{
  WNDCLASSEX wc = { 0, };
  wc.cbSize			= sizeof(WNDCLASSEX);
  wc.lpfnWndProc		= (WNDPROC)AboutWndProc;
  wc.hInstance		= ZResourceManager::GetInstance().GetHInstance();
  wc.hbrBackground	= GetSysColorBrush(COLOR_3DFACE);
  wc.lpszClassName	= TEXT("AboutWindow");
  RegisterClassEx(&wc);

  HWND hCreatedHandle = CreateWindowEx(
    /* exStyle */		  WS_EX_DLGMODALFRAME
    /* classname */		, TEXT("AboutWindow")
    /* window title */	, TEXT("About")
    /* dwStyle */		, WS_VISIBLE | WS_SYSMENU | WS_CAPTION
    /* x */				, 100
    /* y */				, 100
    /* width */			, 600
    /* height */		, 300
    /* hWndParent */	, hParentHWND
    /* hMenu */			, nullptr
    /* hInstance */		, ZResourceManager::GetInstance().GetHInstance()
    /* lpParam */		, nullptr
    );

  if ( hCreatedHandle == NULL )
  {
    assert(!"Cannot create aboutwindow");
    return;
  }

  // disable parent window to set this dialog modal
  EnableWindow( hParentHWND, FALSE );
}

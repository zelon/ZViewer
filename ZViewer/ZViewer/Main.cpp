#include "stdafx.h"
#include "resource.h"
#include "ZMain.h"
#include "src/ZFileExtDlg.h"
#include "src/ZResourceManager.h"
#include "src/ZCacheImage.h"

int CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
int CALLBACK AboutWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

HMENU hPopupMenu;


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance ,LPSTR lpszCmdParam,int nCmdShow)
{
	// 기본적인 언어팩은 프로젝트에 있는 영어이다.
	ZResourceManager::GetInstance().SetHandleInstance(hInstance);

	HINSTANCE hLang = NULL;

	// debug mode 에서는 항상 영어모드(언어팩 테스트를 위해서)
#ifndef _DEBUG
	if ( GetSystemDefaultLangID() == 0x0412 )
	{
		hLang = LoadLibrary("language/korean.dll");

		if ( hLang )
		{
			ZResourceManager::GetInstance().SetHandleInstance(hLang);
		}
		else
		{
			_ASSERTE(hLang != NULL);
		}
	}
#endif

	/// 파일 확장자를 연결하라는 거면

//	MessageBox(HWND_DESKTOP, strInitArg.c_str(), "asdf", MB_OK);

	if ( strcmp(lpszCmdParam, "/fileext") == 0 )	
	{
		int iRet = MessageBox(HWND_DESKTOP, ZResourceManager::GetInstance().GetString(IDS_ASSOCIATE_FILE_EXTS).c_str(), "ZViewer", MB_YESNO);

		if ( iRet == IDYES )
		{
			ZFileExtDlg::GetInstance().SaveExtEnv();
		}
		return 0;
	}
	else if ( strcmp(lpszCmdParam, "/freezvieweragent") == 0 )	// uninstall 할 때 ZViewerAgent 를 unload 한다.
	{
		CoFreeUnusedLibraries();
		return 0;
	}

	std::string strCmdString;

	// 쉘에서 보낼 때는 따옴표로 둘러싸서 준다. 그래서 따옴표를 제거한다.
	if ( strlen(lpszCmdParam) > 0 )
	{
		// 만약 따옴표를 포함하고 있으면(바탕화면에서 보냈을 때)
		size_t iLen = strlen(lpszCmdParam);
		for ( unsigned int i=0; i<iLen; ++i)
		{
			if ( lpszCmdParam[i] == '\"')
			{
				continue;
			}
			strCmdString.push_back(lpszCmdParam[i]);
		}
	}

	// ~1 형식으로 긴 파일명이 온다면, 원래 긴 패스를 얻는다.
	char szTemp[MAX_PATH] = { 0 };
	GetLongPathName(strCmdString.c_str(), szTemp, MAX_PATH);

	strCmdString = szTemp;

	std::string strInitArg = strCmdString;


	//MessageBox(HWND_DESKTOP, strInitArg.c_str(), "sf", MB_OK);

#ifdef _DEBUG
	strInitArg = "C:\\_Samples\\19028-1.jpg";
	//strInitArg = "C:\\A.bmp";
#endif
	ZImage::StartupLibrary();

	ZMain::GetInstance().setInitArg(strInitArg);

	ZMain::GetInstance().SetInstance(hInstance);


	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;

	char lpszClass[256] = "ZViewer";

	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BIG_MAIN));
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=(WNDPROC)WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	RegisterClass(&WndClass);

	int iWidth = 1000;
	int iHeight = 700;
	int iXPosition = (GetSystemMetrics(SM_CXSCREEN)/2) - (iWidth/2);
	int iYPosition = (GetSystemMetrics(SM_CYSCREEN)/2) - ( iHeight/2) ;

	HMENU hMenu = (HMENU)LoadMenu(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDR_MAIN_MENU));

	ZMain::GetInstance().SetMainMenu(hMenu);

	hWnd=CreateWindow(
		lpszClass,
		lpszClass,		// Window Title
		WS_OVERLAPPEDWINDOW,// | WS_EX_ACCEPTFILES,
		iXPosition,		// 기본 x 위치
		iYPosition,		// 기본 y 위치
		iWidth,				// width
		iHeight,			// height
		NULL,
		hMenu,	// MainMenu
		hInstance,NULL);

	// 탐색기에서의 Drag&Drop 을 가능하게 한다.
	DragAcceptFiles(hWnd, TRUE);

	ShowWindow(hWnd,nCmdShow);

	// 단축키 설정
	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAIN_ACCELERATOR));

	while(GetMessage(&Message,0,0,0))
	{
		if (!TranslateAccelerator(hWnd, hAccel, &Message))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}

	//
	if ( hLang )
	{
		if ( FALSE == FreeLibrary(hLang) )
		{
			_ASSERTE(!"Can't free language dll.");
		}
	}

	ZImage::CleanupLibrary();

	return (int)Message.wParam;
//	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, WndProc);
	return 0;
}

bool m_bCapture = false;
int lastX;
int lastY;

int CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch(iMessage)
	{
	case WM_DROPFILES:
		{
			HDROP hDrop = (HDROP)wParam;

			UINT iFileNum = 0;

			iFileNum = DragQueryFile(hDrop, 0xffffffff, 0, 0);

			if ( iFileNum <= 0 ) break;

			char szFileName[MAX_PATH] = { 0 };
			DragQueryFile(hDrop, 0, szFileName, MAX_PATH);

			if ( ZImage::IsValidImageFileExt(szFileName) )
			{
				ZMain::GetInstance().OpenFile(szFileName);
			}
			else
			{
				MessageBox(hWnd, "Invalid image file", "ZViewer", MB_OK);
			}
		}
		break;

	case WM_LBUTTONDBLCLK:
		{
			ZMain::GetInstance().OpenFileDialog();
		}
		break;

	case WM_LBUTTONDOWN:
		{
			m_bCapture = true;
			SetCapture(hWnd);

			lastX = LOWORD(lParam);
			lastY = HIWORD(lParam);
		}
		break;

	case WM_LBUTTONUP:
		{
			m_bCapture = false;
			ReleaseCapture();
		}
		break;

	case WM_MOUSEMOVE:
		{
			if ( m_bCapture )
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				int diffX = x - lastX;
				int diffY = y - lastY;

				ZMain::GetInstance().OnDrag(-diffX, -diffY);
				lastX = x;
				lastY = y;
			}
		}
		break;

	case WM_CREATE:
		{
			InitCommonControls();
			// 아이콘을 지정해준다.
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(ZMain::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDI_BIG_MAIN)));

			// StatusBar 를 생성한다.
			HWND hStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "Status line", hWnd, 0);
			ZMain::GetInstance().SetStatusHandle(hStatusBar);

			// StatusBar 를 split 한다.
			int SBPart[6] =
			{
				70,		/// %d/%d 현재보고 있는 이미지 파일의 index number
				200,	/// %dx%dx%dbpp 해상도와 color depth, image size
				300,		/// image size
				420,	/// temp banner http://www.wimy.com
				500,	/// 파일을 읽어들이는데 걸린 시간
				2000,	/// 파일명표시
			};
			SendMessage(hStatusBar, SB_SETPARTS, 6, (LPARAM)SBPart);

			// 팝업 메뉴를 불러놓는다.
			HMENU hMenu = LoadMenu(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDR_POPUP_MENU));
			hPopupMenu = GetSubMenu(hMenu, 0);
			ZMain::GetInstance().SetPopupMenu(hPopupMenu);

			ZMain::GetInstance().SetHWND(hWnd);
			ZMain::GetInstance().OnInit();
			return TRUE;
		}
		break;

	case WM_CLOSE:
		{
			if ( ZMain::GetInstance().IsFullScreen() )
			{
				ZMain::GetInstance().ShellTrayShow();
			}
			PostQuitMessage(0);
		}
		break;

	case WM_SETCURSOR:
		{
			RECT rt;
			GetClientRect(hWnd, &rt);
			if ( ZMain::GetInstance().IsFullScreen() == false ) rt.bottom -= STATUSBAR_HEIGHT;	// StatusBar 를 위해 뺀다.

			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(hWnd, &pt);

			if ( PtInRect(&rt, pt) )
			{
				if ( ZMain::GetInstance().m_bHandCursor )
				{
					if ( HIWORD(lParam) == 513 )	// 마우스 왼쪽 버튼을 누르고 있으면
					{
						SetCursor(LoadCursor(ZMain::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDC_MOVE_HAND_CAPTURE_CURSOR)));
					}
					else
					{
						SetCursor(LoadCursor(ZMain::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDC_MOVE_HAND_CURSOR)));
					}

					//DebugPrintf("LoadWait");
				}
				else
				{
					SetCursor(LoadCursor(NULL, IDC_ARROW));
					//DebugPrintf("LoadArrow");
				}
				return 0;
			}

		}
		break;

	case WM_MOUSEWHEEL:		// WM_MOUSEWHEEL
		{
			short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			//DebugPrintf("Wheel Delta : %d", zDelta);

#pragma message("ToDo : 휠이 급격히 돌아갈 때는 여러개의 이미지를 뛰어넘게")

			bool bDraw = false;
			if ( zDelta < 0 )
			{
				bDraw = ZMain::GetInstance().NextImage();
			}
			else
			{
				bDraw = ZMain::GetInstance().PrevImage();
			}

			// 이미지를 넘기지 않았으면 새로 그리지 않는다.
			if ( bDraw ) ZMain::GetInstance().Draw();
		}
		break;

	case WM_SIZE:
		{
			SendMessage(ZMain::GetInstance().GetStatusHandle(), WM_SIZE, wParam, lParam);

			RECT rt;
			GetClientRect(hWnd, &rt);
			if ( ZMain::GetInstance().IsFullScreen() == false ) rt.bottom -= STATUSBAR_HEIGHT;

			ZMain::GetInstance().OnChangeCurrentSize(rt.right, rt.bottom);
		}
		break;

	case WM_MBUTTONDOWN:
		{
			ZMain::GetInstance().ToggleFullScreen();
		}
		break;

	case WM_SETFOCUS:
		{
			ZMain::GetInstance().OnFocusGet();
		}
		break;

	case WM_KILLFOCUS:
		{
			ZMain::GetInstance().OnFocusLose();
		}
		break;

	case WM_CONTEXTMENU:
		{
			ZMain::GetInstance().m_bHandCursor = false;
			TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), 0, hWnd, NULL);

		}
		break;

	case WM_COMMAND:
		{
			int wMid = LOWORD(wParam);
			int wmEvent = HIWORD(wParam);

			switch ( wParam )
			{
			case IDOK:
			case ID_MAINMENU_FILE_EXIT:
				SendMessage(hWnd, WM_CLOSE, 0, 0);
				//PostQuitMessage(0);
				break;
			}

			switch ( wMid )
			{
				/////////////////////////////////////////////
				// Main Menu

			case ID_MAINMENU_FILE_EXIT:
				{
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					//PostQuitMessage(0);
				}
				break;

			case ID_MAINMENU_FILE_OPEN:
				{
					ZMain::GetInstance().OpenFileDialog();
				}
				break;

			case ID_MOVE_NEXTFOLDER:
				{
					ZMain::GetInstance().NextFolder();
				}
				break;

			case ID_MOVE_PREVFOLDER:
				{
					ZMain::GetInstance().PrevFolder();
				}
				break;

			case ID_MOVE_NEXTIMAGE:
				{
					if ( ZMain::GetInstance().NextImage() )
					{
						ZMain::GetInstance().Draw();
					}
				}
				break;

			case ID_MOVE_PREVIMAGE:
				{
					if ( ZMain::GetInstance().PrevImage() )
					{
						ZMain::GetInstance().Draw();
					}
				}
				break;

			case ID_MOVE_FIRSTIMAGE:
				{
					if ( ZMain::GetInstance().FirstImage() )
					{
						ZMain::GetInstance().Draw();
					}
				}
				break;

			case ID_MOVE_LASTIMAGE:
				{
					if ( ZMain::GetInstance().LastImage() )
					{
						ZMain::GetInstance().Draw();
					}
				}
				break;

			case ID_VIEW_FULLSCREEN:
				{
					ZMain::GetInstance().ToggleFullScreen();
				}
				break;

			case ID_HELP_ABOUT:
				{
					DialogBox(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDD_DIALOGHELP), hWnd, AboutWndProc);
				}
				break;

			case ID_ACCELERATOR_CANCEL_FULLSCREEN:
				{
					// 현재 풀 스크린 일 때만 풀스크린을 취소한다.
					if ( ZMain::GetInstance().IsFullScreen() )
					{
						ZMain::GetInstance().ToggleFullScreen();
					}
				}
				break;

			case ID_MOVE_UNDOIMAGEPOSITION:
				{
					ZMain::GetInstance().Undo();
					ZMain::GetInstance().Draw();
				}
				break;

			case ID_MOVE_REDOIMAGEPOSITION:
				{
					ZMain::GetInstance().Redo();
					ZMain::GetInstance().Draw();
				}
				break;

			case ID_ACCELERATOR_RIGHT:
				{
					ZMain::GetInstance().OnDrag(100, 0);
				}
				break;

			case ID_ACCELERATOR_LEFT:
				{
					ZMain::GetInstance().OnDrag(-100, 0);
				}
				break;

			case ID_ACCELERATOR_UP:
				{
					ZMain::GetInstance().OnDrag(0, -100);
				}
				break;

			case ID_ACCELERATOR_DOWN:
				{
					ZMain::GetInstance().OnDrag(0, 100);
				}
				break;

			case ID_VIEW_RIGHTTOPFIRSTDRAW:
				{
					ZMain::GetInstance().OnRightTopFirstDraw();
				}
				break;

			case ID_OPTION_FILE_EXT:
				ZMain::GetInstance().ShowFileExtDlg();
				break;

			case ID_VIEW_BIGTOSCREENSTRETCH:
			case ID_POPUPMENU_BIGTOSCREENSTRETCH:
				ZMain::GetInstance().ToggleBigToScreenStretch();
				break;

			case ID_VIEW_ROTATECLOCKWISE:
				ZMain::GetInstance().Rotate(true);
				break;

			case ID_VIEW_ROTATECOUNTERCLOCKWISE:
				ZMain::GetInstance().Rotate(false);
				break;

			case ID_DELETETHISFILE:
				ZMain::GetInstance().DeleteThisFile();
				break;

			case ID_SETDESKTOPWALLPAPER_CENTER:
				ZMain::GetInstance().SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle_CENTER);
				break;

			case ID_SETDESKTOPWALLPAPER_TILE:
				ZMain::GetInstance().SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle_TILE);
				break;

			case ID_SETDESKTOPWALLPAPER_STRETCH:
				ZMain::GetInstance().SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle_STRETCH);
				break;

			case ID_SETDESKTOPWALLPAPER_CLEAR:
				ZMain::GetInstance().ClearDesktopWallPaper();
				break;
				// End of Main Menu
				/////////////////////////////////////////////

			}
		}
		break;

	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);

		ZMain::GetInstance().Draw();
		EndPaint(hWnd, &ps);

		SendMessage(ZMain::GetInstance().GetStatusHandle(), WM_PAINT, wParam, lParam);

		DebugPrintf("Recv WM_PAINT");
		return 0;
	case WM_DESTROY:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		//PostQuitMessage(0);
		return 0;
	}
	return (int)(DefWindowProc(hWnd,iMessage,wParam,lParam));

}

int CALLBACK AboutWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		{
			SetDlgItemText(hWnd, IDC_STATIC_VERSION, g_strVersion.c_str());

			char szTemp[MAX_PATH];
			sprintf(szTemp, "CacheHitRate : %d%%", ZMain::GetInstance().GetLogCacheHitRate());
			SetDlgItemText(hWnd, IDC_STATIC_HITRATE, szTemp);

			NUMBERFMT nFmt = { 0, 0, 3, ".", ",", 1 };

			TCHAR szOUT[20];
			sprintf(szTemp, "%d",ZMain::GetInstance().GetCachedKByte());
			::GetNumberFormat(NULL, NULL, szTemp, &nFmt, szOUT, 20);

			sprintf(szTemp, "CachedMemory : %sKB, Num of Cached Image : %d", szOUT, ZCacheImage::GetInstance().GetNumOfCacheImage());
			SetDlgItemText(hWnd, IDC_STATIC_CACHE_MEMORY, szTemp);

			sprintf(szTemp, "ProgramPath : %s", ZMain::GetInstance().GetProgramFolder().c_str());
			SetDlgItemText(hWnd, IDC_STATIC_PROGRAM_PATH, szTemp);

			sprintf(szTemp, "Library Version : %s", ZImage::GetLibraryVersion());
			SetDlgItemText(hWnd, IDC_STATIC_LIBRARY_VERSION, szTemp);
		}
		return TRUE;

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDOK:
				EndDialog(hWnd, 0);
				break;
			}
		}
		break;
	
	}
	return FALSE;
}
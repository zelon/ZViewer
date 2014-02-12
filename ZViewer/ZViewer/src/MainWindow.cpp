/* ------------------------------------------------------------------------
 *
 * Copyright 2006
 *
 * MainWindow.cpp
 *
 * 2006.11.4 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"
#include "../res/resource.h"

#include "MainWindow.h"
#include "AboutWindow.h"
#include "TaskBar.h"

#include "../commonSrc/MessageManager.h"
#include "../commonSrc/ExtInfoManager.h"
#include "../commonSrc/ZOption.h"

#include "src/ShortCut.h"
#include "src/ZMain.h"
#include "src/ZResourceManager.h"

enum
{
	ARROW_MOVEMENT_LENGTH = 100		///< 화면보다 큰 그림을 볼 때, 방향키를 눌렀을 때 움직이는 정도
};

bool m_bCapture = false;


HMENU g_hPopupMenu;

int CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

CMainWindow::CMainWindow()
{
	SetWndProc();
}


CMainWindow::~CMainWindow()
{
}


void CMainWindow::SetWndProc()
{
	m_wndProc = (WNDPROC)WndProc;
}


HWND CMainWindow::Create(HINSTANCE hInstance, HWND hParentHWND, int nCmdShow)
{
	SetWndProc();
	if ( NULL == m_wndProc)
	{
		assert(!"before create, set WndProc");
		return (HWND)INVALID_HANDLE_VALUE;
	}

	m_hParentWindow = hParentHWND;

	TCHAR lpszClass[256] = TEXT("ZViewer");

	WNDCLASS WndClass;
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BIG_MAIN));
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=m_wndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	RegisterClass(&WndClass);

	int iScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	int iScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);

	int iWidth = (int)(iScreenWidth * 0.8);
	int iHeight = (int)(iScreenHeight * 0.8);

	int iXPosition = (iScreenWidth/2) - (iWidth/2);
	int iYPosition = (iScreenHeight/2) - ( iHeight/2) ;

	HMENU hMenu = (HMENU)LoadMenu(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDR_MAIN_MENU));

	ZMain::GetInstance().SetMainMenu(hMenu);

	m_hWindow = ::CreateWindow(
		lpszClass,
		lpszClass,		///< Window Title
		WS_OVERLAPPEDWINDOW,///< | WS_EX_ACCEPTFILES,
		iXPosition,		///< 기본 x 위치
		iYPosition,		///< 기본 y 위치
		iWidth,			///< width
		iHeight,		///< height
		NULL,			///< parent handle
		hMenu,			///< MainMenu
		hInstance,NULL);

	// 탐색기에서의 Drag&Drop 을 가능하게 한다.
	DragAcceptFiles(m_hWindow, TRUE);

	ShowWindow(m_hWindow, nCmdShow);

	// 단축키 설정. 여기서 반환된 핸들은 프로그램이 종료될 때 자동적으로 close 됨.
	m_hAccel = ShortCut::GetInstance().MakeAccelTable();


	return m_hWindow;
}

HMENU CMainWindow::CreatePopupMenu()
{
	HMENU hPopupMenu = ::CreatePopupMenu();

	AppendMenu(hPopupMenu, MF_STRING, ID_MOVE_FIRSTIMAGE, GetMessage(TEXT("POPUPMENU_FIRST")));
	AppendMenu(hPopupMenu, MF_STRING, ID_MOVE_LASTIMAGE, GetMessage(TEXT("POPUPMENU_LAST")));
	AppendMenu(hPopupMenu, MF_SEPARATOR, 0, nullptr);
	AppendMenu(hPopupMenu, MF_STRING, ID_VIEW_FULLSCREEN, GetMessage(TEXT("POPUPMENU_FULL_SCREEN")));
	AppendMenu(hPopupMenu, MF_STRING, ID_VIEW_RIGHTTOPFIRSTDRAW, GetMessage(TEXT("POPUPMENU_RIGHT_TOP_FIRST_DRAW")));
	AppendMenu(hPopupMenu, MF_SEPARATOR, 0, nullptr);
	AppendMenu(hPopupMenu, MF_STRING, ID_POPUPMENU_BIGTOSCREENSTRETCH, GetMessage(TEXT("POPUPMENU_STRETCH_BIG_TO_SCREEN")));
	AppendMenu(hPopupMenu, MF_STRING, ID_POPUPMENU_SMALLTOSCREENSTRETCH, GetMessage(TEXT("POPUPMENU_STRETCH_SMALL_TO_SCREEN")));
	AppendMenu(hPopupMenu, MF_SEPARATOR, 0, nullptr);
	AppendMenu(hPopupMenu, MF_STRING, ID_DELETETHISFILE, GetMessage(TEXT("POPUPMENU_DELETE")));
	AppendMenu(hPopupMenu, MF_SEPARATOR, 0, nullptr);
	{
		HMENU subPopup = ::CreatePopupMenu();
		AppendMenu(subPopup, MF_STRING, ID_SETDESKTOPWALLPAPER_CENTER, GetMessage(TEXT("POPUPMENU_DESKTOP_WALLPAPER_CENTER")));
		AppendMenu(subPopup, MF_STRING, ID_SETDESKTOPWALLPAPER_STRETCH, GetMessage(TEXT("POPUPMENU_DESKTOP_WALLPAPER_STRETCH")));
		AppendMenu(subPopup, MF_STRING, ID_SETDESKTOPWALLPAPER_TILE, GetMessage(TEXT("POPUPMENU_DESKTOP_WALLPAPER_TILE")));
		AppendMenu(subPopup, MF_SEPARATOR, 0, nullptr);
		AppendMenu(subPopup, MF_STRING, ID_SETDESKTOPWALLPAPER_CLEAR, GetMessage(TEXT("POPUPMENU_DESKTOP_WALLPAPER_CLEAR")));
		AppendMenu(hPopupMenu, MF_POPUP, (UINT_PTR)subPopup, GetMessage(TEXT("POPUPMENU_SET_DESKTOP_WALLPAPER")));
	}
	AppendMenu(hPopupMenu, MF_STRING, ID_START_SLIDESHOW, GetMessage(TEXT("POPUPMENU_START_SLIDESHOW")));
	AppendMenu(hPopupMenu, MF_STRING, ID_MAINMENU_FILE_EXIT, GetMessage(TEXT("POPUPMENU_EXIT")));

	return hPopupMenu;
}

int CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static int lastX;
	static int lastY;

	switch(iMessage)
	{
	case WM_CHAR:
		{
			switch ( wParam )
			{
			case '{':
				ZMain::GetInstance().DecreaseOpacity();
				break;

			case '}':
				ZMain::GetInstance().IncreaseOpacity();
				break;

			case 'g':
			case 'G':
				ZMain::GetInstance().StartSlideMode();
				break;

			case 'f':
			case 'F':
				ZMain::GetInstance().ToggleFullScreen();
				break;

			case 'p':
				ZMain::GetInstance().ShowExif();
				break;

			case 'A':
			case 'a':
			case '7':
				ZMain::GetInstance().OnDrag(-5000, -5000);
				break;

			case 'S':
			case 's':
			case '9':
				ZMain::GetInstance().OnDrag(5000, -5000);
				break;

			case 'Z':
			case 'z':
			case '1':
				ZMain::GetInstance().OnDrag(-5000, 5000);
				break;

			case 'X':
			case 'x':
			case '3':
				ZMain::GetInstance().OnDrag(5000, 5000);
				break;

			case '=':
			case '+':
				ZMain::GetInstance().ZoomIn();
				break;
			case '-':
				ZMain::GetInstance().ZoomOut();
				break;
			case '0':
				ZMain::GetInstance().ZoomNone();
				break;
/// 디버그 모드에서만 작동하는 단축키들
#ifdef _DEBUG
			case '`':
				ZCacheImage::GetInstance().debugShowCacheInfo();
				break;

			case '~':
				{
					TIMECHECK_START("clear spend time");
					ZCacheImage::GetInstance().clearCache();
					TIMECHECK_END();
				}
				break;

			case 'n':
				{
				}
				break;
			case 'm':
				{
					fipMultiPage mimage;
					mimage.open("C:\\b.gif", FALSE, TRUE);

					int iCount = mimage.getPageCount();

					for ( int i=0; i<iCount; ++i )
					{
						DebugPrintf(TEXT("-- pregress gif --"));
						fipWinImage k;

						fipImage pp;

						FIBITMAP* pDatap = mimage.lockPage(i);

						fipImage pre;
						if ( pDatap )
						{
							pp = pDatap;
							if ( pp.isValid() )
							{
								if ( i == 0 )
								{
									pre = pp;
									k = pp;
									k.convertTo32Bits();
								}
								else
								{
									BOOL bRet = k.pasteSubImage(pp, 0, 0);

									if ( bRet )
									{
										DebugPrintf(TEXT("pastesubimage ok"));
									}
									else
									{
										DebugPrintf(TEXT("pastesubimage failed"));
									}
									k = pp;
									//k.get
								}
								RECT r = { 0, 0, 100, 100 };
								r.right = k.getWidth();
								r.bottom = k.getHeight();
								//k.pas
								//RGBQUAD quad;
								//k.getFileBkColor(&quad);
								//COLORREF col = RGB( quad.rgbRed, quad.rgbGreen, quad.rgbBlue );
								//::SetBkColor(GetDC(hWnd), col);
								//::SetBkColor(GetDC(hWnd), RGB( 255,255,255 ));
								//SetBkMode(GetDC(hWnd), TRANSPARENT);
								//k.rotate(90);
								//k.draw(GetDC(hWnd), r);
								//k.drawEx(GetDC(hWnd), r, TRUE, &quad);
								k.drawEx(GetDC(hWnd), r, FALSE, NULL, (FIBITMAP*)pre);
								
								mimage.unlockPage(pp, FALSE);
							}
						}
					}

					mimage.close(0);

				}
				break;
#endif
			}
		}
		break;

	case WM_DROPFILES:///< 탐색기에서 드래그 앤 드랍으로 놓았을 때
		{
			HDROP hDrop = (HDROP)wParam;

			UINT iFileNum = 0;

			iFileNum = DragQueryFile(hDrop, 0xffffffff, 0, 0);

			if ( iFileNum <= 0 ) break;

			TCHAR szFileName[MAX_PATH] = { 0 };
			DragQueryFile(hDrop, 0, szFileName, MAX_PATH);

			if ( ExtInfoManager::GetInstance().IsValidImageFileExt(szFileName) )
			{
				ZMain::GetInstance().OpenFile(szFileName);
			}
			else
			{
				MessageBox(hWnd, GetMessage(TEXT("INVALID_IMAGE_FILE")), TEXT("ZViewer"), MB_OK);
			}
		}
		break;

	case WM_LBUTTONDBLCLK:
		ZMain::GetInstance().OpenFileDialog();
		break;

	case WM_LBUTTONDOWN:
		{
			m_bCapture = true;
			SetCapture(hWnd);

			lastX = LOWORD(lParam);
			lastY = HIWORD(lParam);

			HandCursorProc();
		}
		break;

	case WM_LBUTTONUP:
		{
			m_bCapture = false;
			ReleaseCapture();
			HandCursorProc();
		}
		break;

	case WM_MOUSEMOVE:
		{
			if ( m_bCapture )
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);

				ZMain::GetInstance().OnDrag(-(x - lastX), -(y - lastY));
				lastX = x;
				lastY = y;

				HandCursorProc();
			}
		}
		break;

	case WM_CREATE:
		{
			ZMain::GetInstance().SetHWND(hWnd);

			InitCommonControls();
			// 아이콘을 지정해준다.
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(ZMain::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDI_BIG_MAIN)));

			/// 그림을 표시할 child window 를 만든다.
			ZMain::GetInstance().CreateShowWindow();

			ZMain::GetInstance().CreateStatusBar();

			// 팝업 메뉴를 불러놓는다.
			HMENU hMenu = CMainWindow::CreatePopupMenu();  //LoadMenu(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDR_POPUP_MENU));
			g_hPopupMenu = hMenu; // GetSubMenu(hMenu, 0);
			ZMain::GetInstance().SetPopupMenu(g_hPopupMenu);

			ZMain::GetInstance().OnInit();

			/// 타이머를 시작한다.
			ZMain::GetInstance().StartTimer();
			return TRUE;
		}
		break;

	case WM_CLOSE:
		{
			if ( ZOption::GetInstance().IsFullScreen() )
			{
				TaskBar::ShellTrayShow();
			}

			ZMain::GetInstance().StopTimer();
			PostQuitMessage(0);
		}
		break;

	case WM_SETCURSOR:
		{
			RECT rt;
			ZMain::GetInstance().getCurrentScreenRect(rt);

			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(hWnd, &pt);

			if ( PtInRect(&rt, pt) || m_bCapture )
			{
				HandCursorProc();
				return 0;
			}

		}
		break;

	case WM_MOUSEWHEEL:
		{
			bool bPushedControl = false;
			if ( (LOWORD(wParam) & MK_CONTROL) == MK_CONTROL )
			{
				bPushedControl = true;
			}

			ZMain::GetInstance().OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), bPushedControl);
		}
		break;

	case WM_SIZE:
		{
			/// ShowWindow 크기를 조절한다.
			ZMain::GetInstance().AdjustShowWindowScreen();

			/// StatusBar 크기를 조절한다.
			SendMessage(ZMain::GetInstance().GetStatusHandle(), WM_SIZE, wParam, lParam);
			ZMain::GetInstance().OnWindowResized();
		}
		break;

	case WM_MBUTTONDOWN:
		ZMain::GetInstance().ToggleFullScreen();
		break;

	case WM_SETFOCUS:
		ZMain::GetInstance().OnFocusGet();
		break;

	case WM_KILLFOCUS:
		ZMain::GetInstance().OnFocusLose();
		break;

	case WM_CONTEXTMENU:
		{
			ZMain::GetInstance().SetHandCursor(false);	///< When popup menu, change to arrow cursor

			POINT p;
			GetCursorPos(&p);
			TrackPopupMenuEx(g_hPopupMenu, TPM_RIGHTBUTTON, p.x, p.y, hWnd, NULL);
		}
		break;

	case WM_COMMAND:
		{
			if ( ZOption::GetInstance().m_bSlideMode ) ZOption::GetInstance().m_bSlideMode = false;

			int wMid = LOWORD(wParam);
			//int wmEvent = HIWORD(wParam);

			switch ( wParam )
			{
			case IDOK:
			case ID_MAINMENU_FILE_EXIT:
				ZMain::GetInstance().CloseProgram();
				break;
			}

			switch ( wMid )
			{
			case ID_ESC_KEY_OPERATION:
				ZMain::GetInstance().OnPressedESCKey();
				break;

				/////////////////////////////////////////////
				// Main Menu

			case ID_VIEW_ALWAYSONTOP:
				ZMain::GetInstance().ToggleAlwaysOnTop();
				break;
			case ID_START_SLIDESHOW:
				ZMain::GetInstance().StartSlideMode();
				break;

			case ID_SHOW_CACHED_FILENAME:
				ZCacheImage::GetInstance().ShowCachedImageToOutputWindow();
				break;

			case ID_MAINMENU_FILE_EXIT:
				ZMain::GetInstance().CloseProgram();
				break;

			case ID_MAINMENU_FILE_OPEN:
				ZMain::GetInstance().OpenFileDialog();
				break;

			case ID_FILE_SAVE_AS_NEW_EXT:
				ZMain::GetInstance().SaveFileDialog();
				break;

			case ID_FILE_MOVETO:
				ZMain::GetInstance().MoveThisFile();
				break;

			case ID_FILE_COPYTO:
				ZMain::GetInstance().CopyThisFile();
				break;

			case ID_MOVE_NEXTFOLDER:
				ZMain::GetInstance().NextFolder();
				break;

			case ID_MOVE_PREVFOLDER:
				ZMain::GetInstance().PrevFolder();
				break;

			case ID_MOVE_NEXTIMAGE:
				ZMain::GetInstance().NextImage();
				break;

			case ID_MOVE_PREVIMAGE:
				ZMain::GetInstance().PrevImage();
				break;

			case ID_MOVE_NEXT_JUMP:
				{
					if ( ZMain::GetInstance().MoveRelateIndex(+10) )
					{
						ZMain::GetInstance().Draw();
					}
				}
				break;

			case ID_MOVE_PREV_JUMP:
				{
					if ( ZMain::GetInstance().MoveRelateIndex(-10) )
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

			case ID_ZOOM_100:
				ZMain::GetInstance().ZoomNone();
				break;
			case ID_ZOOM_IN:
				ZMain::GetInstance().ZoomIn();
				break;
			case ID_ZOOM_OUT:
				ZMain::GetInstance().ZoomOut();
				break;

			case ID_VIEW_FULLSCREEN:
				ZMain::GetInstance().ToggleFullScreen();
				break;

			case ID_COPY_TO_CLIPBOARD:
				ZMain::GetInstance().CopyToClipboard();
				break;
			case ID_SORT_FILENAME:
				ZMain::GetInstance().ChangeFileSort(eFileSortOrder_FILENAME);
				break;

			case ID_SORT_FILESIZE:
				ZMain::GetInstance().ChangeFileSort(eFileSortOrder_FILESIZE);
				break;

			case ID_SORT_FILEDATE:
				ZMain::GetInstance().ChangeFileSort(eFileSortOrder_LAST_MODIFY_TIME);
				break;

			case ID_FILE_RESCAN_FOLDER:
				ZMain::GetInstance().ReLoadFileList();
				break;

			case ID_HELP_ABOUT:
				{
					CAboutWindow win;
					win.DoResource(hWnd);
				}
				break;

			case ID_SHOW_DETAILINFOMATION:
				ZMain::GetInstance().ShowExif();
				break;
			case ID_ACCELERATOR_CANCEL_FULLSCREEN:
				// 현재 풀 스크린 일 때만 풀스크린을 취소한다.
				if ( ZOption::GetInstance().IsFullScreen() )
				{
					ZMain::GetInstance().ToggleFullScreen();
				}
				break;

			case ID_MOVE_UNDOIMAGEPOSITION:
				ZMain::GetInstance().Undo();
				ZMain::GetInstance().Draw();
				break;

			case ID_MOVE_REDOIMAGEPOSITION:
				ZMain::GetInstance().Redo();
				ZMain::GetInstance().Draw();
				break;

			case ID_ACCELERATOR_RIGHT:
				ZMain::GetInstance().OnDrag(ARROW_MOVEMENT_LENGTH, 0);
				break;

			case ID_ACCELERATOR_LEFT:
				ZMain::GetInstance().OnDrag(-ARROW_MOVEMENT_LENGTH, 0);
				break;

			case ID_ACCELERATOR_UP:
				ZMain::GetInstance().OnDrag(0, -ARROW_MOVEMENT_LENGTH);
				break;

			case ID_ACCELERATOR_DOWN:
				ZMain::GetInstance().OnDrag(0, ARROW_MOVEMENT_LENGTH);
				break;

			case ID_VIEW_RIGHTTOPFIRSTDRAW:
				ZMain::GetInstance().OnRightTopFirstDraw();
				break;

			case ID_AUTOROTATION:
				ZMain::GetInstance().ToggleAutoRotation();
				break;

			case ID_OPTION_VIEWLOOP:
				ZMain::GetInstance().ToggleLoopImage();
				break;

			case ID_OPTION_FILE_EXT:
				ZMain::GetInstance().ShowFileExtDlg();
				break;

			case ID_VIEW_BIGTOSCREENSTRETCH:
			case ID_POPUPMENU_BIGTOSCREENSTRETCH:
				ZMain::GetInstance().ToggleBigToScreenStretch();
				break;

			case ID_VIEW_SMALLTOSCREENSTRETCH:
			case ID_POPUPMENU_SMALLTOSCREENSTRETCH:
				ZMain::GetInstance().ToggleSmallToScreenStretch();
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

			case ID_DELETETHISFILE_RECYCLEBIN:
				ZMain::GetInstance().DeleteThisFileToRecycleBin();
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
				CDesktopWallPaper::ClearDesktopWallPaper();
				break;
				// End of Main Menu
				/////////////////////////////////////////////

			}
		}
		break;

	case WM_PAINT:
		HDC hdc;
		PAINTSTRUCT ps;
		// 아래의 BeginPaint/EndPaint 부분을 지우면 WM_PAINT 가 무한히 호출된다.
		hdc=BeginPaint(ZMain::GetInstance().GetHWND(), &ps);
		EndPaint(ZMain::GetInstance().GetHWND(), &ps);

		hdc=BeginPaint(ZMain::GetInstance().GetShowWindow(), &ps);
		ZMain::GetInstance().Draw(hdc);
		//ZMain::GetInstance().LoadCurrent();
		EndPaint(ZMain::GetInstance().GetShowWindow(), &ps);

		DebugPrintf(TEXT("Recv WM_PAINT"));
		return 0;
	case WM_DESTROY:
		ZMain::GetInstance().CloseProgram();
		return 0;

	case WM_TIMER:
		ZMain::GetInstance().onTimer();
		return 0;
	}
	return (int)(DefWindowProc(hWnd,iMessage,wParam,lParam));

}

void HandCursorProc()
{
	if ( ZMain::GetInstance().IsHandCursor() )
	{
		if ( m_bCapture )	// 마우스 왼쪽 버튼을 누르고 있으면 움켜쥔 커서를 그린다.
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
}

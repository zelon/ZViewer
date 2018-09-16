#include "stdafx.h"
#include "ZMain.h"

#include "../commonSrc/CommonFunc.h"
#include "../commonSrc/DesktopWallPaper.h"
#include "../commonSrc/ElapseTime.h"
#include "../commonSrc/ExtInfoManager.h"
#include "../commonSrc/Path.h"
#include "../commonSrc/SaveAs.h"
#include "../commonSrc/ZOption.h"
#include "MessageDefine.h"
#include "MessageManager.h"
#include "resource.h"
#include "src/CacheManager.h"
#include "src/ExifDlg.h"
#include "src/SelectToFolderDlg.h"
#include "src/ZFileExtDlg.h"
#include "src/ZResourceManager.h"
#include "TaskBar.h"
#include "ZHistory.h"
#include "ZImage.h"

using namespace std;
using namespace std::chrono;

static const int kTimerId = 9153;

ZMain& ZMain::GetInstance() {
  static ZMain aInstance;
  return aInstance;
}

ZMain::ZMain(void)
	: main_window_handle_(nullptr),
		m_sortOrder(eFileSortOrder_FILENAME),
		m_history(std::make_unique<ZHistory>()),
		m_hBufferDC(nullptr),
		m_alpha(255) {
  memset( &m_lastPosition, 0, sizeof( m_lastPosition ) );
  
  background_brush_ = CreateSolidBrush(RGB(128,128,128));
  m_hEraseRgb = CreateRectRgn(0,0,1,1);

  assert(background_brush_ != INVALID_HANDLE_VALUE);
}

ZMain::~ZMain() {
  if (m_hBufferDC != nullptr) {
    DebugPrintf(TEXT("Before delete bufferDC"));
    const BOOL bRet = DeleteDC(m_hBufferDC);
    if (bRet == FALSE) {
      assert(false);
    }

    DebugPrintf(TEXT("after delete bufferDC"));

    m_hBufferDC = nullptr;
  }

  if ( background_brush_ != INVALID_HANDLE_VALUE ) {
    DeleteObject(background_brush_);
  }
}

void ZMain::OnFileCached() {
  PostMessage(main_window_handle_, WM_CHECK_CURRENT_IMAGE_IS_CACHED, 0, 0);
}

void ZMain::SetImageAndShow(std::shared_ptr<ZImage> image) {
  current_image_ = image;
  assert(current_image_);
  assert(current_image_->IsValid());

  m_iShowingX = 0;
  m_iShowingY = 0;

  if ( ZOption::GetInstance().m_bRightTopFirstDraw ) {// 우측 상단부터 시작해야하면
    RECT rt;
    getCurrentScreenRect(rt);

    if ( current_image_->GetWidth() > rt.right ) {
      m_iShowingX = current_image_->GetWidth() - rt.right;
    }
  }

  /// 기본적으로 그림의 zoom rate 를 설정한다.
  m_fCurrentZoomRate = 1.0f;
  RECT currentScreenRect;
  if ( false == getCurrentScreenRect(currentScreenRect) ) {
    assert(false);
  } else {
    if ( ZOption::GetInstance().IsBigToSmallStretchImage() || ZOption::GetInstance().IsSmallToBigStretchImage() ) {
      RECT toRect;
      SetRect(&toRect, 0, 0, current_image_->GetWidth(), current_image_->GetHeight());

      if ( ZOption::GetInstance().IsBigToSmallStretchImage() ) {
        if ( current_image_->GetWidth() > (currentScreenRect.right - currentScreenRect.left) || current_image_->GetHeight() > (currentScreenRect.bottom - currentScreenRect.top) ) {
          toRect = GetResizedRectForBigToSmall(currentScreenRect, toRect);
        }
      }

      if ( ZOption::GetInstance().IsSmallToBigStretchImage() ) {
        if ( current_image_->GetWidth() < (currentScreenRect.right - currentScreenRect.left) && current_image_->GetHeight() < (currentScreenRect.bottom - currentScreenRect.top) ) {
          toRect = GetResizedRectForSmallToBig(currentScreenRect, toRect);
        }
      }
      m_fCurrentZoomRate = (float)(toRect.right - toRect.left) / (float)(current_image_->GetWidth());
    }
  }
  Draw();

  SetStatusBarText();	///< 상태 표시줄 내용을 설정한다.
}

/// 버퍼로 쓰이는 DC 를 릴리즈한다.
void ZMain::_releaseBufferDC() {
  if ( m_hBufferDC != nullptr) {
    const BOOL bRet = DeleteDC(m_hBufferDC);
    if (bRet == FALSE) {
      assert(false);
    }
    m_hBufferDC = nullptr;
  }
}

void ZMain::onTimer() {
  if ( ZOption::GetInstance().is_slide_mode ) {/// check slidemode
    long long slideShowedTime = duration_cast<milliseconds>(system_clock::now() - ZOption::GetInstance().m_LastSlidedTime).count();

    if ( slideShowedTime > ZOption::GetInstance().m_iSlideModePeriodMiliSeconds ) {
      NextImage();
      Draw();
      ZOption::GetInstance().m_LastSlidedTime = system_clock::now();
    }
  }

  ShowCacheStatus();
}

/// ZViewer 전용 메시지 박스
int ZMain::ShowMessageBox(const TCHAR * msg, UINT button) {
  return ::MessageBox(main_window_handle_, msg, TEXT("ZViewer"), button);
}

void ZMain::SetHWND(const HWND hWnd) {
  main_window_handle_ = hWnd;
  InitOpenFileDialog();
}

void ZMain::InitOpenFileDialog() {
  // Initialize OPENFILENAME
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = main_window_handle_;
  ofn.lpstrFile = szFile;
  //
  // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
  // use the contents of szFile to initialize itself.
  //
  ofn.lpstrFile[0] = '\0';
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = ExtInfoManager::GetInstance().GetFileDlgFilter();

  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = m_strCurrentFolder.c_str();
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLEHOOK | OFN_EXPLORER;
  ofn.lpfnHook = (LPOFNHOOKPROC)CenterOFNHookProc;
}

void ZMain::OpenFileDialog() {
  // 현재 폴더를 세팅해 준다.
  ofn.lpstrInitialDir = m_strCurrentFolder.c_str();

  // Display the Open dialog box.
  if (GetOpenFileName(&ofn)==TRUE)
  {
    OpenFile(ofn.lpstrFile);
  }
}

/// 현재 파일을 다른 형식으로 저장하는 파일 다이얼로그를 연다.
void ZMain::SaveFileDialog() {
  if ( current_image_ == nullptr) {
    return;
  }
  CSaveAs::getInstance().setParentHWND(main_window_handle_);
  CSaveAs::getInstance().setDefaultSaveFilename(m_strCurrentFolder, m_strCurrentFilename);

  if ( true == CSaveAs::getInstance().showDialog() )
  {
    tstring strSaveFilename = CSaveAs::getInstance().getSaveFileName();

    if ( false == current_image_->SaveToFile(strSaveFilename, 0) )
    {
      ShowMessageBox(GetMessage(TEXT("CANNOT_SAVE_AS_FILE")));
      ShowMessageBox(GetMessage(TEXT("CANNOT_SAVE_AS_FILE_TRANS_ERROR")));
    }
  }
}

void ZMain::OnInit() {
  /// 불러온 최종 옵션을 점검하여 메뉴 중 체크표시할 것들을 표시한다.
  SetCheckMenus();

  CacheManager::GetInstance().set_listener(this);

  if ( m_strInitArg.empty() ) {
    RescanFolder();
  } else {
    // 시작 인자가 있으면 그 파일을 보여준다.
    OpenFile(m_strInitArg);
  }
}

/// 현재 보고 있는 파일의 Exif 정보를 보여준다.
void ZMain::ShowExif() {
  if ( current_image_ == nullptr ) {
    assert(current_image_);
    return;
  }
  ExifDlg aDlg;
  aDlg.Create(m_hMainInstance, main_window_handle_, SW_SHOW, TEXT("ExifDlg"), NULL);
  aDlg.MakeExifMap(*current_image_);
  aDlg.DoResource(main_window_handle_);
}

void ZMain::Draw(HDC toDrawDC, bool need_to_erase_background) {
  ElapseTime draw_time;

  HDC mainDC = NULL;

  if ( NULL == toDrawDC ) {
    mainDC = GetDC(m_hShowWindow);
    if (mainDC == NULL) {
      assert(false);
      return;
    }
  } else {
    /// 무효화된 부분만 그릴 때의 DC 를 얻어오기 위해
    mainDC = toDrawDC;
  }

  RECT currentScreenRect;
  if ( false == getCurrentScreenRect(currentScreenRect) ) {
    assert(false);
    return;
  }

  /// 파일이 하나도 없을 때는 배경만 지우고 바로 리턴한다.
  if ( filelist_.size() <= 0 || current_image_ == nullptr) {
    EraseBackground(mainDC, currentScreenRect.right, currentScreenRect.bottom);

    if ( NULL == toDrawDC ) {
      ReleaseDC(main_window_handle_, mainDC);
    }
    return;
  }

  DebugPrintf(TEXT("ZMain::Draw()"));
  if (current_image_->IsValid() == FALSE) {
    return;
  }
  if (main_window_handle_ == NULL) {
    return;
  }

  /// ShowWindow 부분만 그리도록 클리핑 영역을 설정한다.
  HRGN hrgn = CreateRectRgn(currentScreenRect.left, currentScreenRect.top, currentScreenRect.right, currentScreenRect.bottom);
  SelectClipRgn(mainDC, hrgn);

  const int zoomedImageWidth = (int)(current_image_->GetWidth() * m_fCurrentZoomRate);
  const int zoomedImageHeight = (int)(current_image_->GetHeight() * m_fCurrentZoomRate);

  /// 그릴 그림이 현재 화면보다, 가로 & 세로 모두 크면 배경을 지우지 않아도 됨.
  if ( zoomedImageWidth >= currentScreenRect.right && zoomedImageHeight >= currentScreenRect.bottom )
  {
    need_to_erase_background = false;
  }

  if ( zoomedImageWidth <= currentScreenRect.right ) {
    // 가로 길이가 화면보다 작을 때는 중앙에 오게
    m_iShowingX = -((currentScreenRect.right/2) - (zoomedImageWidth/2));
  }

  if ( zoomedImageHeight <= currentScreenRect.bottom ) {
    // 세로 길이가 화면보다 작을 때는 중앙에 오게
    m_iShowingY = -((currentScreenRect.bottom/2) - (zoomedImageHeight/2));
  }

  RECT drawRect = { -m_iShowingX, -m_iShowingY, zoomedImageWidth-m_iShowingX, zoomedImageHeight-m_iShowingY };

  if ( need_to_erase_background ) {// 배경을 지워야 하면 지운다. 그림이 그려지는 부분을 제외하고 지운다.
    HRGN hScreenRGN = CreateRectRgn(currentScreenRect.left, currentScreenRect.top, currentScreenRect.right, currentScreenRect.bottom);
    HRGN hDrawRGN = CreateRectRgn(drawRect.left, drawRect.top, drawRect.right, drawRect.bottom);
    CombineRgn(m_hEraseRgb, hScreenRGN, hDrawRGN, RGN_DIFF);

    FillRgn(mainDC, m_hEraseRgb, background_brush_);

    DeleteObject(hScreenRGN);
    DeleteObject(hDrawRGN);
  }

  /// 추후 확대/축소 후의 화면 위치등을 위해서
  m_fCenterX = (float)((currentScreenRect.right/2.0f) - (float)drawRect.left) / (float)(drawRect.right-drawRect.left);
  m_fCenterY = (float)((currentScreenRect.bottom/2.0f) - (float)drawRect.top) / (float)(drawRect.bottom-drawRect.top);

  current_image_->Draw(mainDC, drawRect);

  ReleaseDC(main_window_handle_, mainDC);

  SetMouseCursor(zoomedImageWidth, zoomedImageHeight, currentScreenRect);

  // 풀 스크린이 아닐 때는 상태 표시줄을 다시 그린다.
  if ( false == ZOption::GetInstance().IsFullScreen() ) {
    PostMessage(m_hStatusBar, WM_PAINT, 0, 0);
  }

  DeleteObject(hrgn);
  if ( NULL == toDrawDC ) {
    ReleaseDC(main_window_handle_, mainDC);
  }
  DebugPrintf(L"Draw time:%d", draw_time.End());
}

void ZMain::SetMouseCursor(const int image_width, const int image_height, const RECT& screen_rect) {
  if ( image_width > screen_rect.right || image_height > screen_rect.bottom ) {
    SetHandCursor(true);
  } else {
    SetHandCursor(false);
  }
  PostMessage(main_window_handle_, WM_SETCURSOR, 0, 0);
}

void ZMain::RescanFolder() {
  tstring strToFindFolder = m_strCurrentFolder;

  strToFindFolder += TEXT("*.*");

  ElapseTime folder_scan_time;
  GetSortedFileList(strToFindFolder, m_sortOrder, &filelist_);
  DebugPrintf(L"Folder scan time:%d", folder_scan_time.End());

  // Cache Thread 에 전달한다.
  CacheManager::GetInstance().SetFilelist(filelist_);

  if ( m_strCurrentFilename.empty() && !filelist_.empty())
  {
    m_strCurrentFilename = filelist_[0].m_strFileName;
    m_iCurretFileIndex = 0;

    LoadCurrent();
  }
}

bool ZMain::GetNeighborFolders(std::vector < tstring > & vFolders) {
  // 현재 폴더의 상위 폴더를 검색한다.
  {
    // 현재 폴더에서 오른쪽부터 2번째의 \ 를 얻는다.
    size_t pos = m_strCurrentFolder.find_last_of(TEXT("\\"));

    if ( pos == m_strCurrentFolder.npos ) {
      ShowMessageBox(GetMessage(TEXT("CANNOT_FIND_PARENT_DIRECTORY")));
      return false;
    }

    tstring strParentFolder = m_strCurrentFolder.substr(0, pos);

    pos = strParentFolder.find_last_of(TEXT("\\"));

    if ( pos == strParentFolder.npos ) {
      ShowMessageBox(GetMessage(TEXT("CANNOT_FIND_PARENT_DIRECTORY")));
      return false;
    }

    strParentFolder = strParentFolder.substr(0, pos);

    strParentFolder += TEXT("\\*.*");
    // 상위 폴더의 하위 폴더들을 얻는다.
    FindFolders(strParentFolder.c_str(), vFolders, false);

    if ( vFolders.size() <= 0 ) {
      assert(vFolders.size() > 0 );
      return false;
    }
  }

  sort(vFolders.begin(), vFolders.end(), CStringCompareIgnoreCase());

  return true;
}

void ZMain::NextFolder() {
  std::vector < tstring > vFolders;
  if (!GetNeighborFolders(vFolders)) {
    return;
  }

  // 상위 폴더의 하위 폴더 중 현재 폴더의 다음 폴더가 있으면 다음 폴더를 검색한 후, 처음 이미지를 연다.
  {
    int iFoundIndex = -1;
    // 현재 폴더의 index 를 찿는다.
    for (unsigned int i=0; i<vFolders.size(); ++i)
    {
      if ( vFolders[i] + TEXT("\\") == m_strCurrentFolder )
      {
        iFoundIndex = i;
        break;
      }
    }

    assert(iFoundIndex != -1);

    if ( (iFoundIndex + 1) >= (int)vFolders.size() ) {
      ShowMessageBox(GetMessage(TEXT("LAST_DIRECTORY")));
      return;
    } else {
      OpenFolder(vFolders[iFoundIndex+1]);
    }
  }
}


void ZMain::PrevFolder() {
  std::vector < tstring > vFolders;

  if ( !GetNeighborFolders(vFolders) ) return;

  // 상위 폴더의 하위 폴더 중 현재 폴더의 다음 폴더가 있으면 다음 폴더를 검색한 후, 처음 이미지를 연다.
  {
    int iFoundIndex = -1;
    // 현재 폴더의 index 를 찿는다.
    for (unsigned int i=0; i<vFolders.size(); ++i)
    {
      if ( vFolders[i] + TEXT("\\") == m_strCurrentFolder )
      {
        iFoundIndex = i;
        break;
      }
    }

    assert(iFoundIndex != -1);

    if ( (iFoundIndex-1 < 0 ) ) {
      // 마지막 폴더이다.
      ShowMessageBox(GetMessage(TEXT("FIRST_FOLDER")));
      return;
    } else {
      OpenFolder(vFolders[iFoundIndex-1]);
    }
  }
}

void ZMain::OpenFolder(const tstring & strFolder) {
  // 특정 폴더의 하위 파일들을 검색해서 정렬 후 첫번째 파일을 연다.
  tstring strTemp = strFolder;
  strTemp += TEXT("\\*.*");

  vector < FileData > vFiles;

  GetSortedFileList(strTemp, m_sortOrder, &vFiles);

  if ( vFiles.size() == 0 ) {
    TCHAR msg[COMMON_BUFFER_SIZE];
    SPrintf(msg, COMMON_BUFFER_SIZE, GetMessage(TEXT("THIS_DIRECTORY_HAS_NO_IMAGE")), strFolder.c_str());
    ::MessageBox(main_window_handle_, msg, TEXT("ZViewer"), MB_OK);
    return;
  } else {
    OpenFile(vFiles[0].m_strFileName);
  }
}

void ZMain::OpenFile(const tstring & strFilename) {
  current_image_ = nullptr;

  m_strCurrentFolder = GetFolderNameFromFullFileName(strFilename);
  RescanFolder();

  // 스캔한 파일 중 현재 파일을 찾는다.
  std::vector< FileData >::const_iterator it, endit = filelist_.end();

  for ( it = filelist_.begin(); it != endit; ++it) {
    if ( it->m_strFileName == strFilename ) {
      break;
    }
  }

  if ( filelist_.empty() ) {
    assert(!"size of scanned file list is 0. Check folder name or path!!");

    return;
  }

  assert(it != filelist_.end());	// 그 파일이 없을리가 없다.

  if ( it != filelist_.end() ) {
    m_iCurretFileIndex = (int)(it - filelist_.begin());//i;
    m_strCurrentFilename = strFilename;

    LoadCurrent();
  }
}


bool ZMain::MoveIndex(int iIndex) {
  if (filelist_.empty()) {
    return false;
  }

  iIndex = GetCalculatedMovedIndex(iIndex);

  if ( m_iCurretFileIndex == iIndex ) return false;

  m_iCurretFileIndex = iIndex;
  m_strCurrentFilename = filelist_[m_iCurretFileIndex].m_strFileName;
  current_image_ = nullptr;
  LoadCurrent();

  return true;
}

/// 새로 이동할 인덱스 번호를 주면 지금 상황에 맞는 인덱스 번호를 돌려준다.
int ZMain::GetCalculatedMovedIndex(int iIndex) {
  if ( iIndex < 0 ) {
    if ( ZOption::GetInstance().IsLoopImages() ) {
      iIndex = (int)(filelist_.size() - ((-1*iIndex) % filelist_.size()));
    } else {
      iIndex = 0;
    }
  }

  if ( iIndex >= (int)filelist_.size() ) {
    if ( ZOption::GetInstance().IsLoopImages() || ZOption::GetInstance().is_slide_mode ) {
      iIndex = (int)( iIndex % filelist_.size() );
    } else {
      iIndex = (int)filelist_.size() - 1;
    }
  }
  return iIndex;
}

/// 다음 이미지 파일로 이동
bool ZMain::NextImage() {
	CacheManager::GetInstance().set_view_direction(ViewDirection::kForward);
	return MoveRelateIndex(+1);
}

/// 이전 이미지 파일로 이동
bool ZMain::PrevImage() {
	CacheManager::GetInstance().set_view_direction(ViewDirection::kBackward);
	return MoveRelateIndex(-1);
}

bool ZMain::FirstImage() {
  // 현재의 위치를 History 에 저장해놓는다.
  m_history->push_lastImageIndex(m_iCurretFileIndex);
  return MoveIndex(0);
}

bool ZMain::LastImage() {
  // 현재의 위치를 History 에 저장해놓는다.
  m_history->push_lastImageIndex(m_iCurretFileIndex);
  return MoveIndex((int)filelist_.size() - 1);
}

void ZMain::OnChangeCurrentSize(int iWidth, int iHeight) {
  if (current_image_ == nullptr) {
    return;
  }

  if ( m_iShowingX + iWidth > current_image_->GetWidth() ) {
    m_iShowingX -= (m_iShowingX + iWidth - current_image_->GetWidth());

    if (m_iShowingX < 0) {
      m_iShowingX = 0;
    }
  }

  if ( m_iShowingY + iHeight > current_image_->GetHeight() ) {
    m_iShowingY -= (m_iShowingY + iHeight - current_image_->GetHeight());
    if (m_iShowingY < 0) {
      m_iShowingY = 0;
    }
  }
}

void ZMain::HideForm() {
  LONG style = GetWindowLong(main_window_handle_, GWL_STYLE);
  style &= ~WS_CAPTION;
  style &= ~WS_THICKFRAME;
  ShowWindow(m_hStatusBar, SW_HIDE);
  SetMenu(main_window_handle_, NULL);
  SetWindowLong(main_window_handle_, GWL_STYLE, style);
}

void ZMain::ShowForm() {
  LONG style = GetWindowLong(main_window_handle_, GWL_STYLE);
  style |= WS_CAPTION;
  style |= WS_THICKFRAME;
  ShowWindow(m_hStatusBar, SW_SHOW);
  SetMenu(main_window_handle_, m_hMainMenu);
  SetWindowLong(main_window_handle_, GWL_STYLE, style);
}

void ZMain::StartTimer() {
  /// Add Timer
  m_timerPtr = SetTimer(main_window_handle_, kTimerId, 100, NULL);

  if ( m_timerPtr == 0 ) {
    MessageBox(main_window_handle_, GetMessage(TEXT("CANNOT_MAKE_TIMER")), TEXT("ZViewer"), MB_OK);
    return;
  }
}

void ZMain::StopTimer() {
  if ( m_timerPtr != 0 ) {
    const BOOL bRet = KillTimer(main_window_handle_, kTimerId);

    if ( FALSE == bRet ) {
      assert(false);
    } else {
      m_timerPtr = 0;
    }
  }
}

/// Cache status 를 상태 표시줄에 표시한다.
void ZMain::ShowCacheStatus() {
  static bool bLastActionIsCache = false;

  const bool bNowActionIsCache = CacheManager::GetInstance().is_cacahing();

  if (bNowActionIsCache != bLastActionIsCache) {
    bLastActionIsCache = bNowActionIsCache;

    static tstring strStatusMsg=TEXT("...");	///< PostMessage() 로 호출하므로, 메모리가 없어지지 않게 하기 위해 static

    if ( filelist_.empty() ) {
      strStatusMsg = TEXT("");
    } else if ( false == bNowActionIsCache ) {///< 캐쉬가 끝났으면
      strStatusMsg = TEXT("Cached");
    } else {///< 아직 캐쉬 중이면
      if ( CacheManager::GetInstance().IsNextFileCached() ) {///< 다음 파일의 캐쉬가 끝났으면
        strStatusMsg = TEXT("Caching files");
      } else {///< 아직 다음 파일도 캐쉬가 안 끝났으면,
        strStatusMsg = TEXT("Caching next");
      }
    }
    PostMessage(m_hStatusBar, SB_SETTEXT,
      static_cast<WPARAM>(StatusBarPosition::kCacheInfo), (LPARAM)strStatusMsg.c_str());
  }
}

void ZMain::ToggleFullScreen() {
  if ( ZOption::GetInstance().IsFullScreen() ) {// 현재 풀스크린이면 원래 화면으로 돌아간다.
    ZOption::GetInstance().SetFullScreen(!ZOption::GetInstance().IsFullScreen());

    TaskBar::Show();	// 숨겨졌던 작업 표시줄을 보여준다.

    ShowForm();	// 메뉴, 상태 표시줄등을 보여준다.

    SetWindowPos(main_window_handle_, HWND_TOP, m_lastPosition.left, m_lastPosition.top, m_lastPosition.right - m_lastPosition.left, m_lastPosition.bottom - m_lastPosition.top, SWP_SHOWWINDOW);
  } else {// 현재 풀스크린이 아니면 풀스크린으로 만든다.
    ZOption::GetInstance().SetFullScreen(!ZOption::GetInstance().IsFullScreen());
    // Save current size
    GetWindowRect(main_window_handle_, &m_lastPosition);

    HideForm();// 메뉴, 상태 표시줄등을 숨긴다.

    const int screenWidth = ::GetSystemMetrics( SM_CXSCREEN );
    const int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    // 화면 듀얼 모니터 처리를 해줌. 처음 위치에 따라서 full screen 후 화면의 위치를 바꿔준다.(아래 소스는 듀얼 모니터이고, 양쪽의 해상도가 같고, 기본 모니터가 왼쪽의 모니터일 때만 적용된다)
    const int screenX = int(m_lastPosition.left / screenWidth) * screenWidth;
    const int screenY = int(m_lastPosition.top / screenHeight) * screenHeight;

    SetWindowPos(main_window_handle_, HWND_TOPMOST, screenX, screenY, screenWidth, screenHeight, SWP_NOMOVE|SWP_NOSIZE);
    MoveWindow(main_window_handle_, screenX, screenY, screenWidth, screenHeight, TRUE);

    // 작업 표시줄을 가려준다.
    TaskBar::Hide();

    // 포커스를 잃으면 원래대로 돌아가야하므로 풀어놓는다.
    SetWindowPos(main_window_handle_, HWND_NOTOPMOST, screenX, screenY, screenWidth, screenHeight, SWP_NOMOVE|SWP_NOSIZE);

  }

  RefreshCurrentImage();

  SetCheckMenus();

  m_iShowingX = 0;
  m_iShowingY = 0;
}

void ZMain::ToggleSmallToScreenStretch() {
  ZOption::GetInstance().ToggleSmallToBigStretchImage();
  RefreshCurrentImage();
  SetCheckMenus();
}

void ZMain::ToggleBigToScreenStretch() {
  ZOption::GetInstance().ToggleBigToSmallStretchImage();
  RefreshCurrentImage();
  SetCheckMenus();
}

void ZMain::ToggleLoopImage() {
  ZOption::GetInstance().ToggleLoopImages();

  SetCheckMenus();
}

void ZMain::SetStatusBarText() {
  TCHAR szTemp[COMMON_BUFFER_SIZE];

  // file index
  {
    if (filelist_.empty()) {
      // File Index
      SetStatusBarTextInternal(StatusBarPosition::kFileIndex, TEXT("No File list"));
    }
    else {
      SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%d/%u"), m_iCurretFileIndex+1, static_cast<unsigned int>(filelist_.size()));
      SetStatusBarTextInternal(StatusBarPosition::kFileIndex, szTemp);
    }
  }

  if ( current_image_ == nullptr || filelist_.size() == 0 || m_strCurrentFilename.empty() ) { // 보고 있는 파일이 없으면
    // Resolution
    SetStatusBarTextInternal(StatusBarPosition::kResolution, TEXT("0x0"));

    // Image File Size
    SetStatusBarTextInternal(StatusBarPosition::kImageFileSize, TEXT(""));

    // Zoom Ratio
    SetStatusBarTextInternal(StatusBarPosition::kZoomRatio, TEXT(""));

    // Homepage URL
    SetStatusBarTextInternal(StatusBarPosition::kHomepageURL, TEXT("http://wimy.com"));

    // Loading time
    SetStatusBarTextInternal(StatusBarPosition::kLoadingTime, TEXT(""));

    // CacheStatus
    if ( filelist_.size() == 0 ) {
      SetStatusBarTextInternal(StatusBarPosition::kCacheInfo, TEXT("No file list"));
    } else {
      ShowCacheStatus();
    }

    // Filename
    SetStatusBarTextInternal(StatusBarPosition::kFilename, TEXT("No File"));
  } else {
    // Image Resolution
    SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%dx%d %ubit color"),
      current_image_->GetOriginalWidth(), current_image_->GetOriginalHeight(), current_image_->GetBPP());
    SetStatusBarTextInternal(StatusBarPosition::kResolution, szTemp);

    // Image file size
    tstring filesize = ConvertFileSize(filelist_[m_iCurretFileIndex].m_nFileSize);
    SetStatusBarTextInternal(StatusBarPosition::kImageFileSize, filesize.c_str());

    // Image Zoom Ratio
    SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%d%%"), static_cast<int>(m_fCurrentZoomRate*100.0f));
    SetStatusBarTextInternal(StatusBarPosition::kZoomRatio, szTemp);

    // Homepage URL
    SetStatusBarTextInternal(StatusBarPosition::kHomepageURL, TEXT("http://wimy.com"));

    // 로딩시간
    SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%.3fsec"), static_cast<float>(m_dwLoadingTime / 1000.0));
    SetStatusBarTextInternal(StatusBarPosition::kLoadingTime, szTemp);

    // CacheStatus
    ShowCacheStatus();

    // Filename
    TCHAR szFilename[MAX_PATH], szFileExt[MAX_PATH];
    SplitPath(m_strCurrentFilename.c_str(), NULL,0, NULL,0, szFilename,MAX_PATH, szFileExt,MAX_PATH);
    SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%s%s"), szFilename, szFileExt);
    SetStatusBarTextInternal(StatusBarPosition::kFilename, szTemp);
  }
}

void ZMain::SetStatusBarTextInternal(const StatusBarPosition position, const TCHAR* text) {
  SendMessage(m_hStatusBar, SB_SETTEXT,
    static_cast<WPARAM>(position), (LPARAM)text);
}

void ZMain::SetTitle() {
  TCHAR szTemp[MAX_PATH+256];

  if ( m_strCurrentFilename.empty() ) {// 현재보고 있는 파일명이 없으면
    SPrintf(szTemp, MAX_PATH+256, TEXT("ZViewer v%s"), g_strVersion.c_str());
  } else {// 현재보고 있는 파일명이 있으면
    TCHAR szFileName[FILENAME_MAX] = { 0 };
    TCHAR szFileExt[MAX_PATH] = { 0 };
    SplitPath(m_strCurrentFilename.c_str(), NULL,0, NULL,0, szFileName,FILENAME_MAX, szFileExt,MAX_PATH);

    SPrintf(szTemp, MAX_PATH+256, TEXT("%s%s - %s"), szFileName, szFileExt, m_strCurrentFilename.c_str());
  }
  const BOOL bRet = SetWindowText(main_window_handle_, szTemp);

  if ( FALSE == bRet ) {
    assert(!"Can't SetWindowText");
  }
}

void ZMain::LoadCurrent() {
  /// 파일 목록이 하나도 없으면 로딩을 시도하지 않는다.
  if (filelist_.empty()) {
    return;
  }

  static bool bFirst = true;

  if ( bFirst ) {
    bFirst = false;

    CacheManager::GetInstance().SetFilelist(filelist_);
    CacheManager::GetInstance().StartCacheThread();
  }

  _releaseBufferDC();

  CacheManager::GetInstance().SetCurrent(m_iCurretFileIndex, m_strCurrentFilename);

  RefreshCurrentImage();
}

void ZMain::RefreshCurrentImage() {
  current_image_ = nullptr;
  CheckCurrentImage();
}

void ZMain::CheckCurrentImage() {
  if (current_image_ != nullptr) {
    return;
  }

  shared_ptr<ZImage> image = CacheManager::GetInstance().GetCachedData(m_strCurrentFilename);
  if ( image != nullptr) {
    CacheManager::GetInstance().IncreaseCacheHitCounter();

    //m_dwLoadingTime = duration_cast<milliseconds>(system_clock::now() - start).count();
    m_dwLoadingTime = 0;

    if (image != nullptr) {
      SetImageAndShow(image);
    }
  } else {
    CacheManager::GetInstance().IncreaseCacheMissCounter();

    Draw(); //< erase background
  }

  SetTitle();
  SetStatusBarText();
}

void ZMain::OnDrag(const int x, const int y) {
  if ( current_image_ == nullptr) {
    return;
  }

  //	DebugPrintf(TEXT("Drag (%d,%d)"), x, y);
  if ( !current_image_->IsValid()) return;

  RECT rt;
  getCurrentScreenRect(rt);

  const int iNowShowingX = m_iShowingX;
  const int iNowShowingY = m_iShowingY;

  const int iZoomedWidth = (int)(current_image_->GetWidth() * m_fCurrentZoomRate);
  const int iZoomedHeight = (int)(current_image_->GetHeight() * m_fCurrentZoomRate);

  /// 현재 그림이 화면보다 작거나 딱 맞으면 drag 를 하지 않는다.
  if ( iZoomedWidth <= rt.right && iZoomedHeight <= rt.bottom ) {
    return;
  }

  m_iShowingX += x;

  if ( m_iShowingX <= 0 )
  {
    m_iShowingX = 0;
  }
  if ( m_iShowingX + rt.right >= iZoomedWidth )
  {
    m_iShowingX = iZoomedWidth - rt.right;
  }

  m_iShowingY += y;
  if ( m_iShowingY <= 0 )
  {
    m_iShowingY = 0;
  }
  if ( m_iShowingY + rt.bottom >= iZoomedHeight )
  {
    m_iShowingY = iZoomedHeight - rt.bottom;
  }

  //DebugPrintf(TEXT("ShowingX(%d), ShowingY(%d)"), m_iShowingX, m_iShowingY);
  /// 보여주는 X 좌표나 Y 좌표 둘 중 하나라도 변경되었으면, 드래그된 것이므로 다시 그린다.
  if ( m_iShowingX != iNowShowingX || m_iShowingY != iNowShowingY )
  {
    /// 여기서 false 인 이유는, 드래그를 할 수 있으면, 배경을 다시 칠할 필요가 없기 때문이다.
    Draw(NULL, false);
  }
}


void ZMain::ChangeFileSort(eFileSortOrder sortOrder) {
  m_sortOrder = sortOrder;
  ReLoadFileList();
}

void ZMain::ReLoadFileList() {
  const tstring strFileName = m_strCurrentFilename;
  RescanFolder();

  OpenFile(strFileName);
}

/// 현재 파일이 지워졌을 때 후의 처리. 파일 삭제, 이동 후에 불리는 함수이다.
void ZMain::_ProcAfterRemoveThisFile() {
  current_image_ = nullptr;
  m_strCurrentFilename.resize(0);

  // is this the last file?
  if ( filelist_.size() <= 1 ) {
    m_iCurretFileIndex = 0;
    m_strCurrentFilename = TEXT("");

    filelist_.resize(0);

    // Cache Thread 에 전달한다.
    CacheManager::GetInstance().SetFilelist(filelist_);

    SetTitle();
    SetStatusBarText();
    Draw(NULL, true);
  } else {
    tstring strNextFilename;

    assert(filelist_.size() > 1);
    assert(m_iCurretFileIndex >= 0);

    if ( m_iCurretFileIndex == (int)(filelist_.size() - 1) ) {///< 마지막 파일이면 이전 파일이고,
      strNextFilename = filelist_[m_iCurretFileIndex-1].m_strFileName;
    } else {/// 마지막 파일이 아니면 다음 파일을 보여준다.
      strNextFilename = filelist_[m_iCurretFileIndex+1].m_strFileName;
    }
    ZMain::GetInstance().OpenFile(strNextFilename);
  }
  CacheManager::GetInstance().SetFilelist(filelist_);
}

void ZMain::OnFocusLose() {
  TaskBar::Show();
}

void ZMain::OnFocusGet() {
  if ( ZOption::GetInstance().IsFullScreen() ) {
    DebugPrintf(TEXT("OnFocusGet() at fullscreen"));

    const int screenWidth = ::GetSystemMetrics( SM_CXSCREEN );
    const int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    // 화면 듀얼 모니터 처리를 해줌.
    const int screenX = int(m_lastPosition.left / screenWidth) * screenWidth;
    const int screenY = int(m_lastPosition.top / screenHeight) * screenHeight;

    SetWindowPos(main_window_handle_, HWND_TOPMOST, screenX, screenY, screenWidth, screenHeight, SWP_NOMOVE|SWP_NOSIZE);
    MoveWindow(main_window_handle_, screenX, screenY, screenWidth, screenHeight, TRUE);
    SetWindowPos(main_window_handle_, HWND_NOTOPMOST, screenX, screenY, screenWidth, screenHeight, SWP_NOMOVE|SWP_NOSIZE);

    TaskBar::Hide();
  }
}

void ZMain::Undo() {
  if ( m_history->CanUndo() ) {
    const int iLast = m_history->Undo();

    // 혹시나 범위를 벗어나면 안됨
    if ( iLast < 0 || iLast >= (int)filelist_.size() ) {
      assert(!"Out of range...");
      return;
    }

    if ( filelist_.empty() ) return;

    m_iCurretFileIndex = iLast;
    m_strCurrentFilename = filelist_[m_iCurretFileIndex].m_strFileName;
    LoadCurrent();
  }
}

void ZMain::Redo() {
  if ( m_history->CanRedo() ) {
    m_history->Redo();
  }
}

void ZMain::OnRightTopFirstDraw() {
  ZOption::GetInstance().m_bRightTopFirstDraw = !ZOption::GetInstance().m_bRightTopFirstDraw;

  SetCheckMenus();
}

void ZMain::ShowFileExtDlg() {
  ZFileExtDlg::GetInstance().ShowDlg();
}

void ZMain::DeleteThisFile() {
  if ( m_strCurrentFilename.empty() || current_image_ == nullptr ) {
    return;
  }

  TCHAR szDeleteMsg[COMMON_BUFFER_SIZE];

  SPrintf(szDeleteMsg, COMMON_BUFFER_SIZE, GetMessage(TEXT("DELETE_THIS_FILE")), GetFileNameFromFullFileName(m_strCurrentFilename).c_str());
  const int iRet = ::MessageBox(main_window_handle_, szDeleteMsg, TEXT("ZViewer"), MB_YESNO);

  if ( iRet == IDYES ) {
    if ( 0 == _tunlink(m_strCurrentFilename.c_str()) ) {
      DebugPrintf(TEXT("-- %s deleted --"), m_strCurrentFilename.c_str());
      _ProcAfterRemoveThisFile();
    } else {
      ShowMessageBox(GetMessage(TEXT("CANNOT_DELETE_THIS_FILE")));
    }
  }
}

void ZMain::DeleteThisFileToRecycleBin() {
  if ( m_strCurrentFilename.empty() || current_image_ == nullptr ) {
    return;
  }

  SHFILEOPSTRUCT fo;
  fo.hwnd = main_window_handle_;
  fo.wFunc = FO_DELETE;

  TCHAR szFilename[_MAX_PATH];
  SPrintf(szFilename, _MAX_PATH, _T("%s"), m_strCurrentFilename.c_str());
  szFilename[m_strCurrentFilename.size()+1] = 0;

  fo.pFrom = szFilename;
  fo.pTo = NULL;
  fo.fFlags = FOF_ALLOWUNDO;

  if ( 0 == SHFileOperation(&fo)) {
    if ( fo.fAnyOperationsAborted == FALSE ) {
      DebugPrintf(TEXT("-- %s deleted --"), m_strCurrentFilename.c_str());
      _ProcAfterRemoveThisFile();
    }
  } else {
    ShowMessageBox(GetMessage(TEXT("CANNOT_DELETE_THIS_FILE")));
  }
}


void ZMain::MoveThisFile() {
  /// 현재 보고 있는 파일이 없으면 바로 리턴한다.
  if ( m_strCurrentFilename.empty() ) {
    return;
  }

  CSelectToFolderDlg aDlg(ZOption::GetInstance().GetLastMoveDirectory());

  if ( !aDlg.DoModal() ) {
    return;
  }

  tstring strFolder = aDlg.GetSelectedFolder();

  tstring filename = GetFileNameFromFullFileName(m_strCurrentFilename);
  tstring strToFileName = aDlg.GetSelectedFolder();

  if ( strToFileName.size() <= 2 ) {
    ShowMessageBox(GetMessage(TEXT("MOVE_DESTINATION_IS_TOO_SHORT")));
    return;
  }

  strToFileName += TEXT("\\");
  strToFileName += filename;

  // 옮겨갈 폴더에 같은 파일이 있는지 확인한다.
  if ( 0 != _taccess(aDlg.GetSelectedFolder().c_str(), 00) ) {
    ShowMessageBox(GetMessage(TEXT("WRONG_DIRECTORY_NAME")));
    return;
  }

  // 같은 파일이 존재하는지 확인한다.
  if ( 0 == _taccess(strToFileName.c_str(), 00) ) {
    // 이미 존재하면
    if ( IDNO == ShowMessageBox(GetMessage(TEXT("ASK_OVERWRITE_FILE")), MB_YESNO) ) {
      return;
    }
  }

  if ( FALSE == MoveFileEx(m_strCurrentFilename.c_str(), strToFileName.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING) ) {
    ShowMessageBox(GetMessage(TEXT("CANNOT_MOVE_FILE")));
  } else {
    ZOption::GetInstance().SetLastMoveDirectory(aDlg.GetSelectedFolder());
  }
  _ProcAfterRemoveThisFile();
}


/// 현재 파일을 복사한다.
void ZMain::CopyThisFile() {
  /// 현재 보고 있는 파일이 없으면 바로 리턴한다.
  if ( m_strCurrentFilename.empty() ) {
    return;
  }

  CSelectToFolderDlg aDlg(ZOption::GetInstance().GetLastCopyDirectory());

  if ( !aDlg.DoModal() ) {
    return;
  }

  tstring strFolder = aDlg.GetSelectedFolder();

  tstring filename = GetFileNameFromFullFileName(m_strCurrentFilename);
  tstring strToFileName = aDlg.GetSelectedFolder();

  if ( strToFileName.size() <= 2 ) {
    ShowMessageBox(GetMessage(TEXT("MOVE_DESTINATION_IS_TOO_SHORT")));
    return;
  }

  strToFileName += TEXT("\\");
  strToFileName += filename;

  // 옮겨갈 폴더에 같은 파일이 있는지 확인한다.
  if ( 0 != _taccess(aDlg.GetSelectedFolder().c_str(), 00) ) {
    ShowMessageBox(GetMessage(TEXT("WRONG_DIRECTORY_NAME")));
    return;
  }

  // 같은 파일이 존재하는지 확인한다.
  if ( 0 == _taccess(strToFileName.c_str(), 00) ) {
    // 이미 존재하면
    if ( IDNO == ShowMessageBox(GetMessage(TEXT("ASK_OVERWRITE_FILE")), MB_YESNO) ) {
      return;
    }
  }

  if ( FALSE == CopyFile(m_strCurrentFilename.c_str(), strToFileName.c_str(), FALSE) ) {
    ShowMessageBox(GetMessage(TEXT("CANNOT_COPY_FILE")));
  } else {
    ZOption::GetInstance().SetLastCopyDirectory(aDlg.GetSelectedFolder());
  }
}


void ZMain::Rotate(bool bClockWise) {
  if (current_image_ == nullptr) {
    assert(false);
    return;
  }

  if ( current_image_->IsValid() ) {
    if ( bClockWise ) {
      current_image_->Rotate(-90);
    } else {
      current_image_->Rotate(90);
    }

    m_iShowingX = 0;
    m_iShowingY = 0;

    _releaseBufferDC();

    Draw(NULL, true);
  }
}

void ZMain::DecreaseOpacity() {
  if ( m_alpha < 20 ) {
    m_alpha = 20;
  }
  m_alpha -= 10;

  SetWindowLong ( main_window_handle_, GWL_EXSTYLE, GetWindowLong(main_window_handle_, GWL_EXSTYLE) | WS_EX_LAYERED );

  SetLayeredWindowAttributes(main_window_handle_, 0, m_alpha, LWA_ALPHA);
}

void ZMain::IncreaseOpacity() {
  if ( m_alpha >= 245 ) {
    m_alpha = 245;
  }
  m_alpha += 10;

  SetWindowLong ( main_window_handle_, GWL_EXSTYLE, GetWindowLong(main_window_handle_, GWL_EXSTYLE) | WS_EX_LAYERED );
  SetLayeredWindowAttributes(main_window_handle_, 0, m_alpha, LWA_ALPHA);
}


void ZMain::SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle style) {
  if ( current_image_ == nullptr ) {
    assert(current_image_);
    return;
  }

  // 현재보고 있는 파일을 윈도우 폴더에 저장한다.
  std::wstring setting_directory = ZViewer::Path::GetZViewerSettingDirectory();
  ZViewer::Path::CreateDirectory(setting_directory);

  tstring strSaveFileName = setting_directory;
  strSaveFileName += TEXT("\\zviewer_background.bmp");

  if (current_image_->SaveToFile(strSaveFileName, BMP_DEFAULT) == false) {
    assert(false);
    return;
  }

  CDesktopWallPaper::SetDesktopWallPaper(strSaveFileName, style);
}


/// 현재 화면의 그릴 수 있는 영역의 크기를 받아온다.
bool ZMain::getCurrentScreenRect(RECT & rect) {
  if ( NULL == m_hShowWindow ) {
    assert(m_hShowWindow);
    return false;
  }

  if (FALSE == GetClientRect(m_hShowWindow, &rect)) {
    return false;
  }
  return true;
}

/// On Window is resized
void ZMain::OnWindowResized() {
  RECT rt;
  getCurrentScreenRect(rt);

  OnChangeCurrentSize(rt.right, rt.bottom);
}

/// 마우스 휠을 돌렸을 때. delta 는 돌린 방향과 돌린 정도, bControlPushed 는 Control 키가 눌려졌으면 true
void ZMain::OnMouseWheel(const short delta, bool bControlPushed) {
  if ( bControlPushed ) {
    if ( delta < 0 ) {
      ZMain::GetInstance().ZoomOut();
    } else {
      ZMain::GetInstance().ZoomIn();
    }
  } else {
    if ( delta < 0 ) {
      ZMain::GetInstance().NextImage();
    } else {
      ZMain::GetInstance().PrevImage();
    }
  }
}

void ZMain::OnPressedESCKey() {
  if (ZOption::GetInstance().IsFullScreen()) {// 현재 풀스크린이면 원래 화면으로 돌아간다.
    ToggleFullScreen();
  } else {
    CloseProgram();
  }
}

/// 상태 표시 윈도우를 만든다.
void ZMain::CreateStatusBar() {
  // StatusBar 를 생성한다.
  m_hStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, TEXT("No Image"), main_window_handle_, 0);

  // StatusBar 를 split 한다. 아래의 숫자는 크기가 아니라 절대 위치라는 것을 명심!!!!!!!
  static const int kStatusSplitCount = 8;

  const int SBPart[kStatusSplitCount] = {
    70,		///< %d/%d 현재보고 있는 이미지 파일의 index number
    200,	///< %dx%dx%dbpp 해상도와 color depth, image size
    300,	///< image size
    350,	///< zoom
    470,	///< temp banner http://www.wimy.com
    550,	///< 파일을 읽어들이는데 걸린 시간
    633,	///< cache status
    1910,	///< 파일명표시
  };
  SendMessage(m_hStatusBar, SB_SETPARTS, kStatusSplitCount, (LPARAM)SBPart);
}

/// 메뉴 중 체크표시 되는 것을 확인하여 설정해준다.
void ZMain::SetCheckMenus() {
  /// start of checking main menu
  CheckMenuItem(m_hMainMenu, ID_OPTION_VIEWLOOP, ZOption::GetInstance().IsLoopImages() ? MF_CHECKED : MF_UNCHECKED);
  CheckMenuItem(m_hMainMenu, ID_VIEW_FULLSCREEN, ZOption::GetInstance().IsFullScreen() ? MF_CHECKED : MF_UNCHECKED);
  CheckMenuItem(m_hMainMenu, ID_VIEW_SMALLTOSCREENSTRETCH, ZOption::GetInstance().IsSmallToBigStretchImage() ? MF_CHECKED : MF_UNCHECKED);
  CheckMenuItem(m_hMainMenu, ID_VIEW_BIGTOSCREENSTRETCH , ZOption::GetInstance().IsBigToSmallStretchImage() ? MF_CHECKED : MF_UNCHECKED);
  CheckMenuItem(m_hMainMenu, ID_VIEW_RIGHTTOPFIRSTDRAW, ZOption::GetInstance().m_bRightTopFirstDraw ? MF_CHECKED : MF_UNCHECKED);

  CheckMenuItem(m_hMainMenu, ID_AUTOROTATION, ZOption::GetInstance().IsUseAutoRotation() ? MF_CHECKED : MF_UNCHECKED);
  CheckMenuItem(m_hMainMenu, ID_VIEW_ALWAYSONTOP, ZOption::GetInstance().is_always_on_top_window_ ? MF_CHECKED : MF_UNCHECKED);
  /// end of checking main menu

  /// start of checking popup menu
  CheckMenuItem(m_hPopupMenu, ID_VIEW_RIGHTTOPFIRSTDRAW, ZOption::GetInstance().m_bRightTopFirstDraw ? MF_CHECKED : MF_UNCHECKED);
  CheckMenuItem(m_hPopupMenu, ID_VIEW_FULLSCREEN, ZOption::GetInstance().IsFullScreen() ? MF_CHECKED : MF_UNCHECKED);
  CheckMenuItem(m_hPopupMenu, ID_POPUPMENU_SMALLTOSCREENSTRETCH, ZOption::GetInstance().IsSmallToBigStretchImage() ? MF_CHECKED : MF_UNCHECKED);
  CheckMenuItem(m_hPopupMenu, ID_POPUPMENU_BIGTOSCREENSTRETCH, ZOption::GetInstance().IsBigToSmallStretchImage() ? MF_CHECKED : MF_UNCHECKED);
  /// end of checking popup menu
}

void ZMain::StartSlideMode() {
  ZOption::GetInstance().is_slide_mode = true;
  ZOption::GetInstance().m_LastSlidedTime = system_clock::now();
}

void ZMain::ToggleAutoRotation() {
  ZOption::GetInstance().SetAutoRotation(!ZOption::GetInstance().IsUseAutoRotation());
  SetCheckMenus();
}

void ZMain::ToggleAlwaysOnTop() {
  ZOption::GetInstance().is_always_on_top_window_ = !ZOption::GetInstance().is_always_on_top_window_;

  if ( ZOption::GetInstance().is_always_on_top_window_ ) {
    SetWindowPos(main_window_handle_, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
  } else {
    SetWindowPos(main_window_handle_, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

  }
  SetCheckMenus();
}

/// 배경을 지운다.
void ZMain::EraseBackground(HDC mainDC, LONG right, LONG bottom) {
  SelectObject(mainDC, background_brush_);
  Rectangle(mainDC, 0, 0, right, bottom);
}

/// Zoom in & out 전의 센터 위치로 그림을 드래그함. ZoomOut, ZoomIn 중에 호출됨
void ZMain::_PositionPreviousCenter() {
  if ( current_image_ == nullptr ) {
    assert(current_image_);
    return;
  }

  RECT current_screen_rect;
  getCurrentScreenRect(current_screen_rect);
  int moveX = (int)(current_image_->GetWidth() * m_fCurrentZoomRate * m_fCenterX - (current_screen_rect.right / 2));
  int moveY = (int)(current_image_->GetHeight() * m_fCurrentZoomRate * m_fCenterY - (current_screen_rect.bottom / 2));
  OnDrag(moveX, moveY);
}

void ZMain::ZoomIn() {
  const double original_zoom_rate = m_fCurrentZoomRate;

  if ( m_fCurrentZoomRate < 1.9999 ) {
    m_fCurrentZoomRate += 0.1;
  } else {
    m_fCurrentZoomRate += 1.0;
  }

  if ( m_fCurrentZoomRate >= 20.0 ) {
    m_fCurrentZoomRate = original_zoom_rate;
  }

  /// 실수 연산이 딱 떨어지지 않기 때문에 처리해준다.
  m_fCurrentZoomRate = ((int)((m_fCurrentZoomRate * 10.0)+0.5) / 10.0f);

  if ( original_zoom_rate != m_fCurrentZoomRate ) {
    m_iShowingX = 0;
    m_iShowingY = 0;

    SetStatusBarText();
    _PositionPreviousCenter(); ///< 이전 배율에서의 화면 위치로 드래그해서 Zoom 할 때 자연스럽게 보여준다.
    Draw();
  }
}

void ZMain::ZoomOut() {
  const double original_zoom_rate = m_fCurrentZoomRate;

  if ( m_fCurrentZoomRate <= 2.000001 ) {
    m_fCurrentZoomRate -= 0.1;
  } else {
    m_fCurrentZoomRate -= 1.0;
  }

  if ( m_fCurrentZoomRate < 0.1 ) {
    m_fCurrentZoomRate = original_zoom_rate;
  }

  /// 실수 연산이 딱 떨어지지 않기 때문에 처리해준다.
  m_fCurrentZoomRate = ((int)((m_fCurrentZoomRate * 10.0)+0.5) / 10.0f);

  if ( original_zoom_rate != m_fCurrentZoomRate ) {
    m_iShowingX = 0;
    m_iShowingY = 0;

    SetStatusBarText();
    _PositionPreviousCenter(); ///< 이전 배율에서의 화면 위치로 드래그해서 Zoom 할 때 자연스럽게 보여준다.
    Draw();
  }
}

void ZMain::ZoomNone() {
  if ( m_fCurrentZoomRate == 1.0 ) return;

  m_fCurrentZoomRate = 1.0;
  m_iShowingX = 0;
  m_iShowingY = 0;

  SetStatusBarText();
  Draw();
}

/// 현재보는 이미지를 클립보드에 복사한다.
void ZMain::CopyToClipboard() {
  if ( current_image_ == nullptr ) {
    assert(false);
    return;
  }

  if ( current_image_->IsValid() ) {
    current_image_->CopyToClipboard(main_window_handle_);
  } else {
    assert(false);
  }
}

void ZMain::CloseProgram(void) {
  SendMessage(main_window_handle_, WM_CLOSE, 0, 0);
}

/// 그림을 보여줄 윈도우를 만든다.
void ZMain::CreateShowWindow() {
  m_hShowWindow = CreateWindow(TEXT("static"), TEXT(""), WS_CHILD | WS_VISIBLE , 0,0,100,100,
  main_window_handle_, (HMENU)-1, ZResourceManager::GetInstance().GetHInstance(), NULL);

  AdjustShowWindowScreen();

  assert(m_hShowWindow != INVALID_HANDLE_VALUE);
}


/// 현재 크기에 맞는 ShowWindow 크기를 정한다.
void ZMain::AdjustShowWindowScreen() {
  RECT rect;
  GetClientRect(main_window_handle_, &rect);

  if ( false == ZOption::GetInstance().IsFullScreen() ) { /// 풀 스크린이 아닐 때는 작업표시줄의 영역만큼은 빼줘야함
    static const int kStatusbarHeight = 20;
    rect.bottom -= kStatusbarHeight;
  }
  SetWindowPos(m_hShowWindow, HWND_TOP, 0, 0, rect.right, rect.bottom, SWP_NOMOVE);
}

void ZMain::SendMsg(UINT Msg, WPARAM wParam, LPARAM lParam) {
  SendMessage(main_window_handle_, Msg, wParam, lParam);
}


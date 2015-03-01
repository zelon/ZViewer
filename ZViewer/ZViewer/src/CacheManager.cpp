#include "stdafx.h"

#include "CacheManager.h"

#include "CacheManagerDetail/CachedData.h"
#include "CacheManagerDetail/FileReader.h"
#include "ZOption.h"
#include "ZMain.h"
#include "../../commonSrc/MessageManager.h"

using namespace std;
using namespace std::chrono;

class CacheManager::Impl {
public:
  std::vector<BYTE> file_read_buffer_;
  CachedData m_cacheData;
  CEventObj m_hCacheEvent;
};

CacheManager & CacheManager::GetInstance() {
  static CacheManager ins;
  return ins;
}

CacheManager::CacheManager()
: m_bCacheGoOn(true)
, m_iLogCacheHit(0)
, m_iLogCacheMiss(0)
, m_bNowCaching(false)
, view_direction_(ViewDirection::kForward)
, m_pImpl(new Impl) {
}

CacheManager::~CacheManager() {
  CleanUpThread();
}

void CacheManager::CleanUpCache() {
  m_pImpl->m_cacheData.ClearCachedImageData();
}

void CacheManager::CleanUpThread() {
  m_bCacheGoOn = false;

  m_pImpl->m_hCacheEvent.setEvent();

  if ( cache_thread_.joinable() ) {
    DebugPrintf(TEXT("Waiting for cache thread end"));
    cache_thread_.join();
    DebugPrintf(TEXT("Completed waiting for cache thread end"));
  }
}

void CacheManager::SetImageVector(const std::vector <FileData> & filedata_list) {
  m_pImpl->m_cacheData.SetImageVector(filedata_list);
}

void CacheManager::StartCacheThread() {
  std::thread new_thread([]() {
    CacheManager::GetInstance().ThreadFunc();
    DebugPrintf(TEXT("End of Cache Thread"));
  });
  cache_thread_.swap(new_thread);

  // Cache 를 진행하는 쓰레드는
  if ( SetThreadPriority(cache_thread_.native_handle(), THREAD_PRIORITY_BELOW_NORMAL) == FALSE ) {
    assert(!"Can't SetThreadPriority!");
  }
}

void CacheManager::ShowCachedImageList() {
#ifndef _DEBUG
  return; // 릴리즈 모드에서는 그냥 리턴
#else
  m_pImpl->m_cacheData.PrintCachedData();
#endif
}

size_t CacheManager::GetNumOfCacheImage() const {
  return m_pImpl->m_cacheData.Size();
}


bool CacheManager::_CacheIndex(int iIndex) {
  /*
  // 최대 캐시 크기를 넘었으면 더 이상 캐시하지 않는다.
  if ( m_iCurrentIndex != iIndex &&
    ((m_lCacheSize / 1024 / 1024) > ZOption::GetInstance().GetMaxCacheMemoryMB()) )
  {
    return false;
  }
  */

  if ( iIndex < 0 )///< 캐시 매니저에서 현재 파일의 뒤를 캐시하려고 시도하기 때문에 이 if 문에 들어오게 된다. assert 상황이 아니다.
  {
    iIndex = 0;
  }
  if (iIndex >= (int)m_pImpl->m_cacheData.GetImageVectorSize())///< 캐시 매니저에서 현재 파일의 앞을 캐시하려고 시도하기 때문에 이 if 문에 들어오게 된다. assert 상황이 아니다.
  {
    iIndex = (int)(m_pImpl->m_cacheData.GetImageVectorSize()) - 1;
  }

  /// 이 상황은 현재 파일 목록이 하나도 없는 상황이다.
  if ( iIndex < 0 ) return false;

  // 이미 캐시되어 있는지 찾는다.
  bool bCacheFound = false;
  tstring strFileName;

  strFileName = m_pImpl->m_cacheData.GetFilenameFromIndex(iIndex);;//m_imageIndex2FilenameMap[iIndex];

  if ( strFileName.size() <= 0 ) return false;

  if ( strFileName.length() <= 0 ) return false;

  if (m_pImpl->m_cacheData.HasCachedDataByFilename(strFileName)) {
    // found!
    bCacheFound = true;
  }

  if (bCacheFound == true) {
    return true;
  }

  std::shared_ptr<ZImage> cache_image(make_shared<ZImage>());

  m_pImpl->file_read_buffer_.resize(0);
  FileReader file(strFileName);

  bool bLoadOK = false;

  if (file.IsOpened() == false) {
    assert(false);
    bLoadOK = false;
  } else {
    static const size_t kReadBufferSize = 2048;
    BYTE readBuffer[kReadBufferSize];

    DWORD dwReadBytes;
    BOOL bReadOK = TRUE;

    system_clock::time_point startTime = system_clock::now();

    while ( bReadOK ) {
      if ( m_bNewChange && m_iCurrentIndex != iIndex ) {///< 새로운 그림으로 넘어갔는데 현재 인덱스가 아니면
        DebugPrintf(TEXT("---------------------------- stop readfile"));
        return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
      }
      bReadOK = file.Read(readBuffer, kReadBufferSize, &dwReadBytes);
      if ( m_bNewChange && m_iCurrentIndex != iIndex ) {///< 새로운 그림으로 넘어갔는데 현재 인덱스가 아니면
        DebugPrintf(TEXT("---------------------------- stop readfile"));
        return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
      }
      if ( FALSE == bReadOK ) {
        assert(false);
        bLoadOK = false;
        break;
      } else {
        if ( dwReadBytes <= 0 ) {///< 파일의 끝까지 읽었다.
          break;
        } else {
          m_pImpl->file_read_buffer_.resize(m_pImpl->file_read_buffer_.size() + dwReadBytes);

          memcpy((&(m_pImpl->file_read_buffer_[0])) + m_pImpl->file_read_buffer_.size() - dwReadBytes, readBuffer, dwReadBytes);
        }
      }
    }

    const long long file_read_duration = duration_cast<milliseconds>(system_clock::now() - startTime).count();
    DebugPrintf(TEXT("----- readfile(%s) time(%d)"), strFileName.c_str(), file_read_duration);

    static const size_t kMinimumFileSize = 5;
    if (m_pImpl->file_read_buffer_.size() < kMinimumFileSize) {
      /// Too small image file
      bLoadOK = false;
    } else {
      /// todo: 아래 내용을 멤버 변수로 바꾸면 더 좋아질까
      fipMemoryIO mem(&m_pImpl->file_read_buffer_[0], (DWORD)m_pImpl->file_read_buffer_.size());

      DebugPrintf(TEXT("----- start decode(%s)"), strFileName.c_str());
      TIMECHECK_START("decode time");
      bLoadOK = cache_image->LoadFromMemory(mem);
      DebugPrintf(TEXT("----- end of decode(%s)"), strFileName.c_str());
      TIMECHECK_END();

      /// 옵션에 따라 자동 회전을 시킨다.
      if ( ZOption::GetInstance().IsUseAutoRotation() ) {
        cache_image->AutoRotate();
      }
    }
  }

  if ( bLoadOK == false ) {
    assert(!"Can't load image");

    tstring strErrorFilename = GetProgramFolder();
    strErrorFilename += TEXT("LoadError.png");
    if ( cache_image->LoadFromFile(strErrorFilename) ) {
      bLoadOK = true; ///< error image 라도 load ok
    } else {
      MessageBox(HWND_DESKTOP, TEXT("Please check LoadError.png in ZViewer installed folder"), TEXT("ZViewer"), MB_OK);
      const HBITMAP hBitmap = CreateBitmap(100, 100, 1, 1, NULL);
      cache_image->CopyFromBitmap(hBitmap);
      return true;
    }
  }

  if ( iIndex == m_iCurrentIndex ) {///< 현재 보는 이미지면 무조건 넣는다.
    AddCacheData(strFileName, cache_image, true);
    return true;
  }
  const long CachedImageSize = cache_image->GetImageSize();

  // 읽은 이미지를 넣을 공간이 없으면
  if ((m_pImpl->m_cacheData.GetCachedTotalSize() + CachedImageSize) / 1024 / 1024 > ZOption::GetInstance().GetMaxCacheMemoryMB()) {
    static const int kPreventInfiniteLoopCount = 100;
    for (int i = 0; i < kPreventInfiniteLoopCount; ++i) {
      // 가장 멀리있는 이미지의 캐시를 clear 해서 캐시 용량을 확보해본다.
      if (ClearFarthestCache(iIndex) == false) {
        return false;
      }
      // 이제 어느 정도 용량을 확보했으니 다시 이 이미지를 넣을 수 있는 지 캐시를 체크한다.
      if ((m_pImpl->m_cacheData.GetCachedTotalSize() + CachedImageSize) / 1024 / 1024 <= ZOption::GetInstance().GetMaxCacheMemoryMB()) {
        AddCacheData(strFileName, cache_image);
        return true;
      } else {
        DebugPrintf(TEXT("There is no vacant space"));
      }
      assert(i <= 99);
    }
  } else {
    AddCacheData(strFileName, cache_image);
    if ( m_bNewChange) return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
  }
  return true;
}

bool CacheManager::ClearFarthestCache(const int index) {
  // 캐시되어 있는 것들 중 가장 현재 index 에서 먼것을 찾는다.
  const int iFarthestIndex = m_pImpl->m_cacheData.GetFarthestIndexFromCurrentIndex(m_iCurrentIndex);
  assert(iFarthestIndex >= 0 );

  const size_t nCachedFarthestDiff = abs(iFarthestIndex - m_iCurrentIndex);
  const size_t nToCacheDiff = abs(index - m_iCurrentIndex );

  if ( nCachedFarthestDiff < nToCacheDiff ) {
    // 캐시 했는 것 중 가장 멀리있는 것이 이번거보다 가까운데 있으면 더이상 캐시하지 않는다
    return false;
  } else if ( nCachedFarthestDiff == nToCacheDiff ) {
    // 캐시했는 거랑 이번에 캐시할 것이 동등한 위치에 있으면

    if ( view_direction_ == ViewDirection::kForward ) {
      // 앞으로 진행 중이면 가장 멀리있는 것이 prev 이면 지운다(앞으로 가고 있을 때는 next image 가 우선순위가 높다)
      if ( iFarthestIndex >= index ) {
        // 캐시되어 있는 것을 비우지 않는다.
        return false;
      }
    } else {
      // 뒤로 진행 중이면 가장 멀리있는 것이 next 이면 지운다.
      if ( iFarthestIndex <= index ) {
        return false;
      }
    }
  }

  //  현재 것이 더 가깝기 때문에 가장 먼 것을 클리어하고, 현재 것을 캐시해 놓는 것이 좋은 상황이다.

  // 가장 먼 것을 clear 한다.
  m_pImpl->m_cacheData.ClearFarthestDataFromCurrent(iFarthestIndex);
  return true;
}

void CacheManager::ThreadFunc() {
  m_bNewChange = false;
  while ( m_bCacheGoOn ) // thread loop
  {
    m_bNowCaching = true;
    int iPos = 0;
    assert((int)m_pImpl->m_cacheData.GetImageVectorSize() == (int)m_pImpl->m_cacheData.GetIndex2FilenameMapSize());
    assert(m_iCurrentIndex <= (int)m_pImpl->m_cacheData.GetIndex2FilenameMapSize());
    assert(m_iCurrentIndex <= (int)m_pImpl->m_cacheData.GetImageVectorSize());

#ifdef _DEBUG
    if (m_pImpl->m_cacheData.IsEmpty())
    {
      assert(m_pImpl->m_cacheData.GetCachedTotalSize() == 0);
    }
#endif

    for ( int i=0; i<ZOption::GetInstance().m_iMaxCacheImageNum/2; ++i) {
      if ( false == m_bCacheGoOn ) break; ///< 프로그램이 종료되었으면 for 를 끝낸다.
      if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

      /// 항상 현재 이미지를 먼저 캐쉬한다.
      _CacheIndex(m_iCurrentIndex);
      if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

      /// 현재보고 있는 방향에 따라서 어디쪽 이미지를 먼저 캐시할 것인지 판단한다.

      if ( view_direction_ == ViewDirection::kForward ) {
        // right side
        if ( false == _CacheIndex(m_iCurrentIndex + iPos) ) break;
        if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

        // left side
        if ( false == _CacheIndex(m_iCurrentIndex - iPos) ) break;
        if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
      } else if ( view_direction_ == ViewDirection::kBackward ) {
        // left side
        if ( false == _CacheIndex(m_iCurrentIndex - iPos) ) break;
        if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

        // right side
        if ( false == _CacheIndex(m_iCurrentIndex + iPos) ) break;
        if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
      } else {
        assert(false);
      }

      ++iPos;
    }

    //DebugPrintf("wait event");

    m_bNowCaching = false;
    m_pImpl->m_hCacheEvent.wait();
    m_bNewChange = false;

    DebugPrintf(TEXT("Recv event"));
  }
}

bool CacheManager::hasCachedData(const tstring & strFilename, int iIndex) {
  // index 를 체크한다.
  m_iCurrentIndex = iIndex;
  m_strCurrentFileName = strFilename;

  m_bNewChange = true;

  m_pImpl->m_hCacheEvent.setEvent();

  if (m_pImpl->m_cacheData.HasCachedDataByFilename(strFilename)) {
    return true;
  }
  return false;
}

std::shared_ptr<ZImage> CacheManager::GetCachedData(const tstring & strFilename) const {
  return m_pImpl->m_cacheData.GetCachedData(strFilename);
}

void CacheManager::AddCacheData(const tstring & strFilename, std::shared_ptr<ZImage> pImage, const bool bForceCache) {
  if ( false == pImage->IsValid() ) {
    assert(false);
    return;
  }

  if (false == m_bCacheGoOn) {
    return;
  }

  m_pImpl->m_cacheData.InsertData(strFilename, pImage, bForceCache);
}

/// 다음 파일이 캐쉬되었나를 체크해서 돌려준다.
bool CacheManager::IsNextFileCached() const {
  int iNextIndex = m_iCurrentIndex;

  if ( view_direction_ == ViewDirection::kForward ) {
    ++iNextIndex;
  } else {
    --iNextIndex;
  }

  iNextIndex = ZMain::GetInstance().GetCalculatedMovedIndex(iNextIndex);

  return m_pImpl->m_cacheData.HasCachedDataByIndex(iNextIndex);
}

void CacheManager::WaitCacheLock() {
  m_pImpl->m_cacheData.WaitCacheLock();
}

/// 현재 캐쉬 정보를 디버그 콘솔에 보여준다. 디버깅모드 전용
void CacheManager::debugShowCacheInfo() {
  RECT rt;
  if ( false == ZMain::GetInstance().getCurrentScreenRect(rt) )
  {
    DebugPrintf(TEXT("CurrentScreenSize : Cannot getCurrentScreenRect"));
    return;
  }
  DebugPrintf(TEXT("CurrentScreenSize : %d, %d"), rt.right, rt.bottom);

  m_pImpl->m_cacheData.ShowCacheInfo();
}


void CacheManager::clearCache() {
  m_pImpl->m_cacheData.ClearCachedImageData();
  DebugPrintf(TEXT("Clear cache data"));
}

void CacheManager::setCacheEvent() {
  m_pImpl->m_hCacheEvent.setEvent();
}


long CacheManager::GetCachedKByte() const {
  return (m_pImpl->m_cacheData.GetCachedTotalSize() / 1024);
}

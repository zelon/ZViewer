#include "stdafx.h"
#include "CacheManager.h"

#include <atomic>

#include "CacheManagerDetail/CachedData.h"
#include "CacheManagerDetail/FileReader.h"
#include "ZOption.h"
#include "ZMain.h"
#include "../../commonSrc/MessageManager.h"
#include "../../commonSrc/ElapseTime.h"

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
  , cache_hit_counter_(0)
  , cache_miss_counter_(0)
  , is_caching_(false)
  , view_direction_(ViewDirection::kForward)
  , impl_(new Impl) {
}

CacheManager::~CacheManager() {
  CleanUpThread();
}

void CacheManager::CleanUpCache() {
  impl_->m_cacheData.ClearCachedImageData();
}

void CacheManager::CleanUpThread() {
  m_bCacheGoOn = false;
  listener_ = nullptr;

  impl_->m_hCacheEvent.setEvent();

  if (cache_thread_.joinable()) {
    DebugPrintf(TEXT("Waiting for cache thread end"));
    cache_thread_.join();
    DebugPrintf(TEXT("Completed waiting for cache thread end"));
  }
}

void CacheManager::SetFilelist(const std::vector <FileData>& filedata_list) {
  impl_->m_cacheData.SetFilelist(filedata_list);
}

void CacheManager::StartCacheThread() {
  std::thread new_thread([]() {
    CacheManager::GetInstance().ThreadFunc();
    DebugPrintf(TEXT("End of Cache Thread"));
  });
  cache_thread_.swap(new_thread);

  // Cache 를 진행하는 쓰레드는
  if (SetThreadPriority(cache_thread_.native_handle(), THREAD_PRIORITY_BELOW_NORMAL) == FALSE) {
    assert(!"Can't SetThreadPriority!");
  }
}

void CacheManager::ShowCachedImageList() {
#ifndef _DEBUG
  return; // 릴리즈 모드에서는 그냥 리턴
#else
  impl_->m_cacheData.PrintCachedData();
#endif
}

size_t CacheManager::GetNumOfCacheImage() const {
  return impl_->m_cacheData.GetCachedCount();
}

bool CacheManager::CacheImageByIndex(int index) {
  /*
  // 최대 캐시 크기를 넘었으면 더 이상 캐시하지 않는다.
  if ( current_index_ != index &&
  ((m_lCacheSize / 1024 / 1024) > ZOption::GetInstance().GetMaxCacheMemoryMB()) )
  {
  return false;
  }
  */

  if (index < 0) {///< 캐시 매니저에서 현재 파일의 뒤를 캐시하려고 시도하기 때문에 이 if 문에 들어오게 된다. assert 상황이 아니다.
    index = 0;
  }

  if (index >= (int)impl_->m_cacheData.GetFilelistSize()) {///< 캐시 매니저에서 현재 파일의 앞을 캐시하려고 시도하기 때문에 이 if 문에 들어오게 된다. assert 상황이 아니다.
    index = (int)(impl_->m_cacheData.GetFilelistSize()) - 1;
    if (index < 0) {
      // filelist is empty
      return false;
    }
  }

  const tstring filename = impl_->m_cacheData.GetFilenameFromIndex(index);
  if (filename.empty()) {
    return false;
  }

  // check already cached
  if (impl_->m_cacheData.HasCachedDataByFilename(filename)) {
    return true;
  }

  std::shared_ptr<ZImage> cache_image(make_shared<ZImage>());

  impl_->file_read_buffer_.resize(0);
  FileReader file(filename);

  bool bLoadOK = false;

  if (file.IsOpened() == false) {
    assert(false);
    bLoadOK = false;
  } else {
    static const size_t kReadBufferSize = 2048;
    BYTE readBuffer[kReadBufferSize];
    BOOL bReadOK = TRUE;
    ElapseTime file_io_time;

    while (bReadOK) {
      if (is_new_change_ && current_index_ != index) {///< 새로운 그림으로 넘어갔는데 현재 인덱스가 아니면
        DebugPrintf(TEXT("---------------------------- stop readfile"));
        return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
      }
      DWORD dwReadBytes = 0;
      bReadOK = file.Read(readBuffer, kReadBufferSize, &dwReadBytes);
      if (is_new_change_ && current_index_ != index) {///< 새로운 그림으로 넘어갔는데 현재 인덱스가 아니면
        DebugPrintf(TEXT("---------------------------- stop readfile"));
        return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
      }
      if (FALSE == bReadOK) {
        assert(false);
        bLoadOK = false;
        break;
      } else {
        if (dwReadBytes <= 0) {///< 파일의 끝까지 읽었다.
          break;
        } else {
          impl_->file_read_buffer_.resize(impl_->file_read_buffer_.size() + dwReadBytes);

          memcpy((&(impl_->file_read_buffer_[0])) + impl_->file_read_buffer_.size() - dwReadBytes, readBuffer, dwReadBytes);
        }
      }
    }

    const tstring only_filename = GetFileNameFromFullFileName(filename);
    DebugPrintf(TEXT("fileio(%s) time:%d"), only_filename.c_str(), file_io_time.End());

    static const size_t kMinimumFileSize = 5;
    if (impl_->file_read_buffer_.size() < kMinimumFileSize) {
      /// Too small image file
      bLoadOK = false;
    }
    else {
      /// todo: 아래 내용을 멤버 변수로 바꾸면 더 좋아질까
      fipMemoryIO mem(&impl_->file_read_buffer_[0], (DWORD)impl_->file_read_buffer_.size());

      ElapseTime decode_elapse;
      bLoadOK = cache_image->LoadFromMemory(mem);
      DebugPrintf(TEXT("decode(%s) time:%d"), only_filename.c_str(), decode_elapse.End());

      /// 옵션에 따라 자동 회전을 시킨다.
      if (ZOption::GetInstance().IsUseAutoRotation()) {
        ElapseTime rotate_elapse;
        cache_image->AutoRotate();
        DebugPrintf(TEXT("rotate(%s) time:%d"), only_filename.c_str(), rotate_elapse.End());
      }
    }
  }

  if (bLoadOK == false) {
    assert(!"Can't load image");

    tstring strErrorFilename = GetProgramFolder();
    strErrorFilename += TEXT("LoadError.png");
    if (cache_image->LoadFromFile(strErrorFilename)) {
      bLoadOK = true; ///< error image 라도 load ok
    } else {
      MessageBox(HWND_DESKTOP, TEXT("Please check LoadError.png in ZViewer installed folder"), TEXT("ZViewer"), MB_OK);
      const HBITMAP hBitmap = CreateBitmap(100, 100, 1, 1, NULL);
      cache_image->CopyFromBitmap(hBitmap);
      return true;
    }
  }

  if (index == current_index_) {///< 현재 보는 이미지면 무조건 넣는다.
    AddCacheData(filename, cache_image, /*force=*/true);
    return true;
  }
  const long CachedImageSize = cache_image->GetImageSize();

  // 읽은 이미지를 넣을 공간이 없으면
  if ((impl_->m_cacheData.GetCachedTotalBytes() + CachedImageSize) / 1024 / 1024 > ZOption::GetInstance().GetMaxCacheMemoryMB()) {
    static const int kPreventInfiniteLoopCount = 100;
    for (int i = 0; i < kPreventInfiniteLoopCount; ++i) {
      // 가장 멀리있는 이미지의 캐시를 clear 해서 캐시 용량을 확보해본다.
      if (ClearFarthestCache(index) == false) {
        return false;
      }
      // 이제 어느 정도 용량을 확보했으니 다시 이 이미지를 넣을 수 있는 지 캐시를 체크한다.
      if ((impl_->m_cacheData.GetCachedTotalBytes() + CachedImageSize) / 1024 / 1024 <= ZOption::GetInstance().GetMaxCacheMemoryMB()) {
        AddCacheData(filename, cache_image);
        return true;
      }
      else {
        DebugPrintf(TEXT("There is no vacant space"));
      }
      assert(i <= 99);
    }
  }
  else {
    AddCacheData(filename, cache_image);
    if (is_new_change_) return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
  }
  return true;
}

bool CacheManager::ClearFarthestCache(const int index) {
  // 캐시되어 있는 것들 중 가장 현재 index 에서 먼것을 찾는다.
  const int iFarthestIndex = impl_->m_cacheData.GetFarthestIndexFromCurrentIndex(current_index_);
  assert(iFarthestIndex >= 0);

  const size_t nCachedFarthestDiff = abs(iFarthestIndex - current_index_);
  const size_t nToCacheDiff = abs(index - current_index_);

  if (nCachedFarthestDiff < nToCacheDiff) {
    // 캐시 했는 것 중 가장 멀리있는 것이 이번거보다 가까운데 있으면 더이상 캐시하지 않는다
    return false;
  }
  else if (nCachedFarthestDiff == nToCacheDiff) {
    // 캐시했는 거랑 이번에 캐시할 것이 동등한 위치에 있으면

    if (view_direction_ == ViewDirection::kForward) {
      // 앞으로 진행 중이면 가장 멀리있는 것이 prev 이면 지운다(앞으로 가고 있을 때는 next image 가 우선순위가 높다)
      if (iFarthestIndex >= index) {
        // 캐시되어 있는 것을 비우지 않는다.
        return false;
      }
    }
    else {
      // 뒤로 진행 중이면 가장 멀리있는 것이 next 이면 지운다.
      if (iFarthestIndex <= index) {
        return false;
      }
    }
  }

  //  현재 것이 더 가깝기 때문에 가장 먼 것을 클리어하고, 현재 것을 캐시해 놓는 것이 좋은 상황이다.

  // 가장 먼 것을 clear 한다.
  impl_->m_cacheData.ClearFarthestDataFromCurrent(iFarthestIndex);
  return true;
}

void CacheManager::ThreadFunc() {
  is_new_change_ = false;
  while (m_bCacheGoOn) {// thread loop
    is_caching_ = true;
    int index_offset = 0;

#ifdef _DEBUG
    if (impl_->m_cacheData.IsEmpty()) {
      assert(impl_->m_cacheData.GetCachedTotalBytes() == 0);
    }
#endif

    for (int i = 0; i < ZOption::GetInstance().m_iMaxCacheImageNum / 2; ++i) {
      if (false == m_bCacheGoOn) break; ///< 프로그램이 종료되었으면 for 를 끝낸다.
      if (is_new_change_) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

      /// 항상 현재 이미지를 먼저 캐쉬한다.
      CacheImageByIndex(current_index_);
      if (is_new_change_) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

      /// 현재보고 있는 방향에 따라서 어디쪽 이미지를 먼저 캐시할 것인지 판단한다.

      if (view_direction_ == ViewDirection::kForward) {
        // right side
        if (false == CacheImageByIndex(current_index_ + index_offset)) break;
        if (is_new_change_) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

        // left side
        if (false == CacheImageByIndex(current_index_ - index_offset)) break;
        if (is_new_change_) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
      }
      else if (view_direction_ == ViewDirection::kBackward) {
        // left side
        if (false == CacheImageByIndex(current_index_ - index_offset)) break;
        if (is_new_change_) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

        // right side
        if (false == CacheImageByIndex(current_index_ + index_offset)) break;
        if (is_new_change_) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
      }
      else {
        assert(false);
      }

      ++index_offset;
    }

    //DebugPrintf("wait event");

    is_caching_ = false;
    impl_->m_hCacheEvent.wait();
    is_new_change_ = false;

    DebugPrintf(TEXT("Recv event"));
  }
}

void CacheManager::SetCurrent(const int index, const tstring & strFilename) {
  current_index_ = index;
  current_filename_ = strFilename;

  is_new_change_ = true;

  impl_->m_hCacheEvent.setEvent();
}

bool CacheManager::HasCachedData(const tstring & strFilename) {
  return impl_->m_cacheData.HasCachedDataByFilename(strFilename);
}

std::shared_ptr<ZImage> CacheManager::GetCachedData(const tstring & strFilename) const {
  return impl_->m_cacheData.GetCachedData(strFilename);
}

void CacheManager::AddCacheData(const tstring & strFilename, std::shared_ptr<ZImage> image, const bool bForceCache) {
  if (false == image->IsValid()) {
    assert(false);
    return;
  }

  if (false == m_bCacheGoOn) {
    return;
  }

  if (impl_->m_cacheData.InsertData(strFilename, image, bForceCache)) {
    if (listener_ != nullptr) {
      listener_->OnFileCached(strFilename, image);
    }
  }
}

bool CacheManager::IsNextFileCached() const {
  int iNextIndex = current_index_;

  if (view_direction_ == ViewDirection::kForward) {
    ++iNextIndex;
  }
  else {
    --iNextIndex;
  }

  iNextIndex = ZMain::GetInstance().GetCalculatedMovedIndex(iNextIndex);

  return impl_->m_cacheData.HasCachedDataByIndex(iNextIndex);
}

void CacheManager::DebugShowCacheInfo() {
  RECT rt;
  if (false == ZMain::GetInstance().getCurrentScreenRect(rt)) {
    DebugPrintf(TEXT("CurrentScreenSize : Cannot getCurrentScreenRect"));
    return;
  }
  DebugPrintf(TEXT("CurrentScreenSize : %d, %d"), rt.right, rt.bottom);

  impl_->m_cacheData.ShowCacheInfo();
}

void CacheManager::ClearCache() {
  impl_->m_cacheData.ClearCachedImageData();
  DebugPrintf(TEXT("Clear cache data"));
}

void CacheManager::SetCacheEvent() {
  impl_->m_hCacheEvent.setEvent();
}

long CacheManager::GetCachedKByte() const {
  return (impl_->m_cacheData.GetCachedTotalBytes() / 1024);
}

int CacheManager::GetCacheHitRate() const {
  if ((cache_hit_counter_ + cache_miss_counter_) == 0) {
    return 0;
  }
  return (cache_hit_counter_ * 100 / (cache_miss_counter_ + cache_hit_counter_));
}


void CacheManagerSpeedTest() {
  class CachedCounter : public CacheEventListenerInterface {
  public:
    CachedCounter() {
      count_ = 0;
    }
    void OnFileCached(const tstring& /*filename*/, std::shared_ptr<ZImage> /*image*/) override {
      ++count_;

      static const int kCheckCount = 3;
      if (count_ == kCheckCount) {
        DebugPrintf(TEXT("%d is cached. time ms: %d"), kCheckCount, caching_time.End());
      }
    }

    int count() const { return count_; }

  private:
    ElapseTime caching_time;

    std::atomic_int count_;
  };
  tstring strToFindFolder = TEXT("P:\\Temp\\");
  strToFindFolder += TEXT("*.*");

  std::vector<FileData> filelist;
  GetSortedFileList(strToFindFolder, eFileSortOrder::eFileSortOrder_FILENAME, &filelist);

  CacheManager::GetInstance().SetFilelist(filelist);
  CachedCounter counter;
  CacheManager::GetInstance().StartCacheThread();

  CacheManager::GetInstance().SetCurrent(0, filelist[0].m_strFileName);

  CacheManager::GetInstance().set_listener(&counter);

  while (counter.count() < 100000) {
    ::Sleep(100);
  }
}

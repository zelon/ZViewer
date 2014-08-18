/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
*
*   2005. 5. 7
*	ZCacheImage.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "stdafx.h"

#include "ZCacheImage.h"

#include "CachedData.h"
#include "ZOption.h"
#include "ZMain.h"
#include "../../commonSrc/MessageManager.h"

using namespace std;
using namespace std::chrono;

class ZCacheImage::Impl
{
public:
  std::vector < BYTE > file_read_buffer_;		///< buffer that keeping data of current reading file
	CachedData m_cacheData;
	CEventObj m_hCacheEvent;
};

ZCacheImage & ZCacheImage::GetInstance()
{
	static ZCacheImage ins;
	return ins;
}

ZCacheImage::ZCacheImage()
:	m_bCacheGoOn(true)
,	m_iLogCacheHit(0)
,	m_iLogCacheMiss(0)
,	m_lastActionDirection(eLastActionDirection_FORWARD)
,	m_pImpl(new Impl)
{
	m_bNowCaching = false;
}

ZCacheImage::~ZCacheImage()
{
	CleanUpThread();
}

void ZCacheImage::CleanUpCache()
{
	m_pImpl->m_cacheData.ClearCachedImageData();
}


void ZCacheImage::CleanUpThread()
{
	m_bCacheGoOn = false;

	m_pImpl->m_hCacheEvent.setEvent();

	if ( m_thread.joinable() )
	{
		DebugPrintf(TEXT("Waiting for cache thread end"));
		m_thread.join();
		DebugPrintf(TEXT("Completed waiting for cache thread end"));
	}
}

void ZCacheImage::SetImageVector(const std::vector < FileData > & v)
{
	m_pImpl->m_cacheData.SetImageVector(v);
}

void ZCacheImage::StartThread()
{
	std::thread cacheThread([]()
		{
			ZCacheImage::GetInstance().ThreadFunc();

			DebugPrintf(TEXT("End of Cache Thread"));
		}
	);

	m_thread.swap(cacheThread);

	// Cache 를 진행하는 쓰레드는
	if ( SetThreadPriority(m_thread.native_handle(), THREAD_PRIORITY_BELOW_NORMAL) == FALSE )
	{
		assert(!"Can't SetThreadPriority!");
	}
}

// 현재 캐시되어 있는 파일들을 output 윈도우로 뿌려준다.
void ZCacheImage::ShowCachedImageToOutputWindow()
{
#ifndef _DEBUG
	return; // 릴리즈 모드에서는 그냥 리턴
#else
	m_pImpl->m_cacheData.PrintCachedData();
#endif
}

size_t ZCacheImage::GetNumOfCacheImage() const
{
	return m_pImpl->m_cacheData.Size();
}

class File final {
public:
  File(const tstring& filename) {
    file_handle_ = CreateFile(filename.c_str(),
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_POSIX_SEMANTICS,
                              NULL);
  }

  ~File() {
    if (IsOpened()) {
      CloseHandle(file_handle_);
    }
  }

  bool IsOpened() const {
    return file_handle_ != INVALID_HANDLE_VALUE;
  }

  bool Read(void* read_buffer, const DWORD read_buffer_size, DWORD* number_of_bytes_read) {
    return ::ReadFile(file_handle_, read_buffer, read_buffer_size, number_of_bytes_read, nullptr) == TRUE;
  }
private:
  HANDLE file_handle_;
};

bool ZCacheImage::_CacheIndex(int iIndex)
{
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

  if (m_pImpl->m_cacheData.HasCachedData(strFileName)) {
    // found!
    bCacheFound = true;
	}

  if (bCacheFound == true) {
    return true;
  }

  std::shared_ptr<ZImage> pCacheReadyImage(new ZImage());

  m_pImpl->file_read_buffer_.resize(0);
  File file(strFileName);

  bool bLoadOK = false;

  if (file.IsOpened() == false) {
    assert(false);
    bLoadOK = false;
  } else
  {
    enum
    {
      readBufferSize = 2048
    };

    BYTE readBuffer[readBufferSize];

    DWORD dwReadBytes;
    BOOL bReadOK = TRUE;

    //DWORD dwStart = GetTickCount();
    system_clock::time_point startTime = system_clock::now();

    while ( bReadOK )
    {
      if ( m_bNewChange && m_iCurrentIndex != iIndex )	///< 새로운 그림으로 넘어갔는데 현재 인덱스가 아니면
      {
        DebugPrintf(TEXT("---------------------------- stop readfile"));
        return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
      }
      bReadOK = file.Read(readBuffer, readBufferSize, &dwReadBytes);
      if ( m_bNewChange && m_iCurrentIndex != iIndex )	///< 새로운 그림으로 넘어갔는데 현재 인덱스가 아니면
      {
        DebugPrintf(TEXT("---------------------------- stop readfile"));
        return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
      }
      if ( FALSE == bReadOK )
      {
        assert(false);
        bLoadOK = false;
        break;
      }
      else
      {
        if ( dwReadBytes <= 0 )	///< 파일의 끝까지 읽었다.
        {
          break;
        }
        else ///
        {
          m_pImpl->file_read_buffer_.resize(m_pImpl->file_read_buffer_.size() + dwReadBytes);

          memcpy((&(m_pImpl->file_read_buffer_[0])) + m_pImpl->file_read_buffer_.size() - dwReadBytes, readBuffer, dwReadBytes);
        }
      }
    }

    if (m_pImpl->file_read_buffer_.size() < 5)
    {
      /// 파일 크기가 너무 작음
      bLoadOK = false;
    }
    else
    {
      long long diffTime = duration_cast<milliseconds>(system_clock::now() - startTime).count();
      DebugPrintf(TEXT("----- readfile(%s) time(%d)"), strFileName.c_str(), diffTime);

      assert(m_pImpl->file_read_buffer_.size() > 0);
      /// todo: 아래 내용을 멤버 변수로 바꾸면 더 좋아질까
      fipMemoryIO mem(&m_pImpl->file_read_buffer_[0], (DWORD)m_pImpl->file_read_buffer_.size());

      DebugPrintf(TEXT("----- start decode(%s)"), strFileName.c_str());
      TIMECHECK_START("decode time");
      bLoadOK = pCacheReadyImage->LoadFromMemory(mem, strFileName);
      DebugPrintf(TEXT("----- end of decode(%s)"), strFileName.c_str());
      TIMECHECK_END();

      /// 옵션에 따라 자동 회전을 시킨다.
      if ( ZOption::GetInstance().IsUseAutoRotation() )
      {
        pCacheReadyImage->AutoRotate();
      }
    }
  }

  if ( bLoadOK == false ) {
    assert(!"Can't load image");

    tstring strErrorFilename = GetProgramFolder();
    strErrorFilename += TEXT("LoadError.png");
    if ( pCacheReadyImage->LoadFromFile(strErrorFilename) )
    {
      bLoadOK = true; ///< error image 라도 load ok
    }
    else
    {
      MessageBox(HWND_DESKTOP, TEXT("Please check LoadError.png in ZViewer installed folder"), TEXT("ZViewer"), MB_OK);
      const HBITMAP hBitmap = CreateBitmap(100, 100, 1, 1, NULL);
      pCacheReadyImage->CopyFromBitmap(hBitmap);
      return true;
    }
  }

  if ( iIndex == m_iCurrentIndex ) {///< 현재 보는 이미지면 무조건 넣는다.
    AddCacheData(strFileName, pCacheReadyImage, true);
    return true;
  }
  const long CachedImageSize = pCacheReadyImage->GetImageSize();

  // 읽은 이미지를 넣을 공간이 없으면
  if ((m_pImpl->m_cacheData.GetCachedTotalSize() + CachedImageSize) / 1024 / 1024 > ZOption::GetInstance().GetMaxCacheMemoryMB())
  {
    // 만약의 무한루프를 방지하기 위해 100번만 돌린다.
    for (int i = 0; i < 100; ++i)
    {
      // 가장 멀리있는 이미지의 캐시를 clear 해서 캐시 용량을 확보해본다.
      if (ClearFarthestCache(iIndex) == false) {
        return false;
      }
      // 이제 어느 정도 용량을 확보했으니 다시 이 이미지를 넣을 수 있는 지 캐시를 체크한다.
      if ((m_pImpl->m_cacheData.GetCachedTotalSize() + CachedImageSize) / 1024 / 1024 <= ZOption::GetInstance().GetMaxCacheMemoryMB())
      {
        AddCacheData(strFileName, pCacheReadyImage);
        return true;
      }
      else
      {
        DebugPrintf(TEXT("There is no vacant space"));
      }
      assert(i <= 99);
    }
  }
  else
  {
    AddCacheData(strFileName, pCacheReadyImage);
    if ( m_bNewChange) return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
  }
  return true;
}

bool ZCacheImage::ClearFarthestCache(const int index) {
  // 캐시되어 있는 것들 중 가장 현재 index 에서 먼것을 찾는다.
  const int iFarthestIndex = m_pImpl->m_cacheData.GetFarthestIndexFromCurrentIndex(m_iCurrentIndex);
  assert(iFarthestIndex >= 0 );

  const size_t nCachedFarthestDiff = abs(iFarthestIndex - m_iCurrentIndex);
  const size_t nToCacheDiff = abs(index - m_iCurrentIndex );

  if ( nCachedFarthestDiff < nToCacheDiff )
  {
    // 캐시 했는 것 중 가장 멀리있는 것이 이번거보다 가까운데 있으면 더이상 캐시하지 않는다
    return false;
  }
  else if ( nCachedFarthestDiff == nToCacheDiff )
  {
    // 캐시했는 거랑 이번에 캐시할 것이 동등한 위치에 있으면

    if ( m_lastActionDirection == eLastActionDirection_FORWARD )
    {
      // 앞으로 진행 중이면 가장 멀리있는 것이 prev 이면 지운다(앞으로 가고 있을 때는 next image 가 우선순위가 높다)
      if ( iFarthestIndex >= index )
      {
        // 캐시되어 있는 것을 비우지 않는다.
        return false;
      }
    }
    else
    {
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

void ZCacheImage::ThreadFunc() {
	m_bNewChange = false;
	while ( m_bCacheGoOn ) // thread loop
	{
		m_bNowCaching = true;
	  int iPos = 0;
		assert((int)m_pImpl->m_cacheData.GetImageVectorSize() == (int)m_pImpl->m_cacheData.GetIndex2FilenameMapSize());
		assert(m_iCurrentIndex <= (int)m_pImpl->m_cacheData.GetIndex2FilenameMapSize());
		assert(m_iCurrentIndex <= (int)m_pImpl->m_cacheData.GetImageVectorSize());

#ifdef _DEBUG

		{
			if (m_pImpl->m_cacheData.IsEmpty())
			{
				assert(m_pImpl->m_cacheData.GetCachedTotalSize() == 0);
			}
		}
#endif

		for ( int i=0; i<ZOption::GetInstance().m_iMaxCacheImageNum/2; ++i)
		{
			if ( false == m_bCacheGoOn ) break; ///< 프로그램이 종료되었으면 for 를 끝낸다.
			if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

			/// 항상 현재 이미지를 먼저 캐쉬한다.
			_CacheIndex(m_iCurrentIndex);
			if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

			/// 현재보고 있는 방향에 따라서 어디쪽 이미지를 먼저 캐시할 것인지 판단한다.

			if ( m_lastActionDirection == eLastActionDirection_FORWARD )
			{
				// right side
				if ( false == _CacheIndex(m_iCurrentIndex + iPos) ) break;
				if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

				// left side
				if ( false == _CacheIndex(m_iCurrentIndex - iPos) ) break;
				if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
			}
			else if ( m_lastActionDirection == eLastActionDirection_BACKWARD )
			{
				// left side
				if ( false == _CacheIndex(m_iCurrentIndex - iPos) ) break;
				if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

				// right side
				if ( false == _CacheIndex(m_iCurrentIndex + iPos) ) break;
				if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
			}
			else
			{
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

bool ZCacheImage::hasCachedData(const tstring & strFilename, int iIndex)
{
	// index 를 체크한다.
	m_iCurrentIndex = iIndex;
	m_strCurrentFileName = strFilename;

	m_bNewChange = true;

	m_pImpl->m_hCacheEvent.setEvent();

	{
		if (m_pImpl->m_cacheData.HasCachedData(strFilename)) return true;
	}
	return false;
}

std::shared_ptr<ZImage> ZCacheImage::GetCachedData(const tstring & strFilename) const
{
  return m_pImpl->m_cacheData.GetCachedData(strFilename);
}


void ZCacheImage::AddCacheData(const tstring & strFilename, std::shared_ptr<ZImage> pImage, bool bForceCache)
{
	if ( false == pImage->IsValid() )
	{
		assert(false);
		return;
	}

	if ( false == m_bCacheGoOn ) return;

	m_pImpl->m_cacheData.InsertData(strFilename, pImage, bForceCache);
}

/// 다음 파일이 캐쉬되었나를 체크해서 돌려준다.
bool ZCacheImage::IsNextFileCached() const
{
	int iNextIndex = m_iCurrentIndex;

	if ( m_lastActionDirection == eLastActionDirection_FORWARD )
	{
		++iNextIndex;
	}
	else
	{
		--iNextIndex;
	}

	iNextIndex = ZMain::GetInstance().GetCalculatedMovedIndex(iNextIndex);

	return m_pImpl->m_cacheData.HasCachedData(iNextIndex);
}

void ZCacheImage::WaitCacheLock()
{
	m_pImpl->m_cacheData.WaitCacheLock();
}

/// 현재 캐쉬 정보를 디버그 콘솔에 보여준다. 디버깅모드 전용
void ZCacheImage::debugShowCacheInfo()
{
	RECT rt;
	if ( false == ZMain::GetInstance().getCurrentScreenRect(rt) )
	{
		DebugPrintf(TEXT("CurrentScreenSize : Cannot getCurrentScreenRect"));
		return;
	}
	DebugPrintf(TEXT("CurrentScreenSize : %d, %d"), rt.right, rt.bottom);

	m_pImpl->m_cacheData.ShowCacheInfo();
}


void ZCacheImage::clearCache()
{
	m_pImpl->m_cacheData.ClearCachedImageData();
	DebugPrintf(TEXT("Clear cache data"));
}

void ZCacheImage::setCacheEvent()
{
	m_pImpl->m_hCacheEvent.setEvent();
}


long ZCacheImage::GetCachedKByte() const
{
	return (m_pImpl->m_cacheData.GetCachedTotalSize() / 1024);
}

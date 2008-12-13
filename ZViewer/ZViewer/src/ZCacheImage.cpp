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
#include "ZOption.h"
#include "ZMain.h"
#include "../../commonSrc/MessageManager.h"

using namespace std;


ZCacheImage & ZCacheImage::GetInstance()
{
	static ZCacheImage ins;
	return ins;
}

ZCacheImage::ZCacheImage()
:	m_bCacheGoOn(true)
,	m_iLogCacheHit(0)
,	m_iLogCacheMiss(0)
,	m_lCacheSize(0)
,	m_numImageVectorSize(0)
,	m_lastActionDirection(eLastActionDirection_FORWARD)
{
	m_bNowCaching = false;
	m_hThread = INVALID_HANDLE_VALUE;
}

ZCacheImage::~ZCacheImage()
{
	m_bCacheGoOn = false;

	m_hCacheEvent.setEvent();

	if ( m_hThread != INVALID_HANDLE_VALUE )
	{
		/// 캐쉬 쓰레드가 종료하길 기다린다.
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);

		m_hThread = INVALID_HANDLE_VALUE;
	}

	DebugPrintf(TEXT("Cached Thread ended."));
}

void ZCacheImage::SetImageVector(const std::vector < FileData > & v)
{
	CLockObjUtil lock(m_cacheLock);

	m_numImageVectorSize = v.size();

	m_cacheData.SetNewFileList(v, m_numImageVectorSize);

	m_cacheData.Clear();
	m_lCacheSize = 0;

	DebugPrintf(TEXT("imageVecSize : %d"), m_numImageVectorSize);
}

void ZCacheImage::StartThread()
{
	if ( ZOption::GetInstance().IsUseCache() )
	{
		DWORD dwThreadID;
		m_hThread = CreateThread(0, 0, ThreadFuncProxy, this, 0, &dwThreadID);

		// Cache 를 진행하는 쓰레드는
		if ( SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL) == FALSE )
		{
			assert(!"Can't SetThreadPriority!");
		}
	}
}

// 현재 캐시되어 있는 파일들을 output 윈도우로 뿌려준다.
void ZCacheImage::ShowCachedImageToOutputWindow()
{
#ifndef _DEBUG
	return; // 릴리즈 모드에서는 그냥 리턴
#endif

	CLockObjUtil lock(m_cacheLock);

	m_cacheData.PrintCachedData();

}

DWORD ZCacheImage::ThreadFuncProxy(LPVOID )
{
	ZCacheImage::GetInstance().ThreadFunc();
	return 0;
}


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

	if ( iIndex < 0 ) iIndex = 0;
	if ( iIndex >= (int)m_numImageVectorSize ) iIndex = (int)m_numImageVectorSize - 1;

	// 이미 캐시되어 있는지 찾는다.
	bool bFound = false;
	tstring strFileName;

	strFileName = m_cacheData.GetFilenameFromIndex(iIndex); ;//m_imageIndex2FilenameMap[iIndex];

	if ( strFileName.length() <= 0 ) return false;

	{
		CLockObjUtil lock(m_cacheLock);
		if ( m_cacheData.HasCachedData(strFileName) )
		{
			// found!
			bFound = true;
		}
	}

	if ( bFound == false )	// 캐시되어 있지 않으면 읽어들인다.
	{
		ZImage cacheReadyImage;

		m_vBuffer.resize(0);
		HANDLE hFile = CreateFile(strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_POSIX_SEMANTICS,  NULL);

		bool bLoadOK = false;

		if ( INVALID_HANDLE_VALUE == hFile )
		{
			assert(false);
			bLoadOK = false;
		}
		else
		{
			enum
			{
				readBufferSize = 2048
			};
			
			BYTE readBuffer[readBufferSize];

			DWORD dwReadBytes;
			BOOL bReadOK = TRUE;

			DWORD dwStart = GetTickCount();

			while ( bReadOK )
			{
				if ( m_bNewChange)
				{
					CloseHandle(hFile);
					DebugPrintf(TEXT("---------------------------- stop readfile"));
					return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
				}
				bReadOK = ReadFile(hFile, readBuffer, readBufferSize, &dwReadBytes,  NULL);
				if ( m_bNewChange)
				{
					CloseHandle(hFile);
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
						m_vBuffer.resize(m_vBuffer.size() + dwReadBytes);

						memcpy((&(m_vBuffer[0])) + m_vBuffer.size() - dwReadBytes, readBuffer, dwReadBytes);
					}
				}
			}
			DWORD dwEnd = GetTickCount();
			DebugPrintf(TEXT("----- readfile(%s) time(%d)"), strFileName.c_str(), dwEnd - dwStart);
			CloseHandle(hFile);	///< 파일에서 읽기가 끝나서 파일을 닫아준다.

			/// todo: 아래 내용을 멤버 변수로 바꾸면 더 좋아질까
			fipMemoryIO mem(&m_vBuffer[0], (DWORD)m_vBuffer.size());
			
			DebugPrintf(TEXT("----- start decode(%s)"), strFileName.c_str());
			TIMECHECK_START("decode time");
			bLoadOK = cacheReadyImage.LoadFromMemory(mem, strFileName);
			DebugPrintf(TEXT("----- end of decode(%s)"), strFileName.c_str());
			TIMECHECK_END();
		}

		if ( bLoadOK == false )
		{
			assert(!"Can't load image");

			tstring strErrorFilename = GetProgramFolder();
			strErrorFilename += TEXT("LoadError.png");
			if ( !cacheReadyImage.LoadFromFile(strErrorFilename) )
			{
				MessageBox(HWND_DESKTOP, TEXT("Please check LoadError.png in ZViewer installed folder"), TEXT("ZViewer"), MB_OK);

				// 에러 때 표시하는 파일을 읽어들이지 못 했으면
				//ShowMessageBox(GetMessage(TEXT("CANNOT_LOAD_ERROR_IMAGE_FILE")));

				HBITMAP hBitmap = CreateBitmap(100, 100, 1, 1, NULL);
				cacheReadyImage.CopyFromBitmap(hBitmap);
			}
		}


		if ( bLoadOK )
		{
			/*
			{/// Debug Code
				//if ( m_bNewChange) return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

				AddCacheData(strFileName, temp);

				if ( m_bNewChange) return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
				*/

				/*
				WORD tempWidth, tempHeight;
				long tempImageSize = cacheReayImage.GetImageSize();

				tempWidth = cacheReayImage.GetWidth();
				tempHeight = cacheReayImage.GetHeight();

				temp.Resize(10, 10);

				tempWidth = cacheReayImage.GetWidth();
				tempHeight = cacheReayImage.GetHeight();
				tempImageSize = cacheReayImage.GetImageSize();
			}
			*/

#if 0
			if ( ZOption::GetInstance().IsBigToSmallStretchImage() )
			{
				RECT screenRect = { 0 };
				if ( false == ZMain::GetInstance().getCurrentScreenRect(screenRect) ) return false;

				RECT imageRect = { 0 };
				imageRect.right = cacheReayImage.GetWidth();
				imageRect.bottom = cacheReayImage.GetHeight();

				if ( imageRect.right > screenRect.right || imageRect.bottom > screenRect.bottom )
				{
					RECT newRect = GetResizedRectForBigToSmall(screenRect, imageRect);

					if ( newRect.right != imageRect.right || newRect.bottom != imageRect.bottom )
					{
						DebugPrintf(TEXT("Resizing Cache..."));
						cacheReayImage.Resize((WORD)newRect.right, (WORD)newRect.bottom);
					}
				}
			}

			if ( ZOption::GetInstance().IsSmallToBigStretchImage() )
			{
				RECT screenRect;
				if ( false == ZMain::GetInstance().getCurrentScreenRect(screenRect) ) return false;

				/*
				if ( screenRect.right > 10 )
				{
					--screenRect.right;
				}
				if ( screenRect.bottom > 10 )
				{
					--screenRect.bottom;
				}
				*/

				RECT imageRect = { 0 };
				imageRect.right = cacheReayImage.GetWidth();
				imageRect.bottom = cacheReayImage.GetHeight();

				if ( imageRect.right < screenRect.right && imageRect.bottom < screenRect.bottom )
				{
					RECT newRect = GetResizedRectForSmallToBig(screenRect, imageRect);

					if ( newRect.right != imageRect.right || newRect.bottom != imageRect.bottom )
					{
						DebugPrintf(TEXT("Resizing Cache..."));
						cacheReayImage.Resize((WORD)newRect.right, (WORD)newRect.bottom);
					}
				}
			}
#endif
			const long CachedImageSize = cacheReadyImage.GetImageSize();

			if ( iIndex == m_iCurrentIndex )	///< 현재 보는 이미지면 무조건 넣는다.
			{
				AddCacheData(strFileName, cacheReadyImage, true);
				return true;
			}
			// 읽은 이미지를 넣을 공간이 없으면
			else if ( (m_lCacheSize + CachedImageSize) / 1024 / 1024 > ZOption::GetInstance().GetMaxCacheMemoryMB() )
			{
				int iTemp = 100;
				int iFarthestIndex = -1;

				do
				{
					// 캐시되어 있는 것들 중 가장 현재 index 에서 먼것을 찾는다.
					
					{
						CLockObjUtil lock(m_cacheLock);
						iFarthestIndex = m_cacheData.GetFarthestIndexFromCurrentIndex(m_iCurrentIndex);
					}
					assert(iFarthestIndex >= 0 );

					size_t nCachedFarthestDiff = abs(iFarthestIndex - m_iCurrentIndex);
					size_t nToCacheDiff = abs(iIndex - m_iCurrentIndex );

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
							if ( iFarthestIndex >= iIndex )
							{
								// 캐시되어 있는 것을 비우지 않는다.
								return false;
							}
						}
						else
						{
							// 뒤로 진행 중이면 가장 멀리있는 것이 next 이면 지운다.
							if ( iFarthestIndex <= iIndex )
							{
								return false;
							}
						}
					}

					//  현재 것이 더 가깝기 때문에 가장 먼 것을 클리어하고, 현재 것을 캐시해 놓는 것이 좋은 상황이다.

					// 가장 먼 것을 clear 한다.
					{
						CLockObjUtil lock(m_cacheLock);
						m_cacheData.ClearFarthestDataFromCurrent(iFarthestIndex, m_lCacheSize);
					}

					// 이제 어느 정도 용량을 확보했으니 다시 이 이미지를 넣을 수 있는 지 캐시를 체크한다.
					if ( (m_lCacheSize + CachedImageSize) / 1024 / 1024 <= ZOption::GetInstance().GetMaxCacheMemoryMB() )
					{
						AddCacheData(strFileName, cacheReadyImage);
						return true;
					}
					else
					{
						DebugPrintf(TEXT("There is no vacant space"));
					}

					// 만약의 무한루프를 방지하기 위해 100번만 돌린다.
					--iTemp;
				} while( iTemp > 0 );

				assert(iTemp >= 0 );
			}
			else
			{
				AddCacheData(strFileName, cacheReadyImage);
				if ( m_bNewChange) return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
			}
		}
	}
	return true;
}

void ZCacheImage::ThreadFunc()
{
	m_bNewChange = false;
	CacheMapIterator it, endit;
	int iPos = 0;
	int i = 0;

	while ( m_bCacheGoOn ) // thread loop
	{
		m_bNowCaching = true;
		iPos = 0;
		assert((int)m_numImageVectorSize == (int)m_cacheData.GetIndex2FilenameMapSize());
		assert(m_iCurrentIndex <= (int)m_cacheData.GetIndex2FilenameMapSize());
		assert(m_iCurrentIndex <= (int)m_numImageVectorSize);

#ifdef _DEBUG

		{
			CLockObjUtil lock(m_cacheLock);
			if ( m_cacheData.IsEmpty() )
			{
				assert(m_lCacheSize == 0);
			}
		}
#endif

		for ( i=0; i<ZOption::GetInstance().m_iMaxCacheImageNum/2; ++i)
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
		m_hCacheEvent.wait();
		m_bNewChange = false;

		DebugPrintf(TEXT("Recv event"));
	}
}

bool ZCacheImage::hasCachedData(const tstring & strFilename, int iIndex)
{
	if ( false == ZOption::GetInstance().IsUseCache() ) return false;	///< 현재 캐쉬를 쓰지 않으면 없는거다.

	// index 를 체크한다.
	m_iCurrentIndex = iIndex;
	m_strCurrentFileName = strFilename;

	m_bNewChange = true;

	m_hCacheEvent.setEvent();

	{
		CLockObjUtil lock(m_cacheLock);

		if ( m_cacheData.HasCachedData(strFilename) ) return true;
	}
	return false;
}

void ZCacheImage::getCachedData(const tstring & strFilename, ZImage & image)
{
	CLockObjUtil lock(m_cacheLock);

	m_cacheData.GetCachedData(strFilename, image);
}


void ZCacheImage::AddCacheData(const tstring & strFilename, ZImage & image, bool bForceCache)
{
	if ( false == image.IsValid() )
	{
		assert(false);
		return;
	}

	long imageSize = image.GetImageSize();

	CLockObjUtil lock(m_cacheLock);

	/// 이미 캐시되어 있으면 캐시하지 않는다.
	if ( m_cacheData.HasCachedData(strFilename) ) return;

	if ( false == bForceCache )
	{
		/// 용량을 체크해서 이 이미지를 캐시했을 때 제한을 넘어섰으면 캐시하지 않는다.
		if ( (m_lCacheSize + imageSize) /1024/1024 > ZOption::GetInstance().GetMaxCacheMemoryMB() ) return;
	}

	if ( false == m_bCacheGoOn ) return;

#ifndef _DEBUG
	try
	{
#endif
		DWORD dwStart = GetTickCount();

		m_cacheData.InsertData(strFilename, image);
		m_lCacheSize += imageSize;

		DebugPrintf(TEXT("%s added to cache"), strFilename.c_str());
		DWORD dwEnd = GetTickCount();
		DebugPrintf(TEXT("Cache insert time : %d filename(%s)"), dwEnd - dwStart, strFilename.c_str());

#ifndef _DEBUG
	}
	catch ( ... )
	{
		return;
	}
#endif
}

/// 다음 파일이 캐쉬되었나를 체크해서 돌려준다.
bool ZCacheImage::IsNextFileCached() const
{
	CLockObjUtil lock(m_cacheLock);

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

	return m_cacheData.HasCachedData(iNextIndex);
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

	CLockObjUtil lock(m_cacheLock);
	m_cacheData.ShowCacheInfo();
}


void ZCacheImage::clearCache()
{
	CLockObjUtil lock(m_cacheLock);
	m_cacheData.Clear();
	m_lCacheSize = 0;
	DebugPrintf(TEXT("Clear cache data"));
}


long ZCacheImage::GetCachedKByte() const
{
	return (m_lCacheSize/1024);
}

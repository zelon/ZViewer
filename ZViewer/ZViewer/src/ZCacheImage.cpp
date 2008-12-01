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

using namespace std;

ZCacheImage & ZCacheImage::GetInstance()
{
	static ZCacheImage ins;
	return ins;
}

ZCacheImage::ZCacheImage()
:	m_bGoOn(true)
,	m_iLogCacheHit(0)
,	m_iLogCacheMiss(0)
,	m_lCacheSize(0)
,	m_numImageVectorSize(0)
,	m_lastActionDirection(eLastActionDirection_FORWARD)
{
	m_bNowCaching = false;
	m_hThread = INVALID_HANDLE_VALUE;
	m_imageMap.clear();
	m_imageMapRev.clear();
	m_cacheData.clear();

}

ZCacheImage::~ZCacheImage()
{
	m_bGoOn = false;

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

	m_imageMap.clear();
	m_imageMapRev.clear();
	m_cacheData.clear();
	m_lCacheSize = 0;

	for ( size_t i = 0; i < m_numImageVectorSize; ++i)
	{
		m_imageMap.insert(std::make_pair((int)i, v[i].m_strFileName));
		m_imageMapRev.insert(std::make_pair(v[i].m_strFileName, (int)i));
	}

	DebugPrintf(TEXT("imageMapSize : %d"), m_imageMap.size());
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
			_ASSERTE(!"Can't SetThreadPriority!");
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

	CacheMapIterator it, endIt = m_cacheData.end();

	for ( it = m_cacheData.begin(); it != endIt; ++it)
	{
		DebugPrintf(TEXT("Cacaed Filename : %s"), it->first.c_str());
	}
}

DWORD ZCacheImage::ThreadFuncProxy(LPVOID p)
{
	ZCacheImage * pThis = (ZCacheImage*)p;

	pThis->ThreadFunc();

	return 0;
}

/// 캐시되어 있는 데이터들 중 현재 인덱스로부터 가장 멀리있는 인덱스를 얻는다.
int ZCacheImage::_GetFarthestIndexFromCurrentIndex()
{
	CLockObjUtil lock(m_cacheLock);

	CacheMapIterator it, endIt = m_cacheData.end();

	int iFarthestIndex = m_iCurrentIndex;
	int iDistanceMax = 0;
	int iDistance = 0;
	int iTempIndex = 0;

	tstring strFarthest;

	for ( it = m_cacheData.begin(); it != endIt; ++it)
	{
		iTempIndex = m_imageMapRev[it->first];
		iDistance = abs(iTempIndex - m_iCurrentIndex);

		if ( iDistance > iDistanceMax )
		{
			iDistanceMax = iDistance;
			iFarthestIndex = iTempIndex;
			strFarthest = it->first;
		}
	}

	_ASSERTE(iFarthestIndex >= 0 );

	return iFarthestIndex;
}

bool ZCacheImage::_CacheIndex(int iIndex)
{
	// 최대 캐시 크기를 넘었으면 더 이상 캐시하지 않는다.
	if ( (m_lCacheSize / 1024 / 1024) > ZOption::GetInstance().GetMaxCacheMemoryMB() )
	{
		return false;
	}

	if ( iIndex < 0 ) iIndex = 0;
	if ( iIndex >= (int)m_numImageVectorSize ) iIndex = (int)m_numImageVectorSize - 1;

	// 이미 캐시되어 있는지 찾는다.
	bool bFound = false;
	tstring strFileName;

	strFileName = m_imageMap[iIndex];

	if ( strFileName.length() <= 0 ) return false;

	{
		CLockObjUtil lock(m_cacheLock);
		if ( m_cacheData.count(strFileName) > 0)
		{
			// found!
			bFound = true;
		}
	}

	if ( bFound == false )	// 캐시되어 있지 않으면 읽어들인다.
	{
		ZImage cacheReayImage;
		if ( cacheReayImage.LoadFromFile(strFileName) )
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
			// 읽은 이미지를 넣을 공간이 없으면
			if ( (m_lCacheSize + cacheReayImage.GetImageSize()) / 1024 / 1024 > ZOption::GetInstance().GetMaxCacheMemoryMB() )
			{
				int iTemp = 100;
				int iFarthestIndex = -1;

				do
				{
					// 캐시되어 있는 것들 중 가장 현재 index 에서 먼것을 찾는다.
					iFarthestIndex = _GetFarthestIndexFromCurrentIndex();
					_ASSERTE(iFarthestIndex >= 0 );

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

						tstring strFindFileName = m_imageMap[iFarthestIndex];
						CacheMapIterator it = m_cacheData.find(m_imageMap[iFarthestIndex]);

						if ( it != m_cacheData.end() )
						{
							m_lCacheSize -= it->second.GetImageSize();
							m_cacheData.erase(it);

							DebugPrintf(TEXT("Farthest one clear"));
						}
						else
						{
							_ASSERTE(!"Can't find the cache data.");
							return false;
						}
					}

					// 이제 어느 정도 용량을 확보했으니 다시 이 이미지를 넣을 수 있는 지 캐시를 체크한다.
					if ( (m_lCacheSize + cacheReayImage.GetImageSize()) / 1024 / 1024 <= ZOption::GetInstance().GetMaxCacheMemoryMB() )
					{
						AddCacheData(strFileName, cacheReayImage);
						return true;
					}

					// 만약의 무한루프를 방지하기 위해 100번만 돌린다.
					--iTemp;
				} while( iTemp > 0 );

				_ASSERTE(iTemp >= 0 );
			}
			else
			{
				AddCacheData(strFileName, cacheReayImage);
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

	while ( m_bGoOn ) // thread loop
	{
		m_bNowCaching = true;
		iPos = 0;
		_ASSERTE((int)m_numImageVectorSize == (int)m_imageMap.size());
		_ASSERTE(m_iCurrentIndex <= (int)m_imageMap.size());
		_ASSERTE(m_iCurrentIndex <= (int)m_numImageVectorSize);

#ifdef _DEBUG

		{
			CLockObjUtil lock(m_cacheLock);
			if ( m_cacheData.empty() )
			{
				_ASSERTE(m_lCacheSize == 0);
			}
		}
#endif

		for ( i=0; i<ZOption::GetInstance().m_iMaxCacheImageNum/2; ++i)
		{
			if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.

			/// 현재보고 있는 방향에 따라서 어디쪽 이미지를 먼저 캐시할 것인지 판단한다.
			if ( m_lastActionDirection == eLastActionDirection_FORWARD )
			{
				// right side
				if ( false == _CacheIndex(m_iCurrentIndex + iPos) ) break;

				// left side
				if ( false == _CacheIndex(m_iCurrentIndex - iPos) ) break;
			}
			else if ( m_lastActionDirection == eLastActionDirection_BACKWARD )
			{
				// left side
				if ( false == _CacheIndex(m_iCurrentIndex - iPos) ) break;

				// right side
				if ( false == _CacheIndex(m_iCurrentIndex + iPos) ) break;
			}
			else
			{
				_ASSERTE(false);
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
	if ( false == ZOption::GetInstance().IsUseCache() ) return false;

	// index 를 체크한다.
	m_iCurrentIndex = iIndex;
	m_bNewChange = true;

	m_hCacheEvent.setEvent();

	{
		CLockObjUtil lock(m_cacheLock);

		if ( m_cacheData.count(strFilename) > 0 ) return true;
	}
	return false;
}

void ZCacheImage::getCachedData(const tstring & strFilename, ZImage & image)
{
	CacheMapIterator it;
	CLockObjUtil lock(m_cacheLock);

	it = m_cacheData.find(strFilename);
	if ( it == m_cacheData.end() )
	{
		_ASSERTE(!"Can't get NOT CACHED");
	}
	else
	{
		DebugPrintf(TEXT("Cache hit"));
	}
	image = it->second;
}


void ZCacheImage::AddCacheData(const tstring & strFilename, ZImage & image)
{
	CLockObjUtil lock(m_cacheLock);

	/// 이미 캐시되어 있으면 캐시하지 않는다.
	if ( m_cacheData.count(strFilename) > 0) return;

	/// 용량을 체크해서 이 이미지를 캐시했을 때 제한을 넘어섰으면 캐시하지 않는다.
	if ( (m_lCacheSize + image.GetImageSize()) /1024/1024 > ZOption::GetInstance().GetMaxCacheMemoryMB() ) return;

	DebugPrintf(TEXT("%s added to cache"), strFilename.c_str());

#ifdef _DEBUG
	m_cacheData[strFilename] = image;
	m_lCacheSize += m_cacheData[strFilename].GetImageSize();
#else
	try
	{
		m_cacheData[strFilename] = image;
	}
	catch ( ... )
	{
		return;
	}
	m_lCacheSize += m_cacheData[strFilename].GetImageSize();
#endif
}


void ZCacheImage::debugShowCacheInfo()
{
	RECT rt;
	if ( false == ZMain::GetInstance().getCurrentScreenRect(rt) )
	{
		DebugPrintf(TEXT("CurrentScreenSize : Cannot getCurrentScreenRect"));
		return;
	}
	DebugPrintf(TEXT("CurrentScreenSize : %d, %d"), rt.right, rt.bottom);

	std::map < tstring, ZImage >::iterator it;

	CLockObjUtil lock(m_cacheLock);
	for ( it = m_cacheData.begin(); it != m_cacheData.end(); ++it )
	{
		ZImage & image = it->second;

		DebugPrintf(TEXT("[%s] width(%d) height(%d)"), it->first.c_str(), image.GetWidth(), image.GetHeight());
	}
}


void ZCacheImage::clearCache()
{
	CLockObjUtil lock(m_cacheLock);
	m_cacheData.clear();
	m_lCacheSize = 0;
	DebugPrintf(TEXT("Clear cache data"));
}


long ZCacheImage::GetCachedKByte() const
{
	return (m_lCacheSize/1024);
}

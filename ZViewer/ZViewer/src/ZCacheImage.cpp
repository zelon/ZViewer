
#include "stdafx.h"
#include "ZCacheImage.h"

using namespace std;

ZCacheImage & ZCacheImage::GetInstance()
{
	static ZCacheImage ins;
	return ins;
}

ZCacheImage::ZCacheImage()
:	m_bGoOn(true)
,	m_iCacheLeftIndex(-1)
,	m_iCacheRightIndex(-1)
,	m_bUseCache(true)
,	m_iLogCacheHit(0)
,	m_iLogCacheMiss(0)
,	m_lCacheSize(0)
,	m_iMaxCacheImageNum(10)
,	m_iMaximumCacheMemoryMB(50)
{
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_imageMap.clear();
	m_imageMapRev.clear();
	m_cacheData.clear();

	InitializeCriticalSection(&m_lock);
}

ZCacheImage::~ZCacheImage()
{
	m_bGoOn = false;
	SetEvent(m_hEvent);

	WaitForSingleObject(m_hThread, INFINITE);

	DebugPrintf("Cached Thread ended.");

	CloseHandle(m_hThread);
	CloseHandle(m_hEvent);

	DeleteCriticalSection(&m_lock);
}

void ZCacheImage::SetImageVector(std::vector < std::string > & v)
{
	m_imageVector = v;
	m_imageMap.clear();
	m_imageMapRev.clear();
	m_cacheData.clear();
	m_lCacheSize = 0;

	for ( size_t i = 0; i < m_imageVector.size(); ++i)
	{
		m_imageMap[(int)i] = m_imageVector[i];
		m_imageMapRev[m_imageVector[i]] = (int)i;
	}

	DebugPrintf("imageMapSize : %d", m_imageMap.size());
	DebugPrintf("imageVec : %d", m_imageVector.size());
}

void ZCacheImage::StartThread()
{
	DWORD dwThreadID;
	m_hThread = CreateThread(0, 0, ThreadFuncProxy, this, 0, &dwThreadID);

	if ( SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL) == FALSE )
	{
		_ASSERTE(!"Can't SetThreadPriority!");
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
	ZCacheLock lock;

	CacheMapIterator it, endIt = m_cacheData.end();

	int iFarthestIndex = m_iCurrentIndex;
	int iDistanceMax = 0;
	int iDistance = 0;
	int iTempIndex = 0;

	std::string strFarthest;

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

bool ZCacheImage::CacheIndex(int iIndex)
{
	// 최대 캐시 크기를 넘었으면 더 이상 캐시하지 않는다.
	if ( (m_lCacheSize / 1024 / 1024) > m_iMaximumCacheMemoryMB )
	{
		return false;
	}

	if ( iIndex < 0 ) iIndex = 0;
	if ( iIndex >= (int)m_imageVector.size() ) iIndex = (int)m_imageVector.size() - 1;
	if ( iIndex == m_iCurrentIndex ) return true;

	// 이미 캐시되어 있는지 찾는다.
	bool bFound = false;
	std::string strFileName;

	strFileName = m_imageMap[iIndex];
	{
		ZCacheLock lock;
		if ( m_cacheData.count(strFileName) > 0)
		{
			// found!
			bFound = true;
		}
	}

	if ( bFound == false )	// 캐시되어 있지 않으면 읽어들인다.
	{
		ZImage temp;
		if ( temp.LoadFromFile(strFileName) )
		{
			/*
			//if ( m_bNewChange) return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
			
			AddCacheData(strFileName, temp);

			if ( m_bNewChange) return false;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
			*/

			// 읽은 이미지를 넣을 공간이 없으면
			if ( (m_lCacheSize + temp.GetImageSize()) / 1024 / 1024 > m_iMaximumCacheMemoryMB )
			{
				int iTemp = 100;
				int iFarthestIndex = -1;

				do
				{
					// 캐시되어 있는 것들 중 가장 현재 index 에서 먼것을 찾는다.
					iFarthestIndex = _GetFarthestIndexFromCurrentIndex();

					if ( abs(iFarthestIndex - m_iCurrentIndex) <= abs(iIndex - m_iCurrentIndex ) )
					{
						return false;
					}
					else
					{
						//  현재 것이 더 가깝기 때문에 가장 먼 것을 클리어하고, 현재 것을 캐시해 놓는 것이 좋은 상황이다.

						// 가장 먼 것을 clear 한다.
						{
							ZCacheLock lock;

							std::string strFindFileName = m_imageMap[iFarthestIndex];
							CacheMapIterator it = m_cacheData.find(m_imageMap[iFarthestIndex]);

							if ( it != m_cacheData.end() )
							{
								m_lCacheSize -= it->second.GetImageSize();
								m_cacheData.erase(it);

								DebugPrintf("Farthest one clear");
							}
							else
							{
								_ASSERTE(!"Can't find the cache data.");
								return false;
							}
						}

						// 이제 어느 정도 용량을 확보했으니 다시 이 이미지를 넣을 수 있는 지 캐시를 체크한다.
						if ( (m_lCacheSize + temp.GetImageSize()) / 1024 / 1024 <= m_iMaximumCacheMemoryMB )
						{
							AddCacheData(strFileName, temp);
						}

					}

					// 만약의 무한루프를 방지하기 위해 10번만 돌린다.
					--iTemp;
				} while( iTemp > 0 );

				_ASSERTE(iTemp > 0 );
			}
			else
			{
				AddCacheData(strFileName, temp);
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

	while ( m_bGoOn )
	{
		iPos = 1;
		_ASSERTE((int)m_imageVector.size() == (int)m_imageMap.size());
		_ASSERTE(m_iCurrentIndex <= (int)m_imageMap.size());
		_ASSERTE(m_iCurrentIndex <= (int)m_imageVector.size());

#ifdef _DEBUG
		if ( m_cacheData.empty() )
		{
			_ASSERTE(m_lCacheSize == 0);
		}
#endif

		/// 캐시 데이터를 보고 적절히 캐시를 비워준다.
		//CheckCacheDataAndClear();

		//for ( i=0; i<m_iMaxCacheImageNum/2; ++i)
		for ( int i=0; i<100; ++i)
		{
			if ( m_bNewChange) break;	// 현재보고 있는 파일 인덱스가 바뀌었으면 빨리 다음 for 를 시작한다.
			
			// left side
			if ( !CacheIndex(m_iCurrentIndex - iPos) ) break;

			// right side
			if ( !CacheIndex(m_iCurrentIndex + iPos) ) break;

			++iPos;
		}

		DebugPrintf("wait event");

		WaitForSingleObject(m_hEvent, INFINITE);
		m_bNewChange = false;

		DebugPrintf("Recv event\r\n");
	}
}

bool ZCacheImage::hasCachedData(const std::string & strFilename, int iIndex)
{
	if ( ! m_bUseCache ) return false;

	// index 를 체크한다.
	m_iCurrentIndex = iIndex;
	m_bNewChange = true;

	SetEvent(m_hEvent);

	{
		ZCacheLock lock;
		
		if ( m_cacheData.count(strFilename) > 0 ) return true;
	}
	return false;
}

void ZCacheImage::getCachedData(const std::string & strFilename, ZImage & image)
{
	CacheMapIterator it;
	ZCacheLock lock;
	
	it = m_cacheData.find(strFilename);
	if ( it == m_cacheData.end() )
	{
		_ASSERTE(!"Can't get NOT CACHED");
	}
	else
	{
		DebugPrintf("Cache hit");
	}
	image = it->second;
}


void ZCacheImage::AddCacheData(const std::string & strFilename, ZImage & image)
{
	ZCacheLock lock;

	/// 이미 캐시되어 있으면 캐시하지 않는다.
	if ( m_cacheData.count(strFilename) > 0) return;

	/// 용량을 체크해서 이 이미지를 캐시했을 때 제한을 넘어섰으면 캐시하지 않는다.
	if ( (m_lCacheSize + image.GetImageSize()) /1024/1024 > m_iMaximumCacheMemoryMB ) return;

	DebugPrintf("%s added to cache", strFilename.c_str());

	m_cacheData[strFilename] = image;
	m_lCacheSize += m_cacheData[strFilename].GetImageSize();
}

long ZCacheImage::GetCachedKByte()
{
	/*
	ZCacheLock lock;
	CacheMapIterator it, endit = m_cacheData.end();

	unsigned long total = 0;

	for ( it = m_cacheData.begin(); it != endit; ++it)
	{
		ZImage & image = it->second;

		total += image.GetImageSize();
	}
	*/

	return (m_lCacheSize/1024);
}
#pragma once

#include "../../lib/ZImage.h"

class ZCacheImage
{
private:
	ZCacheImage();

public:
	static ZCacheImage & GetInstance();

	~ZCacheImage();

	inline CRITICAL_SECTION * GetLockPtr() { return &m_lock; }

	inline void LogCacheHit() { ++m_iLogCacheHit; }
	inline void LogCacheMiss() { ++m_iLogCacheMiss; }

	long GetCachedKByte();
	int GetLogCacheHitRate()
	{
		if ( (m_iLogCacheHit + m_iLogCacheMiss ) == 0 )
		{
			return 0;
		}
		return (m_iLogCacheHit * 100 / (m_iLogCacheMiss+m_iLogCacheHit)); 
	}

	void StartThread();

	size_t GetNumOfCacheImage() const { return m_cacheData.size(); 	}

	static DWORD WINAPI ThreadFuncProxy(LPVOID p);
	void ThreadFunc();

	void SetImageVector(std::vector < std::string > & v);
	bool hasCachedData(const std::string & strFilename, int iIndex);
	void getCachedData(const std::string & strFilename, ZImage & image);
	void AddCacheData(const std::string & strFilename, ZImage & image);

private:


	/// 캐시된 데이터 용량
	long m_lCacheSize;

	/// 캐시가 hit 한 횟수
	unsigned int m_iLogCacheHit;
	/// 캐시가 miss 한 횟수
	unsigned int m_iLogCacheMiss;
	bool m_bUseCache;
	volatile bool m_bNewChange;
	CRITICAL_SECTION m_lock;

	/// 최대 캐시할 이미지 갯수. 여기서 절반반큼 왼쪽, 오른쪽으로 간다.
	const int m_iMaxCacheImageNum;

	/// 최대 캐시 용량
	const int m_iMaximumCacheMemoryMB;

	/// threadfunc 를 계속 실행시킬 것인가. 프로그램이 끝날 때 false 로 해줘야함
	bool m_bGoOn;

	/// 캐시를 좌우로 하는데, 다음 캐시할 것이 오른쪽 것인가?
	bool m_bNextCacheIsRight;

	int m_iCacheLeftIndex;
	int m_iCacheRightIndex;

	std::map < std::string, ZImage > m_cacheData;
	typedef std::map < std::string, ZImage >::iterator CacheMapIterator;

	std::vector < std::string > m_imageVector;
	std::map < int , std::string > m_imageMap;
	std::map < std::string, int > m_imageMapRev;

	/// 현재보고 있는 index;
	volatile int m_iCurrentIndex;

	/// 캐시하기 시작한 인덱스. 이 지점을 어느 정도 벗어나면 캐시를 다시 한다.
	int m_iCacheStartIndex;

	HANDLE m_hThread;
	HANDLE m_hEvent;		// 캐시 이벤트

private:
	/// 캐시된 데이터의 유효성등을 체크해서 캐시를 비워준다.
	//void CheckCacheDataAndClear();

	/// 지정된 번호의 파일을 캐시할 수 있으면 캐시한다.
	bool CacheIndex(int iIndex);

	/// 캐시되어 있는 데이터들 중 현재 인덱스로부터 가장 멀리있는 인덱스를 얻는다.
	int _GetFarthestIndexFromCurrentIndex();

private:

	/// 쉽게 CriticalSection Lock 을 걸기 위한 클래스
	class ZCacheLock
	{
	public:
		ZCacheLock()
		{
			EnterCriticalSection(ZCacheImage::GetInstance().GetLockPtr());
		}

		~ZCacheLock()
		{
			LeaveCriticalSection(ZCacheImage::GetInstance().GetLockPtr());
		}
	};

};


/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZCacheImage.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

#include "../../commonSrc/ZImage.h"
#include "../../commonSrc/LockUtil.h"

/// 캐시를 어느 방향부터 먼저할 것인지, 우선시할 것인지를 위해...
/**
 예를 들어 사용자가 PageDown 으로 다음 이미지들을 보고 있다면 다음 이미지들을 좀 더 캐쉬해놓고,
 PageUp 으로 이전 이미지 방향으로 보고 있다면 이전 이미지들을 좀 더 캐쉬해놓기 위해서이다.
*/
enum eLastActionDirection
{
	eLastActionDirection_FORWARD,	///< PageDown 등으로 다음 파일을 보았다
	eLastActionDirection_BACKWARD,	///< PageUp 등으로 이전 파일을 보았다.
};


/// 이미지를 캐쉬하여 관리하는 클래스
class ZCacheImage
{
private:
	ZCacheImage();

public:
	static ZCacheImage & GetInstance();

	~ZCacheImage();

	void CleanUp()
	{
		m_cacheData.clear();
	}

	inline void LogCacheHit() { ++m_iLogCacheHit; }
	inline void LogCacheMiss() { ++m_iLogCacheMiss; }

	/// 다음 파일이 캐쉬되었나를 체크해서 돌려준다.
	bool IsNextFileCached() const;

	void WaitCacheLock()
	{
		CLockObjUtil lock(m_cacheLock);
	}

	void debugShowCacheInfo();		///< 현재 캐쉬 정보를 디버그 콘솔에 보여준다. 디버깅모드 전용

	void clearCache();
	void setCacheEvent()
	{
		m_hCacheEvent.setEvent();
	}

	long GetCachedKByte() const;
	int GetLogCacheHitRate() const
	{
		if ( (m_iLogCacheHit + m_iLogCacheMiss ) == 0 )
		{
			return 0;
		}
		return (m_iLogCacheHit * 100 / (m_iLogCacheMiss+m_iLogCacheHit)); 
	}

	void StartThread();

	// 현재 캐시되어 있는 파일들을 output 윈도우로 뿌려준다.
	void ShowCachedImageToOutputWindow();

	size_t GetNumOfCacheImage() const { return m_cacheData.size(); 	}

	static DWORD WINAPI ThreadFuncProxy(LPVOID p);
	void ThreadFunc();

	void SetImageVector(const std::vector < FileData > & v);
	bool hasCachedData(const tstring & strFilename, int iIndex);	///< 이 파일에 해당하는 정보를 캐쉬해서 가지고 있는지 체크하는 함수
	void getCachedData(const tstring & strFilename, ZImage & image);	///< 이 파일에 해당하는 ZImage 정보를 받아오는 함수
	void AddCacheData(const tstring & strFilename, ZImage & image);		///< 이 파일에 해당하는 ZImage 정보를 새로 추가해라.

	void SetLastActionDirection(eLastActionDirection last)
	{
		m_lastActionDirection = last;
	}

	bool isCachingNow() const { return m_bNowCaching; }

private:

	/// 현재 캐쉬 중인가...
	bool m_bNowCaching;

	/// 캐쉬의 효율성을 위해서 사용자가 마지막으로 어느 방향으로 움직였는지를 기억해놓는다.
	eLastActionDirection m_lastActionDirection;

	/// 캐시된 데이터 용량
	long m_lCacheSize;

	/// 캐시가 hit 한 횟수. 통계용
	unsigned int m_iLogCacheHit;

	/// 캐시가 miss 한 횟수. 통계용
	unsigned int m_iLogCacheMiss;

	volatile bool m_bNewChange;

	mutable CLockObj m_cacheLock;

	/// threadfunc 를 계속 실행시킬 것인가. 프로그램이 끝날 때 false 로 해주면 캐쉬 쓰레드를 최대한 빨리 끝낼 때 쓰임
	volatile bool m_bCacheGoOn;

	std::map < tstring, ZImage > m_cacheData;		///< 실제로 캐쉬된 데이터를 가지고 있는 맵
	typedef std::map < tstring, ZImage >::iterator CacheMapIterator;

	size_t m_numImageVectorSize;

	std::map < int , tstring > m_imageIndex2FilenameMap;	///< 이미지 파일의 인덱스 번호,파일이름 맵
	std::map < tstring, int > m_imageFilename2IndexMap;		///< 이미지 파일이름,인덱스 번호 맵

	/// 현재보고 있는 index;
	volatile int m_iCurrentIndex;

	/// 현재보고 있는 파일이름
	tstring m_strCurrentFileName;

	HANDLE m_hThread;			///< 캐쉬 쓰레드를 가르키는 핸들
	
	/// 캐시 이벤트
	CEventObj m_hCacheEvent;

	/// 지정된 번호의 파일을 캐시할 수 있으면 캐시한다.
	bool _CacheIndex(int iIndex);

	/// 캐시되어 있는 데이터들 중 현재 인덱스로부터 가장 멀리있는 인덱스를 얻는다.
	int _GetFarthestIndexFromCurrentIndex();

	/*
	/// 현재 캐시 쓰레드에서 읽고 있는 파일이름
	tstring m_cacheThreadReadingFileName;
	CLockObj m_cacheThreadReadingFileNameLock;	///< m_cacheThreadReadingFileName 를 위한 lock 객체

	/// 현재 메인 쓰레드에서 읽고 있는 파일이름
	tstring m_mainThreadReadingFileName;
	CLockObj m_mainThreadReadingFileNameLock;	///< m_mainThreadReadingFileName 를 위한 lock 객체
	*/

};


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

class CachedData;
class ZImage;

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

	void CleanUpCache();
	void CleanUpThread();

	inline void LogCacheHit() { ++m_iLogCacheHit; }
	inline void LogCacheMiss() { ++m_iLogCacheMiss; }

	/// 다음 파일이 캐쉬되었나를 체크해서 돌려준다.
	bool IsNextFileCached() const;

	void WaitCacheLock();

	void debugShowCacheInfo();		///< 현재 캐쉬 정보를 디버그 콘솔에 보여준다. 디버깅모드 전용

	void clearCache();
	void setCacheEvent();

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

	size_t GetNumOfCacheImage() const;

	void ThreadFunc();

	void SetImageVector(const std::vector < FileData > & v);
	bool hasCachedData(const tstring & strFilename, int iIndex);	///< 이 파일에 해당하는 정보를 캐쉬해서 가지고 있는지 체크하는 함수
	std::shared_ptr<ZImage> GetCachedData(const tstring& strFilename) const;	///< 이 파일에 해당하는 ZImage 정보를 받아오는 함수
	void AddCacheData(const tstring & strFilename, std::shared_ptr<ZImage> image, bool bForceCache = false);		///< 이 파일에 해당하는 ZImage 정보를 새로 추가해라.

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

	/// 캐시가 hit 한 횟수. 통계용
	unsigned int m_iLogCacheHit;

	/// 캐시가 miss 한 횟수. 통계용
	unsigned int m_iLogCacheMiss;

	volatile bool m_bNewChange;

	/// threadfunc 를 계속 실행시킬 것인가. 프로그램이 끝날 때 false 로 해주면 캐쉬 쓰레드를 최대한 빨리 끝낼 때 쓰임
	volatile bool m_bCacheGoOn;

	/// 현재보고 있는 index;
	volatile int m_iCurrentIndex;

	/// 현재보고 있는 파일이름
	tstring m_strCurrentFileName;

	std::thread m_thread;
	
	/// 지정된 번호의 파일을 캐시할 수 있으면 캐시한다. 반환값은 캐쉬성공이면 true
	bool _CacheIndex(int iIndex);

  bool ClearFarthestCache(const int index);

	class Impl;
	std::unique_ptr<Impl> m_pImpl;
};

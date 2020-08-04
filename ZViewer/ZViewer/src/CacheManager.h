#pragma once

namespace ZViewer {
class CacheEventListenerInterface;
} // namespace ZViewer

class EventManager;
class ZImage;
class DecodeThread;

/// 캐시를 어느 방향부터 먼저할 것인지, 우선시할 것인지를 위해...
/**
 예를 들어 사용자가 PageDown 으로 다음 이미지들을 보고 있다면 다음 이미지들을 좀 더 캐쉬해놓고,
 PageUp 으로 이전 이미지 방향으로 보고 있다면 이전 이미지들을 좀 더 캐쉬해놓기 위해서이다.
*/
enum class ViewDirection {
	kForward,	///< PageDown 등으로 다음 파일을 보았다
	kBackward,	///< PageUp 등으로 이전 파일을 보았다.
};

/// 이미지를 캐쉬하여 관리하는 클래스
class CacheManager final : NonCopyable {
public:
	static CacheManager& GetInstance();

	~CacheManager();

	void set_listener(ZViewer::CacheEventListenerInterface* listener) { listener_ = listener; }

	void CleanUpCache();
	void CleanUpThread();

	inline void IncreaseCacheHitCounter() { ++cache_hit_counter_; }
	inline void IncreaseCacheMissCounter() { ++cache_miss_counter_; }

	bool IsNextFileCached() const;

	void DebugShowCacheInfo();

	void ClearCache();
	void SetCacheEvent();

	long GetCachedKByte() const;
	int GetCacheHitRate() const;

	void StartCacheThread();

	void ShowCachedImageList();

	size_t GetNumOfCacheImage() const;

	void ThreadFunc();

	void SetFilelist(const std::vector<FileData>& filelist);

	void SetCurrent(const int index, const tstring& filename);
	bool HasCachedData(const tstring& strFilename);

	std::shared_ptr<ZImage> GetCachedData(const tstring& strFilename) const;	///< 이 파일에 해당하는 ZImage 정보를 받아오는 함수
	void AddCacheData(const tstring& strFilename, std::shared_ptr<ZImage> image, bool bForceCache = false);		///< 이 파일에 해당하는 ZImage 정보를 새로 추가해라.

	void OnDecodeCompleted(const tstring& filename, std::shared_ptr<ZImage> image);

	void set_view_direction(const ViewDirection direction) {
		view_direction_ = direction;
	}

	bool is_cacahing() const { return is_caching_; }

private:
	CacheManager();

	ZViewer::CacheEventListenerInterface* listener_ = nullptr;

	bool is_caching_;

	/// 캐쉬의 효율성을 위해서 사용자가 마지막으로 어느 방향으로 움직였는지를 기억해놓는다.
	ViewDirection view_direction_;

	unsigned int cache_hit_counter_;
	unsigned int cache_miss_counter_;

	volatile bool is_new_change_;

	/// threadfunc 를 계속 실행시킬 것인가. 프로그램이 끝날 때 false 로 해주면 캐쉬 쓰레드를 최대한 빨리 끝낼 때 쓰임
	volatile bool m_bCacheGoOn;

	volatile int current_index_;

	tstring current_filename_;

	std::thread cache_thread_;

	void CacheImageByIndex(int iIndex);

	bool ClearFarthestCache(const int index);

	class Impl;
	std::unique_ptr<Impl> impl_;

	std::unique_ptr<EventManager> event_manager_;
};

void CacheManagerSpeedTest();


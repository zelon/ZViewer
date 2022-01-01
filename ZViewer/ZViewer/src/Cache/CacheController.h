#pragma once

#include "../commonSrc/LockUtil.h"
#include "ImageLoadCallback.h"

class ParallelImageLoader;
class ZImage;
enum class RequestType;

class CacheController final {
public:
	static CacheController& GetInstance();

	~CacheController ();

	void RequestLoadImage (const tstring& filename, const RequestType request_type, const int32_t index);
	std::shared_ptr<ZImage> PickImage (const tstring& filename);

	void EmptyFarthestCache ();

	int32_t GetCachingCount () const;
	int64_t cache_hit_count () const;
	int64_t cache_miss_count () const;
	size_t GetCachedCount () const;
	int64_t GetCachedKBytes () const;
	std::vector<std::wstring> ToString () const;

	void Shutdown ();

private:
	CacheController();

	struct CachedImageInfo {
		std::shared_ptr<ZImage> image;
		int32_t index = 0;
	};

	mutable Lock lock_;
	std::unordered_map<tstring, CachedImageInfo> cached_images_;

	std::unique_ptr<ParallelImageLoader> parallel_image_loader_;

	std::atomic<int32_t> caching_count_ = {};
	std::atomic<int64_t> cache_hit_count_ = {};
	std::atomic<int64_t> cache_miss_count_ = {};

	int32_t current_index_ = 0;
};


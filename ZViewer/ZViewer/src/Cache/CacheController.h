#pragma once

#include "../commonSrc/LockUtil.h"
#include "ImageLoadCallback.h"

class IImageLoadCallback;
class ParallelImageLoader;
class ZImage;

class CacheController final {
public:
	static CacheController& GetInstance();

	~CacheController ();

	void RequestLoadImage (const tstring& filename, ImageLoadCallback callback);
	std::shared_ptr<ZImage> PickImage (const tstring& filename);

	void EmptyOldestCache ();

	int32_t GetCachingCount () const;

private:
	CacheController();

	struct CachedImageInfo {
		std::shared_ptr<ZImage> image;
		int64_t operation_id = 0;
	};

	Lock lock_;
	std::unordered_map<tstring, CachedImageInfo> cached_images_;

	std::unique_ptr<ParallelImageLoader> parallel_image_loader_;

	std::atomic<int32_t> caching_count_ = {};
};


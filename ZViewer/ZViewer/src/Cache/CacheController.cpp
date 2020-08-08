#include "stdafx.h"
#include "CacheController.h"

#include "ParallelImageLoader.h"
#include "../commonSrc/CommonFunc.h"

static std::atomic<int64_t> operation_id_factory = {};

CacheController& CacheController::GetInstance () {
	static CacheController cache_controller;
	return cache_controller;
}

CacheController::CacheController ()
		: parallel_image_loader_(std::make_unique<ParallelImageLoader>(/*thread_count:*/10)) {

}

CacheController::~CacheController() {

}

void CacheController::RequestLoadImage (const tstring& filename, ImageLoadCallback callback) {
	const int64_t operation_id = operation_id_factory.fetch_add(1);
	LockGuard lock_guard(lock_);
	auto it = cached_images_.find(filename);
	if (it != cached_images_.end()) {
		// for cache holding
		it->second.operation_id = operation_id;
		callback(filename, it->second.image);
		return;
	}
	caching_count_.fetch_add(1);
	parallel_image_loader_->Load(filename, [callback, operation_id](const tstring& filename, const std::shared_ptr<ZImage>& image) {
		DebugPrintf(std::wstring(L"CacheController LoadCompleted:") + filename);

		auto& self = CacheController::GetInstance();
		self.caching_count_.fetch_sub(1);
		LockGuard lock_guard(self.lock_);
		auto it = self.cached_images_.find(filename);
		if (it != self.cached_images_.end()) {
			// for cache holding
			it->second.operation_id = operation_id;
		} else {
			CachedImageInfo cache_image_info;
			cache_image_info.image = image;
			cache_image_info.operation_id = operation_id;
			self.cached_images_.emplace(filename, cache_image_info);
		}
		callback(filename, image);
	});

}

std::shared_ptr<ZImage> CacheController::PickImage (const tstring& filename) {
	LockGuard lock_guard(lock_);
	auto it = cached_images_.find(filename);
	if (it == cached_images_.end()) {
		return nullptr;
	}
	// cache hitted
	const int64_t operation_id = operation_id_factory.fetch_add(1);
	it->second.operation_id = operation_id;
	return it->second.image;
}

void CacheController::EmptyOldestCache () {
	LockGuard lock_guard(lock_);
	if (cached_images_.size() < 10) {
		return;
	}
	int64_t oldest_operation_id = std::numeric_limits<int64_t>::max();
	tstring oldest_filename;

	for (const auto& [filename, cache_info] : cached_images_) {
		if (cache_info.operation_id < oldest_operation_id) {
			oldest_operation_id = cache_info.operation_id;
			oldest_filename = filename;
		}
	}
	assert(oldest_filename.empty() == false);
	cached_images_.erase(oldest_filename);
}

int32_t CacheController::GetCachingCount () const {
	return caching_count_;
}
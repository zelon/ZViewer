#include "stdafx.h"
#include "CacheController.h"

#include "ParallelImageLoader.h"
#include "../commonSrc/CommonFunc.h"
#include "../commonSrc/ZImage.h"

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

void CacheController::RequestLoadImage (const tstring& filename, const RequestType request_type, const int32_t index) {
	if (request_type == RequestType::kCurrent) {
		parallel_image_loader_->set_current_index(index);
	}
	const int64_t operation_id = operation_id_factory.fetch_add(1);
	LockGuard lock_guard(lock_);
	auto it = cached_images_.find(filename);
	if (it != cached_images_.end()) {
		// for cache holding
		it->second.operation_id = operation_id;

		if (request_type == RequestType::kCurrent) {
			++cache_hit_count_;
		}
		return;
	}
	if (request_type == RequestType::kCurrent) {
		++cache_miss_count_;
	}
	++caching_count_;
	parallel_image_loader_->Load(filename, request_type, index, [operation_id](const tstring& filename, const std::shared_ptr<ZImage>& image) {
		DebugPrintf(fmt::format(L"CacheController LoadCompleted,filename:{},completed:{}",
			filename, image != nullptr));

		auto& self = CacheController::GetInstance();
		--(self.caching_count_);

		if (image == nullptr) {
			return;
		}
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

int64_t CacheController::cache_hit_count () const {
	return cache_hit_count_;
}

int64_t CacheController::cache_miss_count () const {
	return cache_miss_count_;
}

size_t CacheController::GetCachedCount () const {
	LockGuard lock_guard(lock_);
	return cached_images_.size();
}

int64_t CacheController::GetCachedKBytes () const {
	LockGuard lock_guard(lock_);
	int64_t bytes = 0;
	for (const auto& [_, cached_image_info] : cached_images_) {
		bytes += cached_image_info.image->GetImageSize();
	}
	return bytes / 1024;
}

std::vector<tstring> CacheController::ToString() const {
	std::vector<tstring> output;
	LockGuard lock_guard(lock_);

	output.emplace_back(fmt::format(L"caching_count: {}", caching_count_));
	output.emplace_back(fmt::format(L"cache_hit_count: {}", cache_hit_count_));
	output.emplace_back(fmt::format(L"cache_miss_count: {}", cache_miss_count_));
	output.emplace_back(fmt::format(L"cached_count: {}", cached_images_.size()));
	output.emplace_back(fmt::format(L"cached_bytes: {}KB", GetCachedKBytes()));
	for (const auto& [filename, info] : cached_images_) {
		output.emplace_back(fmt::format(L"cached_info:{{filename:{},bytes:{},index:{}}}",
			filename, info.image->GetImageSize(), info.operation_id));
	}

	return output;
}

void CacheController::Shutdown () {
	parallel_image_loader_.reset();
}
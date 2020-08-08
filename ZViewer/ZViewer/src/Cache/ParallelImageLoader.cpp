#include "stdafx.h"
#include "ParallelImageLoader.h"

#include "../src/Defines.h"
#include "Enums.h"
#include "FileReader.h"
#include "ZImage.h"

static bool IsValidIndex (const int32_t index, const int32_t current_index) {
	if (std::abs(index - current_index) <= kMaxOnewayPreCacheCount) {
		return true;
	}
	return false;
}

void ParallelImageLoader::LoadPerThread (const tstring& filename, const int32_t index, ImageLoadCallback callback) {
	FileReader file(filename);

	if (file.IsOpened() == false) {
		callback(filename, nullptr);
		return;
	}

	static const size_t kReadBufferSize = 1024 * 10;
	BYTE readBuffer[kReadBufferSize];
	BOOL bReadOK = TRUE;
	std::vector<BYTE> file_read_buffer;
	while (bReadOK) {
		if (IsValidIndex(index, current_index_) == false) {
			callback(filename, nullptr);
			return;
		}
		DWORD dwReadBytes = 0;
		bReadOK = file.Read(readBuffer, kReadBufferSize, &dwReadBytes);
		if (FALSE == bReadOK) {
			callback(filename, nullptr);
			return;
		}
		else
		{
			if (dwReadBytes <= 0) {///< 파일의 끝까지 읽었다.
				break;
			}
			else {
				file_read_buffer.resize(file_read_buffer.size() + dwReadBytes);
				memcpy((&(file_read_buffer[0])) + file_read_buffer.size() - dwReadBytes, readBuffer, dwReadBytes);
			}
		}
	}

	static const size_t kMinimumFileSize = 5;
	if (file_read_buffer.size() < kMinimumFileSize) {
		/// Too small image file
		callback(filename, nullptr);
		return;
	}
	fipMemoryIO mem(&file_read_buffer[0], (DWORD)file_read_buffer.size());
	std::shared_ptr<ZImage> image = std::make_shared<ZImage>();
	if (IsValidIndex(index, current_index_) == false) {
		callback(filename, nullptr);
		return;
	}
	const bool load_ok = image->LoadFromMemory(mem);
	if (load_ok == false) {
		callback(filename, nullptr);
		return;
	}
	callback(filename, image);
}

ParallelImageLoader::ParallelImageLoader (const int32_t thread_count) {
	for (int i = 0; i < thread_count; ++i) {
		threads_.emplace_back(std::thread([this]() {
			while (go_on_) {
				std::optional<Request> request;
				{
					LockGuard lock_guard(lock_);
					if (requests_.empty() == false) {
						request = requests_.front();
						requests_.pop_front();
					}
				}
				if (request.has_value() == false) {
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					continue;
				}
				if (std::abs(request.value().index - current_index_) > kMaxOnewayPreCacheCount) {
					request.value().callback(request.value().filename, nullptr);
					continue;
				}
				LoadPerThread(request.value().filename, request.value().index, request.value().callback);
			}
		}));
	}
}

ParallelImageLoader::~ParallelImageLoader () {
	{
		LockGuard lock_guard(lock_);
		requests_.clear();
	}
	go_on_ = false;
	for (auto& thread : threads_) {
		try {
			thread.join();
		} catch (std::exception& ) {
			continue;
		}
	}
}

void ParallelImageLoader::set_current_index (const int32_t index) {
	current_index_ = index;
}

void ParallelImageLoader::Load (const tstring& filename,
		const RequestType request_type,
		const int32_t index,
		ImageLoadCallback callback) {
	LockGuard lock_guard(lock_);

	Request request;
	request.filename = filename;
	request.index = index;
	request.callback = callback;

	if (request_type == RequestType::kCurrent) {
		current_index_ = index;
		requests_.emplace_front(std::move(request));
	} else if (request_type == RequestType::kPreCache) {
		if (IsValidIndex(index, current_index_)) {
			requests_.emplace_front(std::move(request));
		} else {
			callback(filename, nullptr);
		}
	} else {
		assert(false);
	}
}

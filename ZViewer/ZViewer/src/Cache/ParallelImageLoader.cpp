#include "stdafx.h"
#include "ParallelImageLoader.h"

#include "Enums.h"
#include "FileReader.h"
#include "ZImage.h"

static void LoadPerThread (const tstring& filename, ImageLoadCallback callback) {
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
	const bool load_ok = image->LoadFromMemory(mem);
	if (load_ok == false) {
		callback(filename, nullptr);
		return;
	}
	callback(filename, image);
}

ParallelImageLoader::ParallelImageLoader (const int32_t thread_count) {
	for (int i = 0; i < thread_count; ++i) {
		threads_.emplace_back(std::thread([&]() {
			while (go_on_) {
				std::pair<tstring, ImageLoadCallback> request;
				{
					LockGuard lock_guard(lock_);
					if (requests_.empty() == false) {
						request = requests_.front();
						requests_.pop_front();
					}
				}
				if (request.second == nullptr) {
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					continue;
				}
				LoadPerThread(request.first, request.second);
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

void ParallelImageLoader::Load (const tstring& filename,
		const RequestType request_type,
		const int32_t index,
		ImageLoadCallback callback) {
	LockGuard lock_guard(lock_);
	if (request_type == RequestType::kCurrent) {
		current_index_ = index;
		requests_.emplace_front(filename, callback);
	} else if (request_type == RequestType::kPreCache) {
		if (std::abs(index - current_index_) < 5) {
			requests_.emplace_front(filename, callback);
		} else {
			requests_.emplace_back(filename, callback);
		}
	} else {
		assert(false);
	}
}

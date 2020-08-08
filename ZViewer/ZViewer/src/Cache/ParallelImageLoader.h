#pragma once

#include "Enums.h"
#include "../commonSrc/LockUtil.h"
#include "ImageLoadCallback.h"

class ZImage;
enum class RequestType;

class ParallelImageLoader final {
public:
	ParallelImageLoader (const int32_t thread_count);
	~ParallelImageLoader ();

	void Load (const tstring& filename,
		const RequestType request_type,
		const int32_t index,
		ImageLoadCallback callback);

private:
	int32_t current_index_ = 0;

	Lock lock_;
	std::list<std::pair<tstring, ImageLoadCallback>> requests_;

	std::vector<std::thread> threads_;
	bool go_on_ = true;
};

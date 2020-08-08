#pragma once

#include "Enums.h"
#include "../commonSrc/LockUtil.h"
#include "ImageLoadCallback.h"

class ZImage;

class ParallelImageLoader final {
public:
	ParallelImageLoader (const int32_t thread_count);
	~ParallelImageLoader ();

	void Load(const tstring& filename, ImageLoadCallback callback);

private:
	Lock lock_;
	std::list<std::pair<tstring, ImageLoadCallback>> requests_;

	std::vector<std::thread> threads_;
	bool go_on_ = true;
};

#pragma once

#include "Enums.h"

class CacheModel final {
public:
	CacheModel();
	~CacheModel();

private:
	ViewDirection view_direction_ = ViewDirection::kForward;

	//std::unordered<std::string, std::shared_ptr<ZImage>> cached_
};


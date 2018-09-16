#pragma once

namespace ZViewer {

class CacheEventListenerInterface {
public:
	virtual ~CacheEventListenerInterface() { /* do nothing */ }

	virtual void OnFileCached() = 0;
};

} // namespace ZViewer

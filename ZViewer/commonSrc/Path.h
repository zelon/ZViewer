#pragma once

namespace ZViewer {

class Path final {
public:
	static bool CreateDirectory(const std::wstring& directory_name);
	static std::wstring GetZViewerSettingDirectory();

};
} // namespace ZViewer

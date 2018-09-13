#include "stdafx.h"
#include "Path.h"

namespace ZViewer {

bool Path::CreateDirectory(const std::wstring& directory_name) {
	return SHCreateDirectory(NULL, directory_name.c_str()) == ERROR_SUCCESS;
}

std::wstring Path::GetZViewerSettingDirectory() {
	TCHAR szSystemFolder[_MAX_PATH] = { 0 };

	if (SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szSystemFolder) == E_FAIL) {
		assert(false);
		return {};
	}
	return std::wstring(szSystemFolder) + std::wstring(L"\\ZViewer");
}

} // namespace ZViewer

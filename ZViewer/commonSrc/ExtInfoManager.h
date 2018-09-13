#pragma once

#include "CommonDefine.h"

/// 파일 확장자에 대한 정보를 관리하는 클래스
class ExtInfoManager final
{
public:
	static ExtInfoManager& GetInstance();


	bool IsValidImageFileExt(const TCHAR * szFilename);

	const TCHAR * GetFileDlgFilter();

	const std::vector<ExtSetting>& ext_settings () const;

private:
	ExtInfoManager();

	/// 확장자를 연결
	void ExtMapInit();

	void AddExtSet(const int iIconIndex, const TCHAR * ext);

	tstring m_strFileDlgFilter;
	std::vector<ExtSetting> m_extConnect;
};
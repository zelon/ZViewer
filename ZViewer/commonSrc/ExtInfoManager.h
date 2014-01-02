/* ------------------------------------------------------------------------
 *
 * 2008. 8. 10 KIM JINWOOK
 *
 * ------------------------------------------------------------------------
 */

#pragma once

/// 파일 확장자에 대한 정보를 관리하는 클래스
class ExtInfoManager
{
public:
	static ExtInfoManager & GetInstance();

	std::vector < ExtSetting > m_extConnect;

	bool IsValidImageFileExt(const TCHAR * szFilename);

	const TCHAR * GetFileDlgFilter();

protected:
	ExtInfoManager();

	/// 확장자를 연결
	void ExtMapInit();

	void _AddExtSet(const int iIconIndex, const TCHAR * ext);

	tstring m_strFileDlgFilter;

};
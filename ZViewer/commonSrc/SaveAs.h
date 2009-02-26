/* ------------------------------------------------------------------------
 *
 * Copyright 2006 by zelon
 *
 * s:\ZViewer\ZViewer\src\SaveAs.h
 *
 * 2006. 2. 4 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#pragma once

/// 다른 이름으로 저장을 했을 때 사용되는 클래스
class CSaveAs
{
    CSaveAs();

public:
    static CSaveAs & getInstance();
    ~CSaveAs();

	/// 부모 윈도우의 핸들을 정해준다.
	void setParentHWND(const HWND hwnd)
	{
		m_hParent = hwnd;
	}

	void setDefaultSaveFilename(const tstring & strInitialiFolder, const tstring & strFilename)
	{
		m_strInitialiFolder = strInitialiFolder;
		m_strSaveFileName = strFilename;
	}

	/// 다른 이름으로 저장창을 띄운다. 반환값이 false 이면 저장하지 않는다.
	bool showDialog();

	const TCHAR * getSaveFileName() const
	{
		return m_szFilenamebuf;
	}

protected:

	OPENFILENAME m_ofn;

	HWND m_hParent;
	tstring m_strInitialiFolder;
	tstring m_strSaveFileName;

	TCHAR m_szFilenamebuf[FILENAME_MAX];
};

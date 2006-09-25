/* ------------------------------------------------------------------------
 *
 * Copyright 2006 by zelon
 *
 * s:\ZViewer\ZViewer\src\SaveAs.cpp
 *
 * 2006. 2. 4 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"
#include "SaveAs.h"

CSaveAs & CSaveAs::getInstance()
{
    static CSaveAs inst;
    return inst;
}

CSaveAs::CSaveAs()
{
	m_hParent = 0;
}

CSaveAs::~CSaveAs()
{
}

/// 다른 이름으로 저장창을 띄운다. 반환값이 false 이면 저장하지 않는다.
bool CSaveAs::showDialog()
{
	// Initialize OPENFILENAME
	ZeroMemory(&m_ofn, sizeof(m_ofn));
	m_ofn.lStructSize = sizeof(m_ofn);
	m_ofn.hwndOwner = m_hParent;
	StringCchPrintf(m_szFilenamebuf, sizeof(m_szFilenamebuf), m_strSaveFileName.c_str());
	m_ofn.lpstrFile = m_szFilenamebuf;
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	//
	m_ofn.lpstrFile[0] = '\0';
	m_ofn.nMaxFile = FILENAME_MAX;
	m_ofn.lpstrFilter = TEXT("ImageFiles(jpg,gif,png,bmp,psd,tga,tif,ico)\0*.jpg;*.jpeg;*.gif;*.png;*.bmp;*.psd,*.tga;*.tif;*.ico\0All(*.*)\0*.*\0");
	m_ofn.nFilterIndex = 1;
	m_ofn.lpstrFileTitle = NULL;
	m_ofn.nMaxFileTitle = 0;
	m_ofn.lpstrInitialDir = m_strInitialiFolder.c_str();
	m_ofn.Flags = OFN_OVERWRITEPROMPT;

	if ( FALSE == GetSaveFileName(&m_ofn) )
	{
		return false;
	}

	return true;
}

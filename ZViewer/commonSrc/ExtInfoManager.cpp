/* ------------------------------------------------------------------------
 *
 * 2008. 8. 10 KIM JINWOOK
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"

#include "ExtInfoManager.h"
#include "CommonFunc.h"


ExtInfoManager & ExtInfoManager::GetInstance()
{
	static ExtInfoManager inst;
	return inst;
}

ExtInfoManager::ExtInfoManager()
{
	ExtMapInit();
}

void ExtInfoManager::_AddExtSet(const int iIconIndex, const TCHAR * ext)
{
	ExtSetting extSet;

	extSet.m_numIconIndex = iIconIndex;
	extSet.m_strExt = ext;
	m_extConnect.push_back(extSet);
}

void ExtInfoManager::ExtMapInit()
{
	_AddExtSet(1, TEXT("bmp"));
	_AddExtSet(1, TEXT("wbmp"));
	_AddExtSet(2, TEXT("jpg"));
	_AddExtSet(2, TEXT("jpeg"));
	_AddExtSet(2, TEXT("jpe"));
	_AddExtSet(2, TEXT("jp2"));
	_AddExtSet(2, TEXT("j2k"));
	_AddExtSet(3, TEXT("png"));
	_AddExtSet(4, TEXT("psd"));
	_AddExtSet(5, TEXT("gif"));
	_AddExtSet(0, TEXT("dds"));
	_AddExtSet(0, TEXT("tga"));
	_AddExtSet(0, TEXT("pcx"));
	_AddExtSet(0, TEXT("xpm"));
	_AddExtSet(0, TEXT("xbm"));
	_AddExtSet(0, TEXT("tif"));
	_AddExtSet(0, TEXT("tiff"));
	_AddExtSet(0, TEXT("cut"));
	_AddExtSet(6, TEXT("ico"));
	_AddExtSet(0, TEXT("hdr"));
	_AddExtSet(0, TEXT("jng"));
	_AddExtSet(0, TEXT("koa"));
	_AddExtSet(0, TEXT("mng"));
	_AddExtSet(0, TEXT("pcd"));
	_AddExtSet(0, TEXT("ras"));
}


bool ExtInfoManager::IsValidImageFileExt(const TCHAR * szFilename)
{
	TCHAR szExt[MAX_PATH];

	SplitPath(szFilename, NULL,0, NULL,0 , NULL,0 , szExt,MAX_PATH);

	if ( _tcslen(szExt) < 2 ) return false;

	TCHAR szLowerExt[MAX_PATH];

	_tcsncpy_s(szLowerExt, szExt, min(_tcslen(szExt)+1, MAX_PATH));

	_tcslwr_s(szLowerExt);

	const TCHAR * szExtOnly = szLowerExt + 1;

	std::vector < ExtSetting >::iterator it, endit = m_extConnect.end();

	bool bFound = false;
	for ( it = m_extConnect.begin(); it != endit; ++it )
	{
		if ( _tcscmp(szExtOnly, it->m_strExt.c_str()) == 0 )
		{
			bFound = true;
		}
	}

	return bFound;
}


const TCHAR * ExtInfoManager::GetFileDlgFilter()
{
	// 오른쪽과 같은 형식의 문자열을 만들어 반환한다. g_strFileFilter TEXT("ImageFiles(jpg,jpeg2000,gif,png,bmp,wbmp,psd,tga,tif,ico)\0*.jpg;*.jpeg;*.jp2;*.j2k;*.gif;*.png;*.bmp;*.wbmp;*.psd,*.tga;*.tif;*.ico\0All(*.*)\0*.*\0")	
	// \0 을 넣기 위해 일단 # 을 넣고 나중에 치환한다.

	tstring result;

	result = TEXT("ImageFiles(");

	size_t i = 0;
	for ( i=0; i<m_extConnect.size(); ++i )
	{
		result += m_extConnect[i].m_strExt;
		
		if ( i < m_extConnect.size()-1 )	///< 마지막 확장자 뒤에는 , 가 없음
		{
			result += TEXT(",");
		}
	}

	result += TEXT(")#");

	for ( i=0; i<m_extConnect.size(); ++i )
	{
		result += TEXT("*.");
		result += m_extConnect[i].m_strExt;
		
		if ( i < m_extConnect.size()-1 )	///< 마지막 확장자 뒤에는 ; 가 없음
		{
			result += TEXT(";");
		}
	}

	result += TEXT("#All(*.*)#*.*#");

	for ( i=0; i<result.size(); ++i )	///< # 을 \0 으로 교체. \0 을 바로 더하기 어려워서...
	{
		if ( result[i] == TEXT('#') )
		{
			result[i] = TEXT('\0');
		}
	}

	m_strFileDlgFilter = result;

	return m_strFileDlgFilter.c_str();
}
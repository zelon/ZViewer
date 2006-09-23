/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZFileExtReg.cpp
*
*                                       http://www.wimy.com
*********************************************************************/
#include "stdafx.h"

#include "ZFileExtReg.H"
#include "CommonFunc.h"

bool ZFileExtReg::SetRegistries()
{
	RegSetExtension();
	RegSetDocumentType();

	return true;
}

bool ZFileExtReg::RegSetExtension()
{
	if( m_strExtension.empty() )
	{
		_ASSERTE(!"Extension string is empty!");
		return false;
	}

	tstring strKey = TEXT(".");
	strKey += m_strExtension;

	SetRegistryValue(HKEY_CLASSES_ROOT, strKey.c_str(), TEXT(""), m_strDocumentClassName.c_str());

	if( !m_strShellOpenCommand.empty() )
	{
		strKey += TEXT("\\shell\\open\\command");
		SetRegistryValue(HKEY_CLASSES_ROOT, strKey.c_str(), TEXT(""), m_strShellOpenCommand.c_str());
	}

	// 확장자에 따른 기본 프로그램을 정한다.
	strKey = TEXT("SoftWare\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.");
	strKey += m_strExtension;
	SetRegistryValue(HKEY_CURRENT_USER, strKey.c_str(), TEXT("ProgID"), m_strDocumentClassName.c_str());

	return TRUE;
}

bool  ZFileExtReg::RegSetDocumentType()
{
	if( m_strDocumentClassName.empty())
	{
		_ASSERTE(!"DocumentClassName string is empty!");
		return false;
	}

	tstring strKey = m_strDocumentClassName;

	SetRegistryValue(HKEY_CLASSES_ROOT, strKey.c_str(), TEXT(""), m_strDocumentDescription.c_str());

	// 기본 아이콘 등록
	if( !m_strDocumentDefaultIcon.empty() )
	{
		strKey  = m_strDocumentClassName;
		strKey += TEXT("\\DefaultIcon");
		SetRegistryValue(HKEY_CLASSES_ROOT, strKey.c_str(), TEXT(""), m_strDocumentDefaultIcon.c_str());
	}

	// 더블 클릭했을 때 실행 시킬 명령
	if( !m_strShellOpenCommand.empty() )
	{
		strKey  = m_strDocumentClassName;
		strKey += TEXT("\\shell\\open\\command");
		SetRegistryValue(HKEY_CLASSES_ROOT, strKey.c_str(), TEXT(""), m_strShellOpenCommand.c_str());
	}

	return true;
}

void ZFileExtReg::Clear()
{
	m_strExtension.resize(0);
	m_strContentType.resize(0);
	m_strShellOpenCommand.resize(0);
	m_strShellNewCommand.resize(0);
	m_strShellNewFileName.resize(0);

	m_strDocumentClassName.resize(0);
	m_strDocumentDescription.resize(0);
	m_strDocumentCLSID.resize(0);
	m_strDocumentCurrentVersion.resize(0);
	m_strDocumentDefaultIcon.resize(0);
	m_strDocumentShellOpenCommand.resize(0);
}
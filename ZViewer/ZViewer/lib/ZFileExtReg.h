/*
 이 코드는 http://www.codeproject.com/shell/cgfiletype.asp 을 참조했습니다.
*/


#pragma once

#include <windows.h>
#include <string>

class ZFileExtReg
{
public:

	ZFileExtReg()
	{
		Clear();
	}
	void Clear();

	bool SetRegistries();

	bool RegSetExtension();			/// HKEY_CLASSES_ROOT\.<Extension> 를 등록한다.
	bool RegSetDocumentType();		/// DocumentType 을 등록한다.

	bool SetRegistryValue(HKEY hOpenKey, const std::string & strKey,LPCTSTR szValue, const std::string & strData);

	std::string		m_strExtension;
	std::string		m_strContentType;
	std::string		m_strShellOpenCommand;
	std::string		m_strShellNewCommand;
	std::string		m_strShellNewFileName;

	std::string		m_strDocumentClassName;
	std::string		m_strDocumentDescription;
	std::string		m_strDocumentCLSID;
	std::string		m_strDocumentCurrentVersion;
	std::string		m_strDocumentDefaultIcon;
	std::string		m_strDocumentShellOpenCommand;

};

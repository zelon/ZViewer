/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZFileExtReg.h
*
*                                       http://www.wimy.com
*********************************************************************/
/*
 이 코드는 http://www.codeproject.com/shell/cgfiletype.asp 을 참조했습니다.
*/


#pragma once

#include <windows.h>
#include <string>

/// 확장자 아이콘, 더블 클릭시 실행에 관한 내용을 레지스트리에 등록하는 클래스
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

	tstring		m_strExtension;
	tstring		m_strContentType;
	tstring		m_strShellOpenCommand;
	tstring		m_strShellNewCommand;
	tstring		m_strShellNewFileName;

	tstring		m_strDocumentClassName;
	tstring		m_strDocumentDescription;
	tstring		m_strDocumentCLSID;
	tstring		m_strDocumentCurrentVersion;
	tstring		m_strDocumentDefaultIcon;
	tstring		m_strDocumentShellOpenCommand;

};

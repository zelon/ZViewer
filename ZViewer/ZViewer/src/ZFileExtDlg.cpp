/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZFileExtDlg.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "ZFileExtDlg.h"
#include "ZMain.h"
#include "ZResourceManager.h"
#include <ShlObj.h>

#include "ZFileExtReg.h"

ZFileExtDlg * pThis = NULL;

ZFileExtDlg::ZFileExtDlg()
{
	pThis = this;

	ExtMapInit();
}

ZFileExtDlg & ZFileExtDlg::GetInstance()
{
	static ZFileExtDlg a;
	return a;
}

void ZFileExtDlg::ShowDlg()
{
	INT_PTR iRet = DialogBox(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDD_FILE_EXT_DIALOG), ZMain::GetInstance().GetHWND(), FileExtDlgProc);

	if ( iRet == IDOK )
	{
		SaveExtEnv();
	}
}


void ZFileExtDlg::_AddExtSet(const int iIconIndex, const TCHAR * ext)
{
	ExtSetting extSet;

	extSet.m_numIconIndex = iIconIndex;
	extSet.m_strExt = ext;
	m_extConnect.push_back(extSet);
}


void ZFileExtDlg::ExtMapInit()
{
	ExtSetting extSet;

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
int CALLBACK ZFileExtDlg::FileExtDlgProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		{
			//pThis->LoadExtEnv(hWnd);
		}
		return TRUE;

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDOK:
				EndDialog(hWnd, IDOK);
				break;

			case IDCANCEL:
				EndDialog(hWnd, IDCANCEL);
				break;
			}
		}
		break;

	case WM_DESTROY:
		EndDialog(hWnd, 0);
		return 0;
	}
	return FALSE;
}

/*
void ZFileExtDlg::LoadExtEnv(HWND hwnd)
{
	/// Registry 에서 설정되어 있는 값들을 받아온다.
	/// TODO: registry 에서 읽어온다.

	/// 읽어온 대로 checkbox 를 세팅한다.
	SendMessage(GetDlgItem(hwnd, (IDC_CHECK_BMP)), BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(GetDlgItem(hwnd, (IDC_CHECK_GIF)), BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(GetDlgItem(hwnd, (IDC_CHECK_PNG)), BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(GetDlgItem(hwnd, (IDC_CHECK_PSD)), BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(GetDlgItem(hwnd, (IDC_CHECK_JPG)), BM_SETCHECK, BST_CHECKED, 0);
	//SendMessage(GetDlgItem(hwnd, MAKEINTRESOURCE(IDC_CHECK_BMP)), BM_SETCHECK, BST_CHECKED, 0);
}
*/

void ZFileExtDlg::SaveExtEnv()
{
	std::vector < ExtSetting >::iterator it, endit = m_extConnect.end();

	tstring strProgramFolder(GetProgramFolder());

	tstring strIconDll = strProgramFolder;
	strIconDll += TEXT("ZViewerIcons.dll");

	for ( it = m_extConnect.begin(); it != endit; ++it)
	{
		const ExtSetting & extset = *it;

		if ( false == SetExtWithProgram(TEXT("ZViewer"), extset.m_strExt, 
			TEXT(""),	/// 프로그램 Full Path. 비워두면 현재 프로그램이다.
			strIconDll.c_str(),	/// 아이콘 프로그램
			extset.m_numIconIndex	/// 아이콘 index
		) )
		{
			_ASSERTE(false);
		}
	}

	/// explorer 의 아이콘을 update 시킨다.
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
}

bool ZFileExtDlg::SetExtWithProgram(const tstring & strProgramName, const tstring & strExt, tstring strFullProgramPath, const tstring & strIcon, int iIconIndex)
{
	ZFileExtReg fileExtReg;


	if ( strFullProgramPath.empty() )
	{
		// get full file path to program executable file
		TCHAR szProgPath[MAX_PATH] = { 0 };
		::GetModuleFileName(NULL, szProgPath, sizeof(szProgPath) / sizeof(TCHAR));

		strFullProgramPath = szProgPath;
	}

	tstring strTempText;

	fileExtReg.m_strExtension = strExt.c_str();

	// 프로그램에게 인자를 넘겨줄 때의 full path 를 만든다.
	strTempText  = strFullProgramPath;
	strTempText += TEXT(" \"%1\"");
	fileExtReg.m_strShellOpenCommand = strTempText.c_str();
	fileExtReg.m_strDocumentShellOpenCommand = strTempText.c_str();

	// 레지스트리에 등록할 때의 프로그램의 이름과 확장자를 정한다.
	tstring strClassName = strProgramName;
	strClassName += TEXT(".");
	strClassName += strExt;
	fileExtReg.m_strDocumentClassName = strClassName.c_str();

	// 확장자에 맞는 기본 아이콘을 지정한다.
	if ( strIcon.size() <= 0 )
	{
		// 아이콘 프로그램을 지정하지 않으면 원래 프로그램의 첫번째 아이콘을 쓴다.
		strTempText  = strFullProgramPath;
		strTempText += TEXT(",0");
	}
	else
	{
		// 아이콘 프로그램을 지정했으면 icon 번호를 쓴다.
		if ( iIconIndex < 0 )
		{
			_ASSERTE(iIconIndex >= 0 );
			iIconIndex = 0;
		}

		strTempText = strIcon;
		strTempText += TEXT(",");

		TCHAR szTemp[COMMON_BUFFER_SIZE] = { 0 };
		StringCchPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%d"), iIconIndex);
		strTempText += szTemp;
	}
	fileExtReg.m_strDocumentDefaultIcon = strTempText.c_str();

	// 설정된 값으로 레지스트리를 등록한다.
	fileExtReg.SetRegistries();

	return true;
}

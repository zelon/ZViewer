/* ------------------------------------------------------------------------
 *
 * ExifDlh.cpp
 *
 * 2008.10.27 KIM JINWOOK
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"

#include <cassert>

#include "ExifDlg.h"
#include "ZResourceManager.h"
#include "../commonSrc/MessageManager.h"

#include "resource.h"

#include "ZMain.h"

int CALLBACK ExifDlgWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

ExifDlg * pDlg = NULL;

void ExifDlg::SetWndProc()
{
	m_wndProc = (WNDPROC)ExifDlgWndProc;
	pDlg = this;
}

int CALLBACK ExifDlgWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM /*lParam*/)
{
	switch(iMessage)
	{
	case WM_CREATE:
		{
		}
		return 0;

	case WM_INITDIALOG:
		{
			pDlg->SetHWND(hWnd);
			InitCommonControls();

			LVCOLUMN col;

			col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			col.fmt = LVCFMT_LEFT;
			col.cx = 150;
			col.pszText = (LPWSTR)GetMessage(TEXT("METADATA_KEY"));
			col.iSubItem = 0;

			HWND hList = GetDlgItem(hWnd, IDC_EXIF_INFO_LIST);

			assert(hList != INVALID_HANDLE_VALUE);

			if ( -1 == SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&col) )
			{
				assert(false);
			}

			col.pszText = (LPWSTR)GetMessage(TEXT("METADATA_VALUE"));
			col.cx = 165;
			col.iSubItem = 1;
			if ( -1 == SendMessage(hList, LVM_INSERTCOLUMN, 1, (LPARAM)&col) )
			{
				assert(false);
			}

			pDlg->ShowExifMap();
		}
		return TRUE;

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDOK:
				EndDialog(hWnd, 0);
				break;
			}
		}
		break;

	}
	return FALSE;
}

void ExifDlg::DoResource(HWND hParentHWND)
{
	DialogBox(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDD_EXIF_DLG), hParentHWND, (DLGPROC)m_wndProc);
}


void ExifDlg::MakeExifMap(ZImage & image)
{
	image.GetExifList(m_exifList);
}

void ExifDlg::ShowExifMap()
{
	std::list < TagData >::iterator it;

	assert(m_hWindow != NULL);
	HWND hList = GetDlgItem(m_hWindow, IDC_EXIF_INFO_LIST);
	assert(hList != NULL);

	LVITEM li;
	tstring readKey, convertedKey, readValue, convertedValue;

	int iIndex = 0;
	for ( it = m_exifList.begin(); it != m_exifList.end(); ++it, ++iIndex )
	{
		li.mask = LVIF_TEXT;
		li.iItem = iIndex;
		li.iSubItem = 0;
		readKey = getWStringFromString(it->m_strKey);
		convertedKey = convertExifKey(readKey);
		li.pszText = (LPWSTR)(convertedKey.c_str());
		SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&li);

		li.iSubItem = 1;
		readValue = getWStringFromString(it->m_strValue);
		convertedValue = convertExifValue(readKey, readValue);
		li.pszText = (LPWSTR)(convertedValue.c_str());
		SendMessage(hList, LVM_SETITEM, 0, (LPARAM)&li);
	}
}

tstring ExifDlg::convertExifKey(const tstring & strKey) const
{
	std::map < tstring, tstring > keyMatch;
	keyMatch[TEXT("Make")] = GetMessage(TEXT("METADATA_KEY_MAKE"));
	keyMatch[TEXT("Orientation")] = GetMessage(TEXT("METADATA_KEY_ORIENTATION"));
	keyMatch[TEXT("Model")] = GetMessage(TEXT("METADATA_KEY_MODEL"));

	if ( keyMatch.find(strKey) != keyMatch.end() )
	{
		return keyMatch[strKey];
	}
	return strKey;
}

tstring ExifDlg::convertExifValue(const tstring & strKey, tstring & strValue) const
{
	std::map < tstring, tstring > valueMatch;
	valueMatch[TEXT(EXIF_ROTATION_0)] = GetMessage(TEXT("METADATA_VALUE_ORIENTATION_0"));
	valueMatch[TEXT(EXIF_ROTATION_90)] = GetMessage(TEXT("METADATA_VALUE_ORIENTATION_90"));
	valueMatch[TEXT(EXIF_ROTATION_180)] = GetMessage(TEXT("METADATA_VALUE_ORIENTATION_180"));
	valueMatch[TEXT(EXIF_ROTATION_270)] = GetMessage(TEXT("METADATA_VALUE_ORIENTATION_270"));

	if ( strKey == TEXT("Orientation") )
	{
		if ( valueMatch.find(strValue) != valueMatch.end() )
		{
			return valueMatch[strValue];
		}
	}
	return strValue;
}

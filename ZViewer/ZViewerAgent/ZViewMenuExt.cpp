// ZViewMenuExt.cpp : CZViewMenuExt의 구현입니다.

#include "stdafx.h"
#include "ZViewMenuExt.h"

// CZViewMenuExt

const LONG CZViewMenuExt::m_lMaxThumbnailSize = 128;
const LONG CZViewMenuExt::m_l3DBorderWidth    = 1;
const LONG CZViewMenuExt::m_lMenuItemSpacing  = 4;
const LONG CZViewMenuExt::m_lTotalBorderSpace = 2*(m_lMenuItemSpacing+m_l3DBorderWidth);

FIBITMAP* GenericLoader(const char* lpszPathName, int flag);

void CZViewMenuExt::MsgBox(const std::string & strMsg)
{
	MessageBox(HWND_DESKTOP, strMsg.c_str(), "ZViewerAgent", MB_OK);
}

// 최대 크기를 넘지 않는 적당한 리사이즈 크기를 돌려준다.
RECT CZViewMenuExt::GetResizedRect(const RECT & MaximumSize, const RECT & originalSize)
{
	if ( originalSize.right <= MaximumSize.right && originalSize.bottom <= MaximumSize.bottom )
	{
		RECT ret = originalSize;
		return ret;
	}

	// 가로 세로 크기 중 큰 값을 찾는다.
	bool bSetWidth = true;		// 가로 크기를 기준으로 맞출 것인가?

	double dWidthRate = (double)MaximumSize.right / (double)originalSize.right;
	double dHeightRate = (double)MaximumSize.bottom / (double)originalSize.bottom;

	if ( dHeightRate >=  dWidthRate)
	{
		bSetWidth = true;
	}
	else
	{
		bSetWidth = false;
	}

	// 큰 값이 MaximumSize 가 되게 하는 비례를 찾는다.
	RECT ret;

	double dRate = 1;
	if ( bSetWidth == true )
	{
		// 가로 크기가 기준이다.
		SetRect(&ret, 0, 0, (int)(originalSize.right*dWidthRate), (int)(originalSize.bottom*dWidthRate));
	}
	else
	{
		// 세로 크기가 기준이다.
		SetRect(&ret, 0, 0, (int)(originalSize.right*dHeightRate), (int)(originalSize.bottom*dHeightRate));
	}


	_ASSERTE(ret.right <= MaximumSize.right);
	_ASSERTE(ret.bottom <= MaximumSize.bottom);

	return ret;
}


bool CZViewMenuExt::IsAvailableFileExt(TCHAR * szFilename)
{
	if ( PathMatchSpec ( m_szFile, _T("*.bmp") ) ||
		PathMatchSpec ( m_szFile, _T("*.jpg") ) ||
		PathMatchSpec ( m_szFile, _T("*.jpeg") ) ||
		PathMatchSpec ( m_szFile, _T("*.gif") ) ||
		PathMatchSpec ( m_szFile, _T("*.ico") ) ||
		PathMatchSpec ( m_szFile, _T("*.pcx") ) ||
		PathMatchSpec ( m_szFile, _T("*.psd") ) ||
		PathMatchSpec ( m_szFile, _T("*.tif") ) ||
		PathMatchSpec ( m_szFile, _T("*.tga") ) ||
		PathMatchSpec ( m_szFile, _T("*.png") ) ||
		PathMatchSpec ( m_szFile, _T("*.jpg") )
		)
	{
		return true;
	}
	return false;
}

STDMETHODIMP CZViewMenuExt::Initialize (LPCITEMIDLIST pidlFolder, LPDATAOBJECT  pDO, HKEY hkeyProgID )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	COleDataObject dataobj;
	HGLOBAL        hglobal;
	HDROP          hdrop;
	bool           bOK = false;

    dataobj.Attach ( pDO, FALSE );      // FALSE = don't release IDataObject interface when destroyed

    // Get the first selected file name.  I'll keep this simple and just check
    // the first name to see if it's a .BMP.
    hglobal = dataobj.GetGlobalData ( CF_HDROP );

    if ( NULL == hglobal )
        return E_INVALIDARG;

    hdrop = (HDROP) GlobalLock ( hglobal );

    if ( NULL == hdrop )
        return E_INVALIDARG;

    // Get the name of the first selected file.
	if ( DragQueryFile ( hdrop, 0, m_szFile, MAX_PATH ))
	{
		// Is its extension .BMP?
		if ( IsAvailableFileExt(m_szFile) )
		{
			if ( m_originalImage.LoadFromFile(m_szFile) )
			{
				bOK = true;
			//	MsgBox("Here");

				if ( m_originalImage.GetBPP() == 8 )
				{
					m_b8bit = true;
				}
			}
			else
			{
				bOK = false;
			//	MsgBox("Here2");
			}
		}
	}

	GlobalUnlock ( hglobal );

    return bOK ? S_OK : E_FAIL;
}


STDMETHODIMP CZViewMenuExt::QueryContextMenu (
    HMENU hmenu,
    UINT uIndex,
    UINT uidCmdFirst,
    UINT uidCmdLast,
    UINT uFlags )
{
    // If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
    if ( uFlags & CMF_DEFAULTONLY )
	{
        return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );
	}

	bool bUseOwnerDraw = false;
	HINSTANCE hinstShell;

    hinstShell = GetModuleHandle ( _T("shell32") );

	if ( NULL != hinstShell )
	{
		DLLGETVERSIONPROC pProc;

		pProc = (DLLGETVERSIONPROC) GetProcAddress(hinstShell, "DllGetVersion");

		if ( NULL != pProc )
		{
			DLLVERSIONINFO rInfo = { sizeof(DLLVERSIONINFO) };

			if ( SUCCEEDED( pProc ( &rInfo ) ))
			{
				if ( rInfo.dwMajorVersion > 4 ||
					rInfo.dwMinorVersion >= 71 )
				{
					bUseOwnerDraw = true;
				}
			}
		}
	}

	MENUITEMINFO mii;

    mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask  = MIIM_ID | MIIM_TYPE;
	mii.fType  = bUseOwnerDraw ? MFT_OWNERDRAW : MFT_BITMAP;
	mii.wID    = uidCmdFirst;

	if ( !bUseOwnerDraw )
	{
		// NOTE: This will put the full-sized bitmap in the menu, which is
		// obviously a bit less than optimal.  Compressing the bitmap down
		// to a thumbnail is left as an exercise.
		mii.dwTypeData = (LPTSTR) m_bmp.GetSafeHandle();
	}

	InsertMenuItem ( hmenu, uIndex, TRUE, &mii );
	char szImageInfo[256];
	sprintf(szImageInfo, "%dx%d %dbpp", m_originalImage.GetWidth(), m_originalImage.GetHeight(), m_originalImage.GetBPP());
	InsertMenu( hmenu, uIndex, MF_BYPOSITION, uidCmdFirst, szImageInfo);

    // Store the menu item's ID so we can check against it later when
    // WM_MEASUREITEM/WM_DRAWITEM are sent.
    m_uOurItemID = uidCmdFirst;

    // Tell the shell we added 1 top-level menu item.
    return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 1 );
}

STDMETHODIMP CZViewMenuExt::GetCommandString (UINT uCmd, UINT uFlags, UINT* puReserved, LPSTR pszName, UINT cchMax)
{
	// Check idCmd, it must be 0 since we have only one menu item.
	if ( 0 != uCmd )
	{
		return E_INVALIDARG;
	}

	static LPCTSTR szHelpString = _T("Select this thumbnail to view the entire picture.");

    USES_CONVERSION;


    // If Explorer is asking for a help string, copy our string into the
    // supplied buffer.
    if ( uFlags & GCS_HELPTEXT )
	{
        if ( uFlags & GCS_UNICODE )
		{
            // We need to cast pszName to a Unicode string, and then use the
            // Unicode string copy API.
            lstrcpynW ( (LPWSTR) pszName, T2CW(szHelpString), cchMax );
		}
        else
		{
            // Use the ANSI string copy API to return the help string.
            lstrcpynA ( pszName, T2CA(szHelpString), cchMax );
		}
	}

    return S_OK;
}

STDMETHODIMP CZViewMenuExt::InvokeCommand ( LPCMINVOKECOMMANDINFO pInfo )	// 그림을 클릭했을 때
{
    // If lpVerb really points to a string, ignore this function call and bail out.
    if ( 0 != HIWORD( pInfo->lpVerb ))
	{
        return E_INVALIDARG;
	}

    // The command ID must be 0 since we only have one menu item.
    if ( 0 != LOWORD( pInfo->lpVerb ))
	{
        return E_INVALIDARG;
	}

    // Open the bitmap in the default paint program.
	//ShellExecute ( pInfo->hwnd, _T("open"), m_szFile, NULL, NULL, SW_SHOWNORMAL );
	//MessageBox(HWND_DESKTOP, "Clisdc1sk!!", "ok", MB_OK);

	char command[FILENAME_MAX] = { 0 };
	char szGetFileName[FILENAME_MAX] = { 0 };

#ifdef _DEBUG
	DWORD ret = GetModuleFileName(GetModuleHandle("ZViewerAgentD.dll"), szGetFileName, FILENAME_MAX);
#else
	DWORD ret = GetModuleFileName(GetModuleHandle("ZViewerAgent.dll"), szGetFileName, FILENAME_MAX);
#endif
	if ( ret == 0 )
	{
		_ASSERTE(!"Can't get module folder");
	}
	char szDrive[_MAX_DRIVE] = { 0 };
	char szDir[_MAX_DIR] = { 0 };
	_splitpath(szGetFileName, szDrive, szDir, 0, 0);

	std::string strProgramFolder = szDrive;
	strProgramFolder += szDir;

	wsprintf(command, "%s\\ZViewer.exe %s", strProgramFolder.c_str(), m_szFile);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	si.dwFlags =STARTF_USESHOWWINDOW;
	si.wShowWindow =SW_SHOW;
	ZeroMemory( &pi, sizeof(pi) );

	// ZViewer 를 실행시킨다.
	CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	//MessageBox(HWND_DESKTOP, command, command, MB_OK);
    return S_OK;
}

STDMETHODIMP CZViewMenuExt::HandleMenuMsg ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // res is a dummy LRESULT variable.  It's not used (IContextMenu2::HandleMenuMsg()
    // doesn't provide a way to return values), but it's here so that the code 
    // in MenuMessageHandler() can be the same regardless of which interface it
    // gets called through (IContextMenu2 or 3).

	LRESULT res;

    return MenuMessageHandler ( uMsg, wParam, lParam, &res );
}

STDMETHODIMP CZViewMenuExt::HandleMenuMsg2 ( UINT uMsg, WPARAM wParam, LPARAM lParam,
                                              LRESULT* pResult )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // For messages that have no return value, pResult is NULL.  This is
    // _very_ mean on the part of MS since it forces us to check whether
    // pResult is valid before using it.  You'd think that a pointer to
    // a "return value" would always be valid, but alas no.
    // If it is NULL, I create a dummy LRESULT variable, so the code in
    // MenuMessageHandler() will always have a valid pResult pointer.

    if ( NULL == pResult )
	{
		LRESULT res;
		return MenuMessageHandler ( uMsg, wParam, lParam, &res );
	}
    else
	{
		return MenuMessageHandler ( uMsg, wParam, lParam, pResult );
	}
}

STDMETHODIMP CZViewMenuExt::MenuMessageHandler ( UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
    switch ( uMsg )
	{
        case WM_MEASUREITEM:
            return OnMeasureItem ( (MEASUREITEMSTRUCT*) lParam, pResult );
        break;

        case WM_DRAWITEM:
            return OnDrawItem ( (DRAWITEMSTRUCT*) lParam, pResult );
        break;
	}

    return S_OK;
}


STDMETHODIMP CZViewMenuExt::OnMeasureItem ( MEASUREITEMSTRUCT* pmis, LRESULT* pResult )
{
    // Check that we're getting called for our own menu item.
	if ( m_uOurItemID != pmis->itemID )
        return S_OK;


	LONG   lThumbWidth;
	LONG   lThumbHeight;

	m_lBmpWidth = m_originalImage.GetWidth();
	m_lBmpHeight = m_originalImage.GetHeight();

	// 썸네일의 크기를 계산한다.
	RECT original = { 0, 0, m_lBmpWidth, m_lBmpHeight };
	RECT max = { 0, 0, m_lMaxThumbnailSize, m_lMaxThumbnailSize };

	RECT result = GetResizedRect(max, original);

	lThumbWidth = result.right;
	lThumbHeight = result.bottom;

    // Calculate the size of the menu item, which is the size of the thumbnail +
    // the border and padding (which provides some space at the edges of the item).
    m_lItemWidth = lThumbWidth + m_lTotalBorderSpace;
    m_lItemHeight = lThumbHeight + m_lTotalBorderSpace;


    // Store the size of the item in the MEASUREITEMSTRUCT.
    pmis->itemWidth = m_lItemWidth;
    pmis->itemHeight = m_lItemHeight;

    *pResult = TRUE;            // we handled the message

    return S_OK;
}

STDMETHODIMP CZViewMenuExt::OnDrawItem ( DRAWITEMSTRUCT* pdis, LRESULT* pResult )
{
    // Check that we're getting called for our own menu item.
    if ( m_uOurItemID != pdis->itemID )
        return S_OK;

	CDC*  pdcMenu = CDC::FromHandle ( pdis->hDC );
	CRect rcItem ( pdis->rcItem );  // RECT of our menu item
	CRect rcDraw;                   // RECT in which we'll be drawing

    // rcDraw will first be set to the RECT that we set in WM_MEASUREITEM.
    // It will get deflated as we go along.
    rcDraw.left = (rcItem.right + rcItem.left - m_lItemWidth) / 2;
    rcDraw.top = (rcItem.top + rcItem.bottom - m_lItemHeight) / 2;
    rcDraw.right = rcDraw.left + m_lItemWidth;
    rcDraw.bottom = rcDraw.top + m_lItemHeight;

    // Shrink rcDraw to account for the padding space around
    // the thumbnail.
    rcDraw.DeflateRect ( m_lMenuItemSpacing, m_lMenuItemSpacing );

   // 메뉴의 배경을 칠한다.
    if ( pdis->itemState & ODS_SELECTED )
	{
		// 마우스가 over 되어 있으면(선택되어있으면)
        pdcMenu->FillSolidRect ( rcItem, GetSysColor ( COLOR_HIGHLIGHT ));
	}
	else
	{
        pdcMenu->FillSolidRect ( rcItem, GetSysColor ( COLOR_3DFACE ));
	}

	// Draw the sunken 3D border.
	for ( int i = 1; i <= m_l3DBorderWidth; i++ )
	{
		pdcMenu->Draw3dRect ( rcDraw, GetSysColor ( COLOR_3DDKSHADOW ),
			GetSysColor ( COLOR_3DHILIGHT ));

		rcDraw.DeflateRect ( 1, 1 );
	}


	//MessageBeep(0);

	// Rescale 이 필요하면 해준다.
	if ( rcDraw.Width() < m_originalImage.GetWidth()
		|| rcDraw.Height() < m_originalImage.GetHeight() )
	{
		if ( m_b8bit )	// 8bit 컬러는 32로 바꾼 후에 rescale 해줘야 한다... 으음.. 왜 이럴까;;
		{
			if ( !m_originalImage.ConvertTo32Bit() )
			{
#ifdef _DEBUG
				MsgBox("Can't convert to 32bit");
#endif
			}
		}
		m_originalImage.Resize(rcDraw.Width(), rcDraw.Height());
	}

	// 실제로 그린다.
	int r = StretchDIBits(*pdcMenu,
		rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height(),
		0, 0, rcDraw.Width(), rcDraw.Height(),
		m_originalImage.GetData(),		//FreeImage_GetBits(m_toDraw),
		m_originalImage.GetBitmapInfo(),		//FreeImage_GetInfo(m_toDraw),
		0, 0x00cc0020);

    *pResult = TRUE;            // we handled the message

    return S_OK;
}


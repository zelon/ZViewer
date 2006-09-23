/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZViewMenuExt.h
*
*                                       http://www.wimy.com
*********************************************************************/

// ZViewMenuExt.h : CZViewMenuExt의 선언입니다.

#pragma once
#include "resource.h"       // 주 기호입니다.


#include "ZViewerAgent.h"
#include "../lib/DesktopWallPaper.h"


// CZViewMenuExt

class ATL_NO_VTABLE CZViewMenuExt : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CZViewMenuExt, &CLSID_ZViewMenuExt>,
	public IDispatchImpl<IZViewMenuExt, &IID_IZViewMenuExt, &LIBID_ZViewerAgentLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IShellExtInit,
	public IContextMenu3
{
public:
	CZViewMenuExt()
	{
		m_b8bit = false;
	}

	~CZViewMenuExt()
	{
//		MsgBox("des");
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_ZVIEWMENUEXT)


BEGIN_COM_MAP(CZViewMenuExt)
	COM_INTERFACE_ENTRY(IZViewMenuExt)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)
	COM_INTERFACE_ENTRY(IContextMenu2)
	COM_INTERFACE_ENTRY(IContextMenu3)
END_COM_MAP()

public:
	// IShellExtInit
	STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

    // IContextMenu
    STDMETHOD(QueryContextMenu)(HMENU, UINT, UINT, UINT, UINT);
    STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO);
    STDMETHOD(GetCommandString)(UINT_PTR, UINT, UINT*, LPSTR, UINT);

    // IContextMenu2
    STDMETHOD(HandleMenuMsg)(UINT, WPARAM, LPARAM);

    // IContextMenu3
    STDMETHOD(HandleMenuMsg2)(UINT, WPARAM, LPARAM, LRESULT*);


	DECLARE_PROTECT_FINAL_CONSTRUCT()

protected:

	/// ZViewer 를 실행시킨다.
	void ExecZViewer();

	/// 바탕화면 파일을 지정한다.
	void SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle style);

	void MsgBox(const tstring & strMsg);		// for debugging messagebox
    TCHAR   m_szFile[MAX_PATH];
	CBitmap m_bmp;
	bool m_b8bit;

	UINT m_uiMaxMenuID;

	ZImage m_originalImage;

    UINT    m_uOurItemID;

    LONG m_lItemWidth, m_lItemHeight;
    LONG m_lBmpWidth, m_lBmpHeight;

    static const LONG m_lMaxThumbnailSize;
    static const LONG m_l3DBorderWidth;
    static const LONG m_lMenuItemSpacing;
    static const LONG m_lTotalBorderSpace;

    // Helper functions for handling the menu-related messages.
    STDMETHOD(MenuMessageHandler)(UINT, WPARAM, LPARAM, LRESULT*);
    STDMETHOD(OnMeasureItem)(MEASUREITEMSTRUCT*, LRESULT*);
    STDMETHOD(OnDrawItem)(DRAWITEMSTRUCT*, LRESULT*);

public:

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	// 현재 쉘의 Extension 이 끝날 때 불린다. 만약 처음 Initialize 할 때 E_FAIL 을 리턴했으면 바로 불리고,
	// 쉘을 확장했으면 팝업메뉴가 사라질 때 불린다.
	void FinalRelease() 
	{
//		MsgBox("FinalRelease");
	}

public:

};

OBJECT_ENTRY_AUTO(__uuidof(ZViewMenuExt), CZViewMenuExt)

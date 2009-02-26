// ZViewerAgent.cpp : DLL 내보내기의 구현입니다.

#include "stdafx.h"
#include "resource.h"
#include "ZViewerAgent.h"

class CZViewerAgentModule : public CAtlDllModuleT< CZViewerAgentModule >
{
public :
	DECLARE_LIBID(LIBID_ZViewerAgentLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ZVIEWERAGENT, "{3081C080-FDE6-4B38-807E-C4EA3CE53C12}")
};

CZViewerAgentModule _AtlModule;

class CZViewerAgentApp : public CWinApp
{
public:

// 재정의
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CZViewerAgentApp, CWinApp)
END_MESSAGE_MAP()

CZViewerAgentApp theApp;

BOOL CZViewerAgentApp::InitInstance()
{
    return CWinApp::InitInstance();
}

int CZViewerAgentApp::ExitInstance()
{
    return CWinApp::ExitInstance();
}


// DLL이 OLE에 의해 언로드될 수 있는지 결정하는 데 사용됩니다.
STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _AtlModule.GetLockCount()==0) ? S_OK : S_FALSE;
}


// 클래스 팩터리를 반환하여 요청된 형식의 개체를 만듭니다.
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - 시스템 레지스트리에 항목을 추가합니다.
STDAPI DllRegisterServer(void)
{
    // 개체, 형식 라이브러리 및 형식 라이브러리에 들어 있는 모든 인터페이스를 등록합니다.
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - 시스템 레지스트리에서 항목을 제거합니다.
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

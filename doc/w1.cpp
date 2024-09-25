#include <atlbase.h>
#include <atlwin.h>
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef UNICODE
#define UNICODE
#endif

CAtlWinModule _Module;

class XWindow : public ATL::CWindowImpl<XWindow>
{
	BOOL m_bSC = TRUE;
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(XWindow)
#if 0
		MESSAGE_HANDLER(WM_ENTERSIZEMOVE, OnEnterSizeMove)
#endif 
		MESSAGE_HANDLER(WM_EXITSIZEMOVE, OnExitSizeMove)
		MESSAGE_HANDLER(WM_NCLBUTTONDOWN, OnNCLButtonDown)
		MESSAGE_HANDLER(WM_NCLBUTTONUP, OnNCLButtonUp)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL bHandled)
	{
		return 0;
	}
	
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL bHandled)
	{
		BOOL bTemp = TRUE;
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, &bTemp, 0);

		PostQuitMessage(0);
		return 0;
	}

	LRESULT OnNCLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL bTemp = TRUE;
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, &bTemp, 0);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNCLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL bTemp = TRUE;
    	//SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, &bTemp, 0);
		bHandled = FALSE;
		return 0;
	}

#if 10
	LRESULT OnEnterSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL bTemp = TRUE;
#if 0
		if(SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &bTemp, 0))
		{
			m_bSC = bTemp;
		}
#endif 
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, &bTemp, 0);

		//bHandled = FALSE;
		return 0;
	}

	LRESULT OnExitSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL bTemp = TRUE;
    	SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, &bTemp, 0);
		//bHandled = FALSE;
		return 0;
	}
#endif 
};

int WINAPI  _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nShowCmd*/)
{
	HRESULT hr;
    MSG msg = { 0 };
	XWindow xw;
	
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(S_OK != hr) return 0;

	xw.Create(NULL, CWindow::rcDefault, _T("Mini ATL Demo"), WS_OVERLAPPEDWINDOW|WS_VISIBLE);
	if(xw.IsWindow()) xw.ShowWindow(SW_SHOW); 
	else goto ExitThisApplication;
	
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

ExitThisApplication:
	CoUninitialize();
    return 0;
}
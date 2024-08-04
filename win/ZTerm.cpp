// ZTerm.cpp : main entry file for ZTerm.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlscrl.h>
#include <atlgdi.h>
#include <atlcoll.h>
#include <atlstr.h>

#include "resource.h"

#include "PuTTY.hpp"

#include "ZTerm.h"
#include "XTabCtl.h"
#include "ViewTab.h"
#include "ViewTTY.h"
#include "WinDlg.h"
#include "MainFrm.h"

CAppModule _Module;

///////////////////////////////////////////////////////////////////////////////
// XMessageLoop - PuTTY message loop implementation

class XMessageLoop
{
public:
	ATL::CSimpleArray<CMessageFilter*> m_aMsgFilter;
	ATL::CSimpleArray<CIdleHandler*> m_aIdleHandler;
	MSG m_msg;

	XMessageLoop()
	{
		memset(&m_msg, 0, sizeof(m_msg));
	}

	virtual ~XMessageLoop()
	{ }

	// Message filter operations
	BOOL AddMessageFilter(CMessageFilter* pMessageFilter)
	{
		return m_aMsgFilter.Add(pMessageFilter);
	}

	BOOL RemoveMessageFilter(CMessageFilter* pMessageFilter)
	{
		return m_aMsgFilter.Remove(pMessageFilter);
	}

	// Idle handler operations
	BOOL AddIdleHandler(CIdleHandler* pIdleHandler)
	{
		return m_aIdleHandler.Add(pIdleHandler);
	}

	BOOL RemoveIdleHandler(CIdleHandler* pIdleHandler)
	{
		return m_aIdleHandler.Remove(pIdleHandler);
	}

	// message loop
	int Run()
	{
		BOOL bDoIdle = TRUE;
		int nIdleCount = 0;
		BOOL bHasMessage = FALSE;

		for (;;)
		{
			bHasMessage = ::PeekMessage(&m_msg, NULL, 0, 0, PM_NOREMOVE);
			while (bDoIdle && !bHasMessage)
			{
				if (!OnIdle(nIdleCount++))
					bDoIdle = FALSE;
			}

			PuTTY_MessageLoop(bHasMessage, &m_msg);

			if (m_msg.message == WM_QUIT)
				break;

			if (IsIdleMessage(&m_msg))
			{
				bDoIdle = TRUE;
				nIdleCount = 0;
			}
		}

		return (int)m_msg.wParam;
	}

	// Overrideables
		// Override to change message filtering
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		// loop backwards
		for (int i = m_aMsgFilter.GetSize() - 1; i >= 0; i--)
		{
			CMessageFilter* pMessageFilter = m_aMsgFilter[i];
			if ((pMessageFilter != NULL) && pMessageFilter->PreTranslateMessage(pMsg))
				return TRUE;
		}
		return FALSE;   // not translated
	}

	// override to change idle processing
	virtual BOOL OnIdle(int /*nIdleCount*/)
	{
		for (int i = 0; i < m_aIdleHandler.GetSize(); i++)
		{
			CIdleHandler* pIdleHandler = m_aIdleHandler[i];
			if (pIdleHandler != NULL)
				pIdleHandler->OnIdle();
		}
		return FALSE;   // don't continue
	}

	// override to change non-idle messages
	virtual BOOL IsIdleMessage(MSG* pMsg) const
	{
		// These messages should NOT cause idle processing
		switch (pMsg->message)
		{
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
		case WM_PAINT:
		case 0x0118:	// WM_SYSTIMER (caret blink)
			return FALSE;
		}
		return TRUE;
	}
};

static int AppRun(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	int nRet = 0;
	XMessageLoop theLoop; // we use XMessageLoop because PuTTY has some special logic
	_Module.AddMessageLoop((CMessageLoop*)&theLoop);

	CMainFrame wndMain;

	if (wndMain.CreateEx())
	{
		// start up the background network communication threads
		//zx_StartupNetworkThread(wndMain);

		PuTTY_AttachWindow(wndMain.m_viewTTY, wndMain.m_hWnd, 0);

		nRet = theLoop.Run();
	}

	if (wndMain.IsWindow())
	{
		// when the user type "exit" command in the last session
		wndMain.DestroyWindow();
		wndMain.m_hWnd = NULL;
	}

	_Module.RemoveMessageLoop();
	return nRet;
}

static int AppInit(HINSTANCE hInstance)
{
	int ret;
	ret = PuTTY_Init(hInstance);
	if (ret)
		return ret;

	return 0;
}

static int AppTerm(HINSTANCE hInstance = NULL)
{
	PuTTY_Term();
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpstrCmdLine, _In_ int nCmdShow)
{
	int nRet;

	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	nRet = AppInit(hInstance);
	if (nRet == 0)
	{
		AppRun(lpstrCmdLine, nCmdShow);
	}

	AppTerm();

	_Module.Term();
	::CoUninitialize();

	return nRet;
}

// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#define DECLARE_TTYWND_CLASS(WndClassName) \
static ATL::CWndClassInfo& GetWndClassInfo() \
{ \
	static ATL::CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW /*| CS_DBLCLKS*/, StartWindowProc, \
		  0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, WndClassName, NULL }, \
		NULL, NULL, IDC_IBEAM, TRUE, 0, _T("") \
	}; \
	return wc; \
}

#define MESSAGE_PUTTYMSG(func) \
	{ \
		bHandled = TRUE; \
		lResult = func(m_hWnd, uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

class CTTYView : public CWindowImpl<CTTYView>
{
public:
	DECLARE_TTYWND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return FALSE;
	}

	BEGIN_MSG_MAP(CTTYView)
		MESSAGE_PUTTYMSG(PuTTY_WndProc)
	END_MSG_MAP()
};

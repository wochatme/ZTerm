// ViewTTY.h : interface of the CViewTTY class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#define DECLARE_TTYWND_CLASS(WndClassName) \
static ATL::CWndClassInfo& GetWndClassInfo() \
{ \
	static ATL::CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, StartWindowProc, \
		  0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, WndClassName, NULL }, \
		NULL, NULL, IDC_IBEAM, TRUE, 0, _T("") \
	}; \
	return wc; \
}

#define MESSAGE_HANDLEALL(func) \
	{ \
		bHandled = TRUE; \
		lResult = func(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define TTY_MODE		(0x00000001)

class CViewTTY : public CWindowImpl<CViewTTY>
{
	DWORD m_dwState = TTY_MODE;
public:
	DECLARE_TTYWND_CLASS(NULL)
#if 0
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}
#endif 
	BEGIN_MSG_MAP(CViewTTY)
		if (m_dwState & TTY_MODE)
			MESSAGE_HANDLEALL(OnTTYMode)
		else
			MESSAGE_HANDLEALL(OnSETMode)
	END_MSG_MAP()

	LRESULT OnTTYMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		switch (uMsg)
		{
		case WM_ERASEBKGND:
			bHandled = TRUE;
			return 1L;
		case WM_PAINT:
			bHandled = TRUE;
			{
				PAINTSTRUCT ps = { 0 };
				HDC hdc = BeginPaint(&ps);
				if (hdc)
				{
					HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
					if (hBrush)
					{
						RECT rc = { 0 };
						GetClientRect(&rc);
						FillRect(hdc, &rc, hBrush);
						DeleteObject(hBrush);
					}
				}
				EndPaint(&ps);
			}
            break;
		default:
			break;
		}
		return 0L;
	}

	LRESULT OnSETMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0L;
	}

};

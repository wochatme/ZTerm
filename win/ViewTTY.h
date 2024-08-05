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

#define SCREEN_BUF_MAX		(1<<18)

class CTTYView : public CWindowImpl<CTTYView>
{
public:
	U8* m_utf8Buf = nullptr;
	U32 m_utf8Max = 0;
	U32 m_utf8Len = 0;

	WCHAR* m_utf16Buf = nullptr;
	U32 m_utf16Max = 0;

	CTTYView()
	{
		m_utf16Max = SCREEN_BUF_MAX;
		m_utf16Buf = (WCHAR*)VirtualAlloc(NULL, m_utf16Max, MEM_COMMIT, PAGE_READWRITE);
		ATLASSERT(m_utf16Buf);
		m_utf8Max = 0;
	}

	~CTTYView()
	{
		if (nullptr != m_utf16Buf)
		{
			VirtualFree(m_utf16Buf, 0, MEM_RELEASE);
			m_utf16Buf = nullptr;
		}

		if (nullptr != m_utf8Buf)
		{
			VirtualFree(m_utf16Buf, 0, MEM_RELEASE);
			m_utf8Buf = nullptr;
		}
		m_utf8Len = 0;
	}

public:
	DECLARE_TTYWND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return FALSE;
	}

	BEGIN_MSG_MAP(CTTYView)
		MESSAGE_PUTTYMSG(PuTTY_WndProc)
	END_MSG_MAP()

	U8* GetScreenData(U32& length)
	{
		U8* sbuf = nullptr;
		if (m_utf16Buf)
		{
			size_t realsize = 0;
			U32 slen = PuTTY_ScreenCopy(m_utf16Buf, m_utf16Max, &realsize);
			if (slen == 0)
			{
				VirtualFree(m_utf16Buf, 0, MEM_RELEASE);
				if (realsize)
				{
					m_utf16Max = WT_ALIGN_PAGE(realsize);
				}
				else
				{
					m_utf16Max <<= 1;
				}

				m_utf16Buf = (wchar_t*)VirtualAlloc(NULL, m_utf16Max, MEM_COMMIT, PAGE_READWRITE);
				if (m_utf16Buf)
				{
					slen = PuTTY_ScreenCopy(m_utf16Buf, m_utf16Max, NULL);
				}
			}

			if (slen)
			{
				U32 utf8len = 0;
				ATLASSERT(m_utf16Buf);
				U32 status = wt_UTF16ToUTF8((U16*)m_utf16Buf, slen, NULL, &utf8len);
				if (status == WT_OK && utf8len)
				{
					if (utf8len > m_utf8Max)
					{
						if (m_utf8Buf)
						{
							VirtualFree(m_utf8Buf, 0, MEM_RELEASE);
						}
						m_utf8Max = WT_ALIGN_PAGE(utf8len);
						m_utf8Buf = (U8*)VirtualAlloc(NULL, m_utf8Max, MEM_COMMIT, PAGE_READWRITE);
					}
					ATLASSERT(m_utf8Max > utf8len);
					if (m_utf8Buf)
					{
						status = wt_UTF16ToUTF8((U16*)m_utf16Buf, slen, m_utf8Buf, NULL);
						length = utf8len;
						m_utf8Buf[utf8len] = '\0';
						sbuf = m_utf8Buf;
					}
				}
			}
		}
		return sbuf;
	}

};

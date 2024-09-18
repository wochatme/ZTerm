#pragma once
#if 0
class CView : public CWindowImpl<CView>
{
public:
	DECLARE_WND_CLASS(NULL)
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return FALSE;
	}
	BEGIN_MSG_MAP(CView)
	END_MSG_MAP()
};
#endif 

#pragma warning( disable: 4838)
static const unsigned char ai_greeting[] =
{
	'+','+','+','+','+','+','+','+','+','\n',
	0xF0,0x9F,0x99,0x82,0x0A,0x48,0x65,0x6C,
	0x6C,0x6F,0x2C,0x20,0x69,0x66,0x20,0x79,
	0x6F,0x75,0x20,0x68,0x61,0x76,0x65,0x20,
	0x61,0x6E,0x79,0x20,0x74,0x65,0x63,0x68,
	0x6E,0x69,0x63,0x61,0x6C,0x20,0x71,0x75,
	0x65,0x73,0x74,0x69,0x6F,0x6E,0x2C,0x20,
	0x70,0x6C,0x65,0x61,0x73,0x65,0x20,0x69,
	0x6E,0x70,0x75,0x74,0x20,0x69,0x6E,0x20,
	0x74,0x68,0x65,0x20,0x62,0x65,0x6C,0x6F,
	0x77,0x20,0x77,0x69,0x6E,0x64,0x6F,0x77,
	0x2E,0x0A,0
};

class CGPTView
{
public:
	HWND m_hWnd = NULL;

	HWND Create(
		_In_opt_ HWND hWndParent,
		_In_ _U_RECT rect = NULL,
		_In_opt_z_ LPCTSTR szWindowName = NULL,
		_In_ DWORD dwStyle = 0,
		_In_ DWORD dwExStyle = 0,
		_In_ _U_MENUorID MenuOrID = 0U,
		_In_opt_ LPVOID lpCreateParam = NULL)
	{
		m_hWnd = CreateWindowExW(0, L"Scintilla", NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
			0, 0, 16, 16, hWndParent, NULL, HINST_THISCOMPONENT, NULL);

		if (::IsWindow(m_hWnd))
		{
			::SendMessage(m_hWnd, SCI_SETTECHNOLOGY, SC_TECHNOLOGY_DIRECTWRITE, 0);
			::SendMessage(m_hWnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
			::SendMessage(m_hWnd, SCI_SETEOLMODE, SC_EOL_LF, 0);
			::SendMessage(m_hWnd, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);
			::SendMessage(m_hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)"Courier New");
			::SendMessage(m_hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, 1100);

			//::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(255, 255, 255));
			//::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 1, 2);
			::SendMessage(m_hWnd, SCI_SETPHASESDRAW, SC_PHASES_MULTIPLE, 0);
			::SendMessage(m_hWnd, SCI_SETTEXT, 0, (LPARAM)ai_greeting);
			::SendMessage(m_hWnd, SCI_SETREADONLY, 1, 0);

			::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 0, 0);
			::SendMessage(m_hWnd, SCI_SETMARGINS, 0, 0);
			::SendMessage(m_hWnd, SCI_SETMARGINLEFT, 0, 0);

			::SetWindowTheme(m_hWnd, L"DarkMode_Explorer", nullptr);

#if 0
			::SendMessage(m_hWnd, SCI_SETVSCROLLBAR, 0, 0);
#endif 
		}
		return m_hWnd;
	}

	operator HWND() const throw()
	{
		return m_hWnd;
	}

	BOOL IsWindow() const throw()
	{
		return ::IsWindow(m_hWnd);
	}

	BOOL GetFocus()
	{
		return (m_hWnd == ::GetFocus());
	}

	BOOL ShowWindow(int nCmdShow)
	{
		BOOL bRet = FALSE;

		if (IsWindow())
		{
			bRet = ::ShowWindow(m_hWnd, nCmdShow);
		}

		return bRet;
	}

	BOOL SetWindowPos(HWND hWndInsertAfter, int  X, int  Y, int  cx, int  cy, UINT uFlags)
	{
		BOOL bRet = FALSE;
		if (IsWindow())
		{
			bRet = ::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
		}
		return bRet;
	}

	int AppendText(const char* text, U32 length)
	{
		if (IsWindow())
		{
			int totalLines;
			//bool readonly = (bool)::SendMessage(m_hWnd, SCI_GETREADONLY, 0, 0);
			::SendMessage(m_hWnd, SCI_SETREADONLY, 0, 0);
			::SendMessage(m_hWnd, SCI_APPENDTEXT, length, (LPARAM)text);
			totalLines = (int)SendMessage(m_hWnd, SCI_GETLINECOUNT, 0, 0);
			::SendMessage(m_hWnd, SCI_LINESCROLL, 0, totalLines);
			::SendMessage(m_hWnd, SCI_SETREADONLY, 1, 0);
		}
		return 0;
	}

	void EnableDarkMode(bool darkmode = true)
	{
		if (darkmode)
		{
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(0, 0, 0));
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_DEFAULT, RGB(13, 13, 13));
			::SendMessage(m_hWnd, SCI_STYLESETFORE, STYLE_DEFAULT, RGB(250, 250, 250));
			::SendMessage(m_hWnd, SCI_SETCARETFORE, RGB(250, 250, 250), 0);
			::SendMessage(m_hWnd, SCI_STYLECLEARALL, 0, 0);
		}
		else
		{
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(255, 255, 255));
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_DEFAULT, RGB(255, 255, 255));
			::SendMessage(m_hWnd, SCI_STYLESETFORE, STYLE_DEFAULT, RGB(250, 250, 250));
			//::SendMessage(m_hWnd, SCI_SETCARETFORE, RGB(250, 250, 250), 0);
			::SendMessage(m_hWnd, SCI_STYLECLEARALL, 0, 0);
		}
	}
};

#define INPUT_BUF_INPUT_MAX		(1<<16)
class CAskView
{
	U8* m_inputBuf = nullptr;
	U32 m_inputMax = INPUT_BUF_INPUT_MAX;
	U32 m_inputLen = 0;

public:
	HWND m_hWnd = NULL;

	CAskView()
	{
		m_inputMax = INPUT_BUF_INPUT_MAX;
		m_inputLen = 0;
		m_inputBuf = (U8*)VirtualAlloc(NULL, INPUT_BUF_INPUT_MAX, MEM_COMMIT, PAGE_READWRITE);
		ATLASSERT(m_inputBuf);
	}

	~CAskView()
	{
		if (nullptr != m_inputBuf)
		{
			VirtualFree(m_inputBuf, 0, MEM_RELEASE);
			m_inputBuf = nullptr;
		}
		m_inputLen = 0;
	}

	HWND Create(
		_In_opt_ HWND hWndParent,
		_In_ _U_RECT rect = NULL,
		_In_opt_z_ LPCTSTR szWindowName = NULL,
		_In_ DWORD dwStyle = 0,
		_In_ DWORD dwExStyle = 0,
		_In_ _U_MENUorID MenuOrID = 0U,
		_In_opt_ LPVOID lpCreateParam = NULL)
	{
		m_hWnd = CreateWindowExW(0, L"Scintilla", NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL,
			0, 0, 16, 16, hWndParent, NULL, HINST_THISCOMPONENT, NULL);

		if (::IsWindow(m_hWnd))
		{
			::SendMessage(m_hWnd, SCI_SETTECHNOLOGY, SC_TECHNOLOGY_DIRECTWRITE, 0);
			::SendMessage(m_hWnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
			::SendMessage(m_hWnd, SCI_SETEOLMODE, SC_EOL_LF, 0);
			::SendMessage(m_hWnd, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);
			::SendMessage(m_hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)"Courier New");
			::SendMessage(m_hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, 1100);

			//::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(255, 255, 255));
			//::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 1, 2);
			::SendMessage(m_hWnd, SCI_SETPHASESDRAW, SC_PHASES_MULTIPLE, 0);
			::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 0, 0);
			::SendMessage(m_hWnd, SCI_SETMARGINS, 0, 0);
			::SendMessage(m_hWnd, SCI_SETMARGINLEFT, 0, 0);

			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(0, 0, 0));
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_DEFAULT, RGB(13, 13, 13));
			::SendMessage(m_hWnd, SCI_STYLESETFORE, STYLE_DEFAULT, RGB(250, 250, 250));
			::SendMessage(m_hWnd, SCI_SETCARETFORE, RGB(250, 250, 250), 0);
			::SendMessage(m_hWnd, SCI_STYLECLEARALL, 0, 0);

			::SetWindowTheme(m_hWnd, L"DarkMode_Explorer", nullptr);
		}
		return m_hWnd;
	}

	operator HWND() const throw()
	{
		return m_hWnd;
	}

	BOOL IsWindow() const throw()
	{
		return ::IsWindow(m_hWnd);
	}

	BOOL GetFocus()
	{
		return (m_hWnd == ::GetFocus());
	}

	BOOL ShowWindow(int nCmdShow)
	{
		BOOL bRet = FALSE;

		if (IsWindow())
		{
			bRet = ::ShowWindow(m_hWnd, nCmdShow);
		}

		return bRet;
	}

	BOOL SetWindowPos(HWND hWndInsertAfter, int  X, int  Y, int  cx, int  cy, UINT uFlags)
	{
		BOOL bRet = FALSE;
		if (IsWindow())
		{
			bRet = ::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
		}
		return bRet;
	}

	int SetText(const char* text)
	{
		if (IsWindow() && text)
		{
			::SendMessage(m_hWnd, SCI_SETTEXT, 0, (LPARAM)text);
		}
		return 0;
	}

	char GetLastChar()
	{
		char ch = '\0';
		if (IsWindow())
		{
			int pos = (int)::SendMessage(m_hWnd, SCI_GETCURRENTPOS, 0, 0);
			if (pos > 0)
				ch = (char)::SendMessage(m_hWnd, SCI_GETCHARAT, pos - 1, 0);
		}
		return ch;
	}

	U8* GetInputData(U32& bytes, U8& offset, bool& shareScreen, bool screenConf = true)
	{
		U8* p = nullptr;

		if (IsWindow())
		{
			U32 real_len, len = 0;
			bool hasPrefix = false;
			bool ss = screenConf;
			len = (U32)::SendMessage(m_hWnd, SCI_GETTEXTLENGTH, 0, 0);
			if (len > INPUT_BUF_INPUT_MAX - 10)
				len = INPUT_BUF_INPUT_MAX - 10;

			p = m_inputBuf + 10;
			::SendMessage(m_hWnd, SCI_GETTEXT, len, (LPARAM)p);

			if (len > 3)
			{
				if (p[0] == '-' && p[1] == '-')
				{
					hasPrefix = true;
					ss = false;
					p += 2;
					len -= 2;
				}
				else if (p[0] == '+' && p[1] == '+')
				{
					hasPrefix = true;
					ss = true;
					p += 2;
					len -= 2;
				}
			}
			/* skip the white space */
			real_len = len;
			while (real_len && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
			{
				p++;
				real_len--;
			}

			if (real_len) /* the user does input some data */
			{
				offset = 6;
				U8* q = p - offset;
				if (hasPrefix)
				{
					offset = 9;
					q = p - offset;
					q[0] = '\n'; q[1] = 0xF0; q[2] = 0x9F; q[3] = 0xA4; q[4] = 0x9A; q[5] = '\n';
					if (ss)
					{
						q[6] = '+'; q[7] = '+'; q[8] = ' ';
					}
					else
					{
						q[6] = '-'; q[7] = '-'; q[8] = ' ';
					}
				}
				else
				{
					q[0] = '\n'; q[1] = 0xF0; q[2] = 0x9F; q[3] = 0xA4; q[4] = 0x9A; q[5] = '\n';
				}
				bytes = real_len + offset;
				shareScreen = ss;
				p = q;
			}
			else
			{
				p = nullptr;
			}
		}
		return p;
	}
};

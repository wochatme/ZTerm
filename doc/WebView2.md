# 命令行编译WebView2的例子

本文档讲述了命令行编译一个WebView2 C++的例子的全部过程

## 下载WebView2Samples
它的网址是： https://github.com/MicrosoftEdge/WebView2Samples

```
git clone https://github.com/MicrosoftEdge/WebView2Samples.git
```
假设该代码库在D:\github\WebView2Samples中。在D:\github\WebView2Samples\GettingStartedGuides\Win32_GettingStarted目录下你会发现一个文件packages.config
```
D:\github\WebView2Samples\GettingStartedGuides\Win32_GettingStarted>type packages.config
<?xml version="1.0" encoding="utf-8"?>
<packages>
  <package id="Microsoft.Web.WebView2" version="1.0.902.49" targetFramework="native" />
  <package id="Microsoft.Windows.ImplementationLibrary" version="1.0.191107.2" targetFramework="native" />
</packages>
```
我们需要下载两个包，在上述文件中已经表明了。执行如下命令：
```
D:\github\WebView2Samples\GettingStartedGuides\Win32_GettingStarted>nuget restore packages.config -PackagesDirectory packages
Restoring NuGet package Microsoft.Web.WebView2.1.0.902.49.
Restoring NuGet package Microsoft.Windows.ImplementationLibrary.1.0.191107.2.
Adding package 'Microsoft.Windows.ImplementationLibrary.1.0.191107.2' to folder 'D:\github\WebView2Samples\GettingStartedGuides\Win32_GettingStarted\packages'
Adding package 'Microsoft.Web.WebView2.1.0.902.49' to folder 'D:\github\WebView2Samples\GettingStartedGuides\Win32_GettingStarted\packages'
Added package 'Microsoft.Windows.ImplementationLibrary.1.0.191107.2' to folder 'D:\github\WebView2Samples\GettingStartedGuides\Win32_GettingStarted\packages'
Added package 'Microsoft.Web.WebView2.1.0.902.49' to folder 'D:\github\WebView2Samples\GettingStartedGuides\Win32_GettingStarted\packages'

NuGet Config files used:
    C:\Users\Frank Wang\AppData\Roaming\NuGet\NuGet.Config
    C:\Program Files (x86)\NuGet\Config\Microsoft.VisualStudio.FallbackLocation.config
    C:\Program Files (x86)\NuGet\Config\Microsoft.VisualStudio.Offline.config

Feeds used:
    C:\Users\Frank Wang\.nuget\packages\
    https://api.nuget.org/v3/index.json
    C:\Program Files (x86)\Microsoft SDKs\NuGetPackages\

Installed:
    2 package(s) to packages.config projects
```
结果发现产生了一个新的目录packages。我们需要的两个组件都在里面了。你可以进去看看。

我们的主程序是wv.cpp
```
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c
#include <windows.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include <wrl.h>
#include <wil/com.h>
#include "WebView2.h"

using namespace Microsoft::WRL;

// Global variables

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("WebView sample");

HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Pointer to WebViewController
static wil::com_ptr<ICoreWebView2Controller> webviewController;

// Pointer to WebView window
static wil::com_ptr<ICoreWebView2> webview;

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	// Store instance handle in our global variable
	hInst = hInstance;

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1200, 900,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// <-- WebView2 sample code starts here -->
	// Step 3 - Create a single WebView within the parent window
	// Locate the browser and set up the environment for WebView
	CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[hWnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

				// Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
				env->CreateCoreWebView2Controller(hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[hWnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
						if (controller != nullptr) {
							webviewController = controller;
							webviewController->get_CoreWebView2(&webview);
						}

						// Add a few settings for the webview
						// The demo step is redundant since the values are the default settings
						wil::com_ptr<ICoreWebView2Settings> settings;
						webview->get_Settings(&settings);
						settings->put_IsScriptEnabled(TRUE);
						settings->put_AreDefaultScriptDialogsEnabled(TRUE);
						settings->put_IsWebMessageEnabled(TRUE);

						// Resize WebView to fit the bounds of the parent window
						RECT bounds;
						GetClientRect(hWnd, &bounds);
						webviewController->put_Bounds(bounds);

						// Schedule an async task to navigate to Bing
						webview->Navigate(L"https://maps.google.com");

						// <NavigationEvents>
						// Step 4 - Navigation events
						// register an ICoreWebView2NavigationStartingEventHandler to cancel any non-https navigation
						EventRegistrationToken token;
						webview->add_NavigationStarting(Callback<ICoreWebView2NavigationStartingEventHandler>(
							[](ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
								wil::unique_cotaskmem_string uri;
								args->get_Uri(&uri);
								std::wstring source(uri.get());
								if (source.substr(0, 5) != L"https") {
									args->put_Cancel(true);
								}
								return S_OK;
							}).Get(), &token);
						// </NavigationEvents>

						// <Scripting>
						// Step 5 - Scripting
						// Schedule an async task to add initialization script that freezes the Object object
						webview->AddScriptToExecuteOnDocumentCreated(L"Object.freeze(Object);", nullptr);
						// Schedule an async task to get the document URL
						webview->ExecuteScript(L"window.document.URL;", Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
							[](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
								LPCWSTR URL = resultObjectAsJson;
								//doSomethingWithURL(URL);
								return S_OK;
							}).Get());
						// </Scripting>

						// <CommunicationHostWeb>
						// Step 6 - Communication between host and web content
						// Set an event handler for the host to return received message back to the web content
						webview->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>(
							[](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
								wil::unique_cotaskmem_string message;
								args->TryGetWebMessageAsString(&message);
								// processMessage(&message);
								webview->PostWebMessageAsString(message.get());
								return S_OK;
							}).Get(), &token);

						// Schedule an async task to add initialization script that
						// 1) Add an listener to print message from the host
						// 2) Post document URL to the host
						webview->AddScriptToExecuteOnDocumentCreated(
							L"window.chrome.webview.addEventListener(\'message\', event => alert(event.data));" \
							L"window.chrome.webview.postMessage(window.document.URL);",
							nullptr);
						// </CommunicationHostWeb>

						return S_OK;
					}).Get());
				return S_OK;
			}).Get());
	
	// <-- WebView2 sample code ends here -->
	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR greeting[] = _T("Hello, Windows desktop!");

	switch (message)
	{
	case WM_SIZE:
		if (webviewController != nullptr) {
			RECT bounds;
			GetClientRect(hWnd, &bounds);
			webviewController->put_Bounds(bounds);
		};
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

```

下面是编译它的命令行：
```
cl wv.cpp /std:c++17 /EHsc /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS user32.lib Advapi32.lib Shell32.lib Version.lib /Id:/github/WebView2Samples/GettingStartedGuides/Win32_GettingStarted/packages/Microsoft.Windows.ImplementationLibrary.1.0.191107.2/include /Id:/github/WebView2Samples/GettingStartedGuides/Win32_GettingStarted/packages/Microsoft.Web.WebView2.1.0.902.49/build/native/include d:/github/WebView2Samples/GettingStartedGuides/Win32_GettingStarted/packages/Microsoft.Web.WebView2.1.0.902.49/build/native/x64/WebView2LoaderStatic.lib
```
如果编译没有错误，产生了最终的可执行文件
```
C:\windev>dir wv.exe
 Volume in drive C is Windows
 Volume Serial Number is C416-5CB9

 Directory of C:\windev

03/23/2024  11:49 AM           239,616 wv.exe
               1 File(s)        239,616 bytes
               0 Dir(s)  197,604,958,208 bytes free
```
你执行这个可执行文件，一个谷歌地图的应用就出现了。
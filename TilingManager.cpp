// TilingManager.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TilingManager.h"
#include "WindowController.h"

#define MAX_LOADSTRING 100

// Global Variables:
HANDLE m_Screen = GetStdHandle(STD_OUTPUT_HANDLE);
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK LLKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK	mouseProc(int nCode, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK enumVisibleWindows(HWND hWnd, LPARAM lParam);
BOOL CALLBACK enumMaximizedWindows(HWND hWnd, LPARAM lParam);

HHOOK hMouseHook;
WindowController *windowController;

void enumAllWindows() {
	EnumWindows(enumVisibleWindows, 0);
	EnumWindows(enumMaximizedWindows, 0);
	windowController->cleanUpVisibleWindows();
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, hInstance, 0);
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TILINGMANAGER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TILINGMANAGER));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (windowController->updateEnumWindows()) {
			enumAllWindows();
		}
		windowController->enumWindowsComplete();
		//windowController->windowLoop();
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TILINGMANAGER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TILINGMANAGER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//



BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable
   
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   windowController = new WindowController();
   enumAllWindows();
   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	static HHOOK hook_keys;
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		hook_keys = SetWindowsHookEx(WH_KEYBOARD_LL, LLKeyboardProc,((LPCREATESTRUCT) lParam)->hInstance,0);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

LRESULT CALLBACK LLKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT hookstruct;
	windowController->checkKeyStates();
	if (nCode == HC_ACTION)
	{
		switch (wParam)
		{
		case WM_KEYDOWN: case WM_SYSKEYDOWN:
			hookstruct = (PKBDLLHOOKSTRUCT) lParam;
			windowController->keysPressed.push_back(hookstruct->vkCode);
			if (windowController->isPressed(VK_LWIN) && hookstruct->vkCode != VK_LWIN) {
				//some combination of windows + something else
				return 1;
			}
			break;
		case WM_KEYUP:    case WM_SYSKEYUP:
			hookstruct = (PKBDLLHOOKSTRUCT) lParam;
			windowController->keysPressed.erase(std::remove(windowController->keysPressed.begin(), windowController->keysPressed.end(), hookstruct->vkCode), windowController->keysPressed.end());
			//handle release of window key
			//fixes scroll + window key
			if (hookstruct->vkCode == VK_LWIN && windowController->disableStartMenu) {
				/* pesky Windows button */
				windowController->disableStartMenu = false;
				INPUT ip;
				ip.type = INPUT_KEYBOARD;
				ip.ki.wScan = 0; // hardware scan code for key
				ip.ki.time = 0;
				ip.ki.dwExtraInfo = 0;
				// Release the windows key
				ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
				SendInput(1, &ip, sizeof(INPUT));
			}
			break;
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0) {
		return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
	}
	if (nCode == HC_ACTION){
		switch (wParam){
		case WM_MOUSEWHEEL:
			MSLLHOOKSTRUCT *mouseInfo = (MSLLHOOKSTRUCT*) lParam;
			short currentWheelDelta = windowController->getWheelDelta();
			windowController->setWheelDelta(windowController->getWheelDelta() + HIWORD(mouseInfo->mouseData));
			windowController->checkForScroll();
			break;
		}
	}
	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

BOOL CALLBACK enumVisibleWindows(HWND hWnd, LPARAM lParam)
{
	if (IsWindowVisible(hWnd)) {
		std::string name = windowController->getNameOfWindow(hWnd);
		if (name != "Program Manager" && name != "Start Menu" && name != "Start" && name.length() > 1) {
			windowController->insertNewVisibleWindow(hWnd);
		}

	}
	return TRUE;
}

BOOL CALLBACK enumMaximizedWindows(HWND hWnd, LPARAM lParam)
{
	if (IsWindowVisible(hWnd) && !IsIconic(hWnd)) {
		std::string name = windowController->getNameOfWindow(hWnd);
		if (name != "Program Manager" && name != "Start Menu" && name != "Start" && name.length() > 1) {
			windowController->insertNewMaximizedWindow(hWnd);
		}
	}
	return TRUE;
}



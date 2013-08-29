#include "stdafx.h"
#include "WindowController.h"

using namespace std;

HWND activeWindow;
HWND lastActiveWindow;
int lastActiveWindowIndex;

short wheel_delta, old_wheel_delta = 0;
bool keyDown = false;
bool enumWindows = false;

//bool is a flag to test if its already been seen when enumerating
vector<pair<HWND, bool>> visibleWindows;
vector<HWND> maximizedWindows;
vector<char> input;

void WindowController::clearVisibleWindows() {
	visibleWindows.erase(visibleWindows.begin(), visibleWindows.end());
}

void WindowController::clearMaximizedWindows() {
	maximizedWindows.erase(maximizedWindows.begin(), maximizedWindows.end());
}

void WindowController::insertNewVisibleWindow(HWND hWnd) {
	bool found = false;
	for each (pair<HWND, bool> p in visibleWindows)
	{
		if (p.first == hWnd) {
			found = true;
			break;
		}
	}

	if (!found) {
		pair<HWND, bool> p(hWnd, true);
		visibleWindows.push_back(p);
	}
}

void WindowController::insertNewMaximizedWindow(HWND hWnd) {
	maximizedWindows.push_back(hWnd);
}

void WindowController::setWheelDelta(short new_delta) {
	wheel_delta = new_delta;
}

void WindowController::enumWindowsComplete() {
	enumWindows = false;
}

bool WindowController::updateEnumWindows() {
	return enumWindows;
}

short WindowController::getWheelDelta(void) {
	return wheel_delta;
}

void WindowController::cleanUpVisibleWindows() {
	int index = 0;
	for each (pair<HWND, bool> p in visibleWindows)
	{
		//if it wasnt seen in the last enum, delete
		if (p.second == false) {
			visibleWindows.erase(visibleWindows.begin() + index);
		}
		else{
		//reset for next time
			//visibleWindows[index].second = false;
		}
		index++;
	}
}

void WindowController::forceForegroundWindow(HWND theWindow) {
	DWORD nForeThread = GetWindowThreadProcessId(GetForegroundWindow(), 0);
	DWORD nAppThread = GetCurrentThreadId();

	if (nForeThread != nAppThread) {
		AttachThreadInput(nForeThread, nAppThread, true);
		BringWindowToTop(theWindow);
		ShowWindow(theWindow, SW_RESTORE);
		AttachThreadInput(nForeThread, nAppThread, false);
	}
	else{
		BringWindowToTop(theWindow);
		ShowWindow(theWindow, SW_RESTORE);
	}

	activeWindow = theWindow;
}

HWND WindowController::getActiveWindowHandler(void) {
	HWND activeWindowHandle = GetForegroundWindow();
	return activeWindowHandle;
}

string WindowController::getNameOfWindow(HWND handle) {
	char title[250];
	GetWindowText(handle, title, sizeof(title));
	string stringTitle(title);
	return stringTitle;
}

void WindowController::printNameOfWindow(HWND handle) {
	CHAR s[256];
	sprintf_s(s, "Name: %s\n", getNameOfWindow(handle).c_str());
	OutputDebugString(s);
}

void WindowController::printVisibleWindows(HWND selection = NULL) {
	for each (pair<HWND, bool> p in visibleWindows)
	{
		HWND currentHandle = p.first;
		if (selection == currentHandle) {
			debugString("**->");
		}
		printNameOfWindow(currentHandle);
	}
}

void WindowController::minimizeWindow(HWND handle) {
	// if window is NOT minimized
	if (IsWindowVisible(handle) && !IsIconic(handle)) {
		ShowWindow(handle, SW_MINIMIZE);
	}
}

bool WindowController::isHandlerValid(HWND handle) {
	string title = getNameOfWindow(handle);
	if (title.length() < 1 || title.compare("Program Manager") == 0) {
		return false;
	}
	return true;
}

void WindowController::minimizeAllWindows(vector<HWND>* exception = NULL) {
	for each(pair<HWND, bool> p in visibleWindows) {
		HWND windowHandler = p.first;
		if (exception != NULL) {
			vector<HWND>::iterator it = std::find(exception->begin(), exception->end(), windowHandler);
			//There are exceptions but the current windowHandler is not in it
			if (it == exception->end()) {
				if (isHandlerValid(windowHandler)) {
					minimizeWindow(windowHandler);
				}
			}
		}
		else if (isHandlerValid(windowHandler)) {
				minimizeWindow(windowHandler);
		}
	}
}

int WindowController::getIndexOfWindow(HWND activeWindow, vector<pair<HWND, bool>> windowList) {
	int i = 0;
	for (int i = 0; i < windowList.size(); i++) {
		if (windowList[i].first == activeWindow) {
			return i;
		}
	}

	CloseWindow(activeWindow);
	ShowWindow(activeWindow, SW_MINIMIZE);
	forceForegroundWindow(windowList[lastActiveWindowIndex].first);
	return lastActiveWindowIndex;
}

int WindowController::getIndexOfActiveWindow(vector<pair<HWND, bool>> windowList) {
	activeWindow = getActiveWindowHandler();
	return getIndexOfWindow(activeWindow, windowList);
}

int WindowController::getNextIndex(int currentIndex, vector<pair<HWND, bool>> windowList) {
	if (currentIndex >= windowList.size() - 1) {
		return 0;
	}
	int nextIndex = currentIndex + 1;
	return nextIndex;
}

int WindowController::getPreviousIndex(int currentIndex, vector<pair<HWND, bool>> windowList) {
	if (currentIndex <= 0) {
		return windowList.size() - 1;
	}
	int previousIndex = currentIndex - 1;
	return previousIndex;
}

void WindowController::switchWindowLeft(vector<pair<HWND, bool>> windowList) {
	int index = getIndexOfActiveWindow(windowList);
	lastActiveWindowIndex = index;
	int nextIndex = getNextIndex(index, windowList);
	HWND nextHandler = windowList[nextIndex].first;
	lastActiveWindow = getActiveWindowHandler();
	ShowWindow(getActiveWindowHandler(), SW_MINIMIZE);
	forceForegroundWindow(nextHandler);
	printVisibleWindows(nextHandler);
}

void WindowController::switchWindowRight(vector<pair<HWND, bool>> windowList) {
	int index = getIndexOfActiveWindow(windowList);
	lastActiveWindowIndex = index;
	int previousIndex = getPreviousIndex(index, windowList);
	HWND nextHandler = windowList[previousIndex].first;
	lastActiveWindow = getActiveWindowHandler();
	ShowWindow(getActiveWindowHandler(), SW_MINIMIZE);
	forceForegroundWindow(nextHandler);
	printVisibleWindows(nextHandler);
}

void WindowController::checkForScroll() {
	if ((wheel_delta > (old_wheel_delta)) && isPressed(VK_LWIN)) {
		//means we need to update our array of windows
		enumWindows = true;
		disableStartMenu = true;
		switchWindows(0, visibleWindows);
		old_wheel_delta, wheel_delta = 0;
	}
	else if ((wheel_delta < (old_wheel_delta)) && isPressed(VK_LWIN)) {
		enumWindows = true;
		disableStartMenu = true;
		switchWindows(1, visibleWindows);
		old_wheel_delta, wheel_delta = 0;
	}
}

void WindowController::debugString(string s) {
	CHAR b[256];
	sprintf_s(b, "%s\n", s.c_str());
	OutputDebugString(b);
}

template <class T>
void WindowController::debugNum(T num) {
	CHAR b[256];
	sprintf_s(b, "%d\n", num);
	OutputDebugString(b);
}

void WindowController::setDefaultActiveWindow() {
	if (visibleWindows.size() < 1) {
		return;
	}
	if (!activeWindow) {
		activeWindow = visibleWindows[0].first;
	}
	forceForegroundWindow(activeWindow);
}

void WindowController::switchWindows(int direction, vector<pair<HWND, bool>> windowList) {
	
	if (windowList.size() < 1) {
		return;
	}
	string nameOfActive = getNameOfWindow(getActiveWindowHandler());
	// Hitting the windows key may bring up the start menu, in which case would be our active window.
	// In such case we want to minimize and continue on
	if (nameOfActive.compare("Start") == 0 || nameOfActive.compare("Start menu") == 0) {
		debugString("At start menu");
		setDefaultActiveWindow();
	}
	debugString("+++--------------------+++");
	switch (direction)
	{
	case 0: 
		switchWindowLeft(windowList);
		break;
	case 1:
		switchWindowRight(windowList);
		break;
	default:
		switchWindowLeft(windowList);
		break;
	}
}

void WindowController::splitWindows() {
	int numVisibleWindows = visibleWindows.size();
	HWND activeWindow = getActiveWindowHandler();
	//split active window

	int index = getIndexOfActiveWindow(visibleWindows);
	int previousIndex = index;
	HWND next = NULL;
	while (previousIndex == index || !next) {
		lastActiveWindowIndex = index;
		previousIndex = getPreviousIndex(index, visibleWindows);
		next = visibleWindows[previousIndex].first;
	}
	vector<HWND>* exceptions = new vector<HWND>();
	exceptions->push_back(activeWindow);
	exceptions->push_back(next);
	minimizeAllWindows(exceptions);
	forceForegroundWindow(next);
	forceForegroundWindow(activeWindow);

	debugString("Setting Active to the left:");
	SetWindowPos(activeWindow, NULL, 0, 0, HALF_SCREEN_X, SCREEN_HEIGHT, SWP_NOZORDER);
	printNameOfWindow(activeWindow);
	debugString("Setting other to the right:");
	SetWindowPos(next, NULL, HALF_SCREEN_X, 0, HALF_SCREEN_X, SCREEN_HEIGHT, SWP_NOZORDER);
	printNameOfWindow(next);
}

bool WindowController::isPressed(int key) {
	if (std::find(keysPressed.begin(), keysPressed.end(), key) != keysPressed.end()) {
		return true;
	}
	return false;
}

void WindowController::checkKeyStates() {
	//Windows + 7 (Left)
	/*debugString("==================");
	for (int i = 0; i < keysPressed.size(); i++) {
		debugNum(keysPressed[i]);
	}*/

	if (isPressed(VK_LWIN) && isPressed(VK_NUMPAD7)) {
		if (!keyDown) {
			enumWindows = true;
			switchWindows(0, visibleWindows);
			keyDown = true;
		}
	}
	else
	//Windows + 9 (Right)
	if (isPressed(VK_LWIN) && isPressed(VK_NUMPAD9)) {
		if (!keyDown) {
			enumWindows = true;
			switchWindows(1, visibleWindows);
			keyDown = true;
		}
	}
	//Windows +  Split(1)
	if (isPressed(VK_LWIN) && isPressed(VK_NUMPAD1)) {
		if (!keyDown) {
			enumWindows = true;
			splitWindows();
			keyDown = true;
		}
	}
	else{
		keyDown = false;
	}
}

WindowController::WindowController()
{
	disableStartMenu = false;
	windowsKeyPressed = false;

	RECT taskBarRect;
	int taskBarHeight = 0;
	HWND taskBar = FindWindow("Shell_traywnd", NULL);
	if (taskBar && GetWindowRect(taskBar, &taskBarRect)) {
		taskBarHeight = taskBarRect.bottom - taskBarRect.top;
	}
	RECT actualDesktop;
	GetWindowRect(GetDesktopWindow(), &actualDesktop);
	SCREEN_WIDTH = actualDesktop.right - actualDesktop.left;
	SCREEN_HEIGHT = actualDesktop.bottom - actualDesktop.top - taskBarHeight;
	HALF_SCREEN_X = (int) SCREEN_WIDTH / 2;
	HALF_SCREEN_Y = (int) SCREEN_HEIGHT / 2;
}


WindowController::~WindowController()
{
}

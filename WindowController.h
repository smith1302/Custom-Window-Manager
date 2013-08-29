#pragma once
#include <vector>
#include <string>
#include <atlstr.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <algorithm>
#include <unordered_map>

class WindowController
{
public:
	bool disableStartMenu;
	bool windowsKeyPressed;
	int keyPressed;
	std::vector<int> keysPressed;

	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;
	int HALF_SCREEN_X;
	int HALF_SCREEN_Y;

	static const int KEY_J = 0x4A;
	static const int KEY_A = 0x41;
	static const int KEY_1 = 0x41;

	void clearVisibleWindows();
	void clearMaximizedWindows();
	void insertNewVisibleWindow(HWND hWnd);
	void insertNewMaximizedWindow(HWND hWnd);
	void setWheelDelta(short new_delta);
	void enumWindowsComplete();
	bool updateEnumWindows();

	short getWheelDelta(void);

	void cleanUpVisibleWindows();
	std::string getNameOfWindow(HWND handle);
	void forceForegroundWindow(HWND theWindow);
	void getAllVisibleWindows(void);
	void getAllMaximizedWindows(void);
	HWND getActiveWindowHandler(void);
	void printNameOfWindow(HWND handle);
	void printVisibleWindows(HWND selection);
	void minimizeWindow(HWND handle);
	bool isHandlerValid(HWND handle);
	void minimizeAllWindows(std::vector<HWND>* exception);
	int getIndexOfWindow(HWND activeWindow, std::vector<std::pair<HWND, bool> > windowList);
	int getIndexOfActiveWindow(std::vector< std::pair<HWND, bool> > windowList);
	int getNextIndex(int currentIndex, std::vector<std::pair< HWND, bool> > windowList);
	int getPreviousIndex(int currentIndex, std::vector<std::pair<HWND, bool> > windowList);
	void switchWindows(int direction, std::vector< std::pair<HWND, bool> > windowList);
	void switchWindowLeft(std::vector<std::pair<HWND, bool> > windowList);
	void switchWindowRight(std::vector<std::pair<HWND, bool> > windowList);
	void splitWindows();
	void checkForScroll();
	void checkKeyStates();
	void setDefaultActiveWindow();
	bool isPressed(int key);

	void debugString(std::string s);
	template <class T> void debugNum(T num);

	WindowController();
	~WindowController();
};


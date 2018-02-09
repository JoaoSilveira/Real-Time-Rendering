#pragma once

#include <windows.h>

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow);

LRESULT WINAPI EntryMessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
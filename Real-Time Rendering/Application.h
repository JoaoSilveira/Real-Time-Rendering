#pragma once
#include <windows.h>

class CHostApplication
{
public:
    CHostApplication();
    virtual ~CHostApplication();

    LRESULT WINAPI MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void Go();

protected:
    HWND _hWnd;

    LONG _windowWidth;
    LONG _windowHeight;
};
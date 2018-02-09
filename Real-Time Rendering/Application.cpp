#include "Executable.h"
#include "Application.h"

CHostApplication::CHostApplication() : _hWnd(nullptr), _windowWidth(640), _windowHeight(480)
{
}

CHostApplication::~CHostApplication()
{
    if (_hWnd)
        DestroyWindow(_hWnd);
}

void CHostApplication::Go()
{
    WNDCLASSEX windowClass = {sizeof(WNDCLASSEX), CS_CLASSDC,
                EntryMessageHandler, 0, 0, GetModuleHandle(nullptr),
                nullptr, nullptr, nullptr, nullptr, "Host Application", nullptr};

    RegisterClassEx(&windowClass);

    _hWnd = CreateWindow("Host Application", "Host Application", WS_OVERLAPPEDWINDOW, 0, 0,
            _windowWidth, _windowHeight, GetDesktopWindow(), NULL, windowClass.hInstance, NULL);

    if (_hWnd)
        ShowWindow(_hWnd, SW_SHOW);
    else
        return;
    
    MSG message;

    PeekMessage(&message, nullptr, 0, 0, PM_REMOVE);
    while(message.message != WM_QUIT)
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
        PeekMessage(&message, nullptr, 0, 0, PM_REMOVE);
    }
}

LRESULT WINAPI CHostApplication::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
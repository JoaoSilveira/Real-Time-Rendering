#include "Executable.h"
#include "Application.h"

static CHostApplication *hostApplication = nullptr;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
    hostApplication = new CHostApplication();

    hostApplication->Go();

    delete hostApplication;

    return 0;
}

LRESULT WINAPI EntryMessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return hostApplication->MessageHandler(hWnd, message, wParam, lParam);
}
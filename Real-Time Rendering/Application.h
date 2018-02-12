#pragma once
#include <windows.h>
#include <d3d9.h>
#include <dxgi.h>
#include <d3dx9.h>

#pragma comment (lib, "d3dx9.lib") // necessary because of god knows why https://stackoverflow.com/questions/33486139/unresolved-external-symbol-error-c-vs15-d3dx9-library

class CHostApplication
{
public:
    CHostApplication();
    virtual ~CHostApplication();

    LRESULT WINAPI MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void Go();

protected:
    HWND _hWnd;
	LPDIRECT3DDEVICE9 d3dDevice;
	LPDIRECT3D9 d3d;

	D3DPRESENT_PARAMETERS presentParameters;
	D3DDEVICE_CREATION_PARAMETERS creationParameters;

	bool _continue;
    LONG _windowWidth;
    LONG _windowHeight;

	HRESULT InitializeD3D();
	HRESULT CreateDevice(D3DDEVICE_CREATION_PARAMETERS * createParameters, D3DPRESENT_PARAMETERS *presentParameters);
	HRESULT EasyCreateWindowed(HWND windowHandle, D3DDEVTYPE deviceType, DWORD behavior);
	HRESULT EasyCreateFullScreen(D3DDISPLAYMODE *displayModes, D3DDEVTYPE deviceType, DWORD behavior);
	HRESULT RestoreDevice();
	long EnumerateModes(D3DDISPLAYMODE *displayModes, long modeCount);
	HRESULT DestroyDevice();

	virtual bool PreInitialize();
	virtual bool PostInitialize();
	virtual bool PreTerminate();
	virtual bool PostTerminate();
	virtual void PreRender();
	virtual void Render();
	virtual HRESULT PostRender();
	virtual bool PreReset();
	virtual bool PostReset();
	virtual bool HandleMessage(MSG *message);
};
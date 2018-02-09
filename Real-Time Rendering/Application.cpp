#include "Executable.h"
#include "Application.h"

CHostApplication::CHostApplication() : _hWnd(nullptr), d3dDevice(nullptr), d3d(nullptr), _continue(false), _windowWidth(640), _windowHeight(480)
{
}

HRESULT CHostApplication::InitializeD3D()
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	return S_OK;
}

HRESULT CHostApplication::CreateDevice(D3DDEVICE_CREATION_PARAMETERS * createParameters, D3DPRESENT_PARAMETERS * presentParameters)
{
	memcpy(&creationParameters, createParameters, sizeof(D3DDEVICE_CREATION_PARAMETERS));
	memcpy(&this->presentParameters, presentParameters, sizeof(D3DPRESENT_PARAMETERS));

	return d3d->CreateDevice(createParameters->AdapterOrdinal, createParameters->DeviceType,
		createParameters->hFocusWindow, createParameters->BehaviorFlags, presentParameters, &d3dDevice);
}

HRESULT CHostApplication::EasyCreateWindowed(HWND windowHandle, D3DDEVTYPE deviceType, DWORD behavior)
{
	D3DDISPLAYMODE currentMode;

	if (SUCCEEDED(d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &currentMode)))
	{
		ZeroMemory(&presentParameters, sizeof(D3DPRESENT_PARAMETERS));

		presentParameters.Windowed = true;
		presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		presentParameters.BackBufferFormat = currentMode.Format;
		presentParameters.EnableAutoDepthStencil = true;
		presentParameters.AutoDepthStencilFormat = D3DFMT_D16;

		creationParameters.AdapterOrdinal = D3DADAPTER_DEFAULT;
		creationParameters.DeviceType = deviceType;
		creationParameters.hFocusWindow = windowHandle;
		creationParameters.BehaviorFlags = behavior;

		return CreateDevice(&creationParameters, &presentParameters);
	}

	return E_FAIL;
}

HRESULT CHostApplication::EasyCreateFullScreen(D3DDISPLAYMODE * displayModes, D3DDEVTYPE deviceType, DWORD behavior)
{
	ZeroMemory(&presentParameters, sizeof(D3DPRESENT_PARAMETERS));

	presentParameters.Windowed = false;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferWidth = displayModes->Width;
	presentParameters.BackBufferHeight = displayModes->Height;
	presentParameters.BackBufferFormat = displayModes->Format;
	presentParameters.FullScreen_RefreshRateInHz = displayModes->RefreshRate;
	presentParameters.EnableAutoDepthStencil = true;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;

	creationParameters.AdapterOrdinal = D3DADAPTER_DEFAULT;
	creationParameters.DeviceType = deviceType;
	creationParameters.BehaviorFlags = behavior;
	creationParameters.hFocusWindow = _hWnd;

	return CreateDevice(&creationParameters, &presentParameters);
}

HRESULT CHostApplication::RestoreDevice()
{
	auto result = d3dDevice->TestCooperativeLevel();

	while (result == D3DERR_DEVICELOST)
	{
		while (result != D3DERR_DEVICENOTRESET)
		{
			Sleep(1000);
			MSG message;

			PeekMessage(&message, nullptr, 0, 0, 0);
			TranslateMessage(&message);
			DispatchMessage(&message);

			result = d3dDevice->TestCooperativeLevel();
		}

		if (FAILED(d3dDevice->Reset(&presentParameters)))
			result = D3DERR_DEVICELOST;
	}

	return S_OK;
}

CHostApplication::~CHostApplication()
{
	if (_hWnd)
		DestroyWindow(_hWnd);
	_hWnd = nullptr;

	if (d3d)
		d3d->Release();
	d3d = nullptr;
}

long CHostApplication::EnumerateModes(D3DDISPLAYMODE * displayModes, long modeCount)
{
	long count = d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_UNKNOWN); // It's different from directx 8 calling

	if (modeCount > count)
		modeCount = count;

	for (auto i = 0; i < modeCount; i++)
	{
		d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_UNKNOWN, i, &displayModes[i]);
	}

	return count;
}

HRESULT CHostApplication::DestroyDevice()
{
	if (d3dDevice)
		d3dDevice->Release();
	d3dDevice = nullptr;

	return S_OK;
}

void CHostApplication::Go()
{
	WNDCLASSEX windowClass = { sizeof(WNDCLASSEX), CS_CLASSDC,
				EntryMessageHandler, 0, 0, GetModuleHandle(nullptr),
				nullptr, nullptr, nullptr, nullptr, "Host Application", nullptr };

	RegisterClassEx(&windowClass);

	_hWnd = CreateWindow("Host Application", "Host Application", WS_OVERLAPPEDWINDOW, 0, 0,
		_windowWidth, _windowHeight, GetDesktopWindow(), NULL, windowClass.hInstance, NULL);

	if (_hWnd)
		ShowWindow(_hWnd, SW_SHOW);
	else
		return;

	_continue = true;

	if (!PreInitialize())
		return;

	InitializeD3D();

	if (FAILED(EasyCreateWindowed(_hWnd, D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return;

	if (!PostInitialize())
		return;

	MSG message;

	PeekMessage(&message, nullptr, 0, 0, PM_REMOVE);
	while (message.message != WM_QUIT && _continue)
	{
		PreRender();
		Render();

		if (D3DERR_DEVICELOST == PostRender())
		{
			PreReset();
			RestoreDevice();
			PostReset();
		}

		TranslateMessage(&message);
		DispatchMessage(&message);
		PeekMessage(&message, nullptr, 0, 0, PM_REMOVE);
		_continue = HandleMessage(&message);
	}

	PreTerminate();
	DestroyDevice();
	PostTerminate();
}

void CHostApplication::PreRender()
{
	d3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0),
		1.0f, 0);
	d3dDevice->BeginScene();
}

void CHostApplication::Render()
{
}

HRESULT CHostApplication::PostRender()
{
	d3dDevice->EndScene();
	return d3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

bool CHostApplication::PreReset()
{
	return true;
}

bool CHostApplication::PostReset()
{
	return true;
}

bool CHostApplication::PreInitialize()
{
	return true;
}

bool CHostApplication::PostInitialize()
{
	return true;
}

bool CHostApplication::PreTerminate()
{
	return true;
}

bool CHostApplication::PostTerminate()
{
	return true;
}

bool CHostApplication::HandleMessage(MSG * message)
{
	if (message->message == WM_KEYDOWN && message->wParam == VK_ESCAPE)
		return false;

	return true;
}

LRESULT WINAPI CHostApplication::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
#include "TransformApplication.h"

#define D3DFVF_SIMPLEVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define NUM_VERTICES 40
#define RADIUS 50.0f
#define RANDOM_NUMBER (static_cast<float>(rand()) / RAND_MAX)

struct SIMPLE_VERTEX
{
	float x;
	float y;
	float z;

	DWORD color;
};

CTransformApplication::CTransformApplication(): vertexBuffer(nullptr)
{
}

CTransformApplication::~CTransformApplication()
{
	DestroyVertexBuffer();
}

void CTransformApplication::InitializeViewports()
{
	D3DVIEWPORT9 mainViewport;

	d3dDevice->GetViewport(&mainViewport);

	_rViewport.Width = _srtViewport.Width = _trViewport.Width = _rtrsViewport.Width = mainViewport.Width / 2;
	_rViewport.Height = _srtViewport.Height = _trViewport.Height = _rtrsViewport.Height = mainViewport.Height / 2;

	_rViewport.X = _trViewport.X = 0;
	_rViewport.Y = _srtViewport.Y = 0;

	_srtViewport.X = _rtrsViewport.X = mainViewport.Width / 2;
	_trViewport.Y = _rtrsViewport.Y = mainViewport.Height / 2;

	_rViewport.MinZ = _srtViewport.MinZ = _trViewport.MinZ = _rtrsViewport.MinZ = 0.0f;
	_rViewport.MaxZ = _srtViewport.MaxZ = _trViewport.MaxZ = _rtrsViewport.MaxZ = 1.0f;
}

bool CTransformApplication::CreateVertexBuffer()
{
	if (FAILED(d3dDevice->CreateVertexBuffer(NUM_VERTICES * sizeof(SIMPLE_VERTEX),
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS, D3DFVF_SIMPLEVERTEX,
		D3DPOOL_DEFAULT, &vertexBuffer, nullptr)))
		return false;

	//d3dDevice->SetVertexShader(D3DFVF_SIMPLEVERTEX); // changes to line below in DX9
	d3dDevice->SetFVF(D3DFVF_SIMPLEVERTEX);
	d3dDevice->SetStreamSource(0, vertexBuffer, 0, sizeof(SIMPLE_VERTEX));

	return true;
}

bool CTransformApplication::FillVertexBuffer()
{
	if (!vertexBuffer)
		return false;

	SIMPLE_VERTEX *vertices;

	if (FAILED(vertexBuffer->Lock(0, NUM_VERTICES * sizeof(SIMPLE_VERTEX),
		reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD)))
	{
		DestroyVertexBuffer();
		return false;
	}

	auto vpWidth = static_cast<float>(_windowWidth) / 2;
	auto vpHeight = static_cast<float>(_windowHeight) / 2;

	auto xOffset = vpWidth * RANDOM_NUMBER - static_cast<float>(vpWidth) / 2;
	auto yOffset = vpHeight * RANDOM_NUMBER - static_cast<float>(vpHeight) / 2; // center object, it was on 4th quadrant

	for (auto i = 0; i < NUM_VERTICES; i++)
	{
		auto angle = static_cast<float>(i) / static_cast<float>(NUM_VERTICES) * 2.0f * D3DX_PI;

		vertices[i].x = xOffset + RADIUS * cosf(angle);
		vertices[i].y = yOffset + RADIUS * sinf(angle);
		vertices[i].z = 300.0f; // changed because of perspective projection (object couldn't be seen)
		vertices[i].color = 0xffffffff;
	}

	vertexBuffer->Unlock();
	return true;
}

void CTransformApplication::DestroyVertexBuffer()
{
	if (!vertexBuffer)
		return;

	vertexBuffer->Release();
	vertexBuffer = nullptr;
}

bool CTransformApplication::PostInitialize()
{
	if (FAILED(EasyCreateWindowed(_hWnd, D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return false;

	d3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	D3DXMatrixIdentity(&_viewMatrix);
	d3dDevice->SetTransform(D3DTS_VIEW, &_viewMatrix);

	InitializeViewports();
	return CreateVertexBuffer();
}

bool CTransformApplication::PreTerminate()
{
	DestroyVertexBuffer();
	return true;
}

bool CTransformApplication::PreReset()
{
	DestroyVertexBuffer();
	return true;
}

bool CTransformApplication::PostReset()
{
	return CreateVertexBuffer();
}

void CTransformApplication::Render()
{
	FillVertexBuffer();
	RECT windowRect;

	GetWindowRect(_hWnd, &windowRect);

	D3DXMatrixPerspectiveFovLH(&_projectionMatrix, D3DX_PI / 4.0f, 
		static_cast<float>(windowRect.right - windowRect.left) / static_cast<float>(windowRect.bottom - windowRect.top),
		1.0f, 1000.0f);
	d3dDevice->SetTransform(D3DTS_PROJECTION, &_projectionMatrix);

	D3DXMATRIX rotationMatrix1;
	D3DXMATRIX rotationMatrix2;
	D3DXMATRIX translationMatrix;
	D3DXMATRIX scalingMatrix;

	D3DVIEWPORT9 mainViewport;

	d3dDevice->GetViewport(&mainViewport);

	D3DXMatrixRotationZ(&rotationMatrix1, static_cast<float>(GetTickCount()) / 1000.0f);
	D3DXMatrixRotationZ(&rotationMatrix2, -static_cast<float>(GetTickCount()) / 1000.0f);

	D3DXMatrixTranslation(&translationMatrix, 30.0f, 0.0f, 0.0f); // changed because of the scale
	D3DXMatrixScaling(&scalingMatrix, 1.0f, 0.5f, 1.0f);

	d3dDevice->SetViewport(&_rViewport);
	_worldMatrix = rotationMatrix1;
	d3dDevice->SetTransform(D3DTS_WORLD, &_worldMatrix);
	d3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, NUM_VERTICES);

	d3dDevice->SetViewport(&_srtViewport);
	_worldMatrix = translationMatrix * rotationMatrix1 * scalingMatrix;
	d3dDevice->SetTransform(D3DTS_WORLD, &_worldMatrix);
	d3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, NUM_VERTICES);

	d3dDevice->SetViewport(&_trViewport);
	_worldMatrix = rotationMatrix1 * translationMatrix;
	d3dDevice->SetTransform(D3DTS_WORLD, &_worldMatrix);
	d3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, NUM_VERTICES);

	d3dDevice->SetViewport(&_rtrsViewport);
	_worldMatrix = scalingMatrix * rotationMatrix2 * translationMatrix * rotationMatrix1;
	d3dDevice->SetTransform(D3DTS_WORLD, &_worldMatrix);
	d3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, NUM_VERTICES);

	d3dDevice->SetViewport(&mainViewport);
}
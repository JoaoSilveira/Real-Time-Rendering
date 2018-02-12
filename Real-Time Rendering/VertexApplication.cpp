#include "VertexApplication.h"

#define D3DFVF_SIMPLEVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
#define NUM_VERTICES 1000
#define RADIUS 50.0f
#define RANDOM_NUMBER (static_cast<float>(rand()) / RAND_MAX)

struct SIMPLE_VERTEX
{
	float x;
	float y;
	float z;
	float rhw;
	DWORD color;
};

CVertexApplication::CVertexApplication(): vertexBuffer(nullptr)
{
}

CVertexApplication::~CVertexApplication()
{
	DestroyVertexBuffer();
}

bool CVertexApplication::CreateVertexBuffer()
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

bool CVertexApplication::FillVertexBuffer()
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

	auto xOffset = _windowWidth * RANDOM_NUMBER;
	auto yOffset = _windowHeight * RANDOM_NUMBER;

	for (auto i = 0; i < NUM_VERTICES; i++)
	{
		auto angle = static_cast<float>(i) / static_cast<float>(NUM_VERTICES) * 2.0f * D3DX_PI;
		
		vertices[i].x = xOffset + RADIUS * cosf(angle);
		vertices[i].y = yOffset + RADIUS * sinf(angle);
		vertices[i].z = 1.0f;
		vertices[i].rhw = 1.0f;
		vertices[i].color = 0xffffffff;
	}
	
	vertexBuffer->Unlock();
	return true;
}

void CVertexApplication::DestroyVertexBuffer()
{
	if (!vertexBuffer)
		return;

	vertexBuffer->Release();
	vertexBuffer = nullptr;
}

bool CVertexApplication::PostInitialize()
{
	if (FAILED(EasyCreateWindowed(_hWnd, D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return false;

	return CreateVertexBuffer();
}

bool CVertexApplication::PreTerminate()
{
	DestroyVertexBuffer();
	return true;
}

bool CVertexApplication::PreReset()
{
	DestroyVertexBuffer();
	return true;
}

bool CVertexApplication::PostReset()
{
	return CreateVertexBuffer();
}

void CVertexApplication::Render()
{
	FillVertexBuffer();
	d3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, NUM_VERTICES);
}

#pragma once
#include "Application.h"

class CTransformApplication : public CHostApplication
{
	D3DXMATRIX _worldMatrix;
	D3DXMATRIX _viewMatrix;
	D3DXMATRIX _projectionMatrix;

	D3DVIEWPORT9 _rViewport;
	D3DVIEWPORT9 _srtViewport;
	D3DVIEWPORT9 _trViewport;
	D3DVIEWPORT9 _rtrsViewport;

	bool flick;

public:
	CTransformApplication();
	virtual ~CTransformApplication();

	void InitializeViewports();
	bool CreateVertexBuffer();
	bool FillVertexBuffer();
	void DestroyVertexBuffer();

protected:
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;

	bool PostInitialize() override;
	bool PreTerminate() override;
	bool PreReset() override;
	bool PostReset() override;
	void Render() override;
	bool HandleMessage(MSG *message) override;

};


#pragma once
#include "Application.h"

class CVertexApplication : public CHostApplication
{
public:
	CVertexApplication();
	virtual ~CVertexApplication();

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
};
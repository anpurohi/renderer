#pragma once

#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include "..\inc\util.h"
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

struct VertexType
{
    XMFLOAT3 position;
    XMFLOAT4 color;
};

class ModelClass
{
public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

    HRESULT Initialize(ID3D11Device*);
    void Shutdown();
    void Render(ID3D11DeviceContext*);
    
    UINT GetIndexCount();

private:
    HRESULT InitializeBuffer(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

private:
    ID3D11Buffer* m_pVertexBuffer;
    ID3D11Buffer* m_pIndexBuffer;
    UINT m_vertexCount;
    UINT m_indexCount;
};

#endif

#pragma once

#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include "..\inc\util.h"
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#if USE_TEXTURE
#include "TextureClass.h"
#endif

struct VertexType
{
    XMFLOAT3 position;

#if USE_TEXTURE
    XMFLOAT2 texture;
#else
    XMFLOAT4 color;
#endif
};

class ModelClass
{
public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

#if USE_TEXTURE
    HRESULT Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
#else
    HRESULT Initialize(ID3D11Device*);
#endif
    void Shutdown();
    void Render(ID3D11DeviceContext*);
    
    UINT GetIndexCount();

    void GetRotationMatrix(XMMATRIX&);
    void GetScaleMatrix(XMMATRIX&);
    void GetTranslationMatrix(XMMATRIX&);
    void GetTransformationMatrix(XMMATRIX&);

    void SetRotation(float, float, float);
    void SetScaling(float, float, float);
    void SetTranslation(float, float, float);

    void Rotate(float, float, float);
    void Scale(float, float, float);
    void Translate(float, float, float);

#if USE_TEXTURE
    ID3D11ShaderResourceView* GetTexture();
#endif

private:
    HRESULT InitializeBuffer(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

#if USE_TEXTURE
    HRESULT LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
    void ReleaseTexture();
#endif

private:
    ID3D11Buffer* m_pVertexBuffer;
    ID3D11Buffer* m_pIndexBuffer;

    UINT m_vertexCount;
    UINT m_indexCount;

    XMMATRIX m_rotationMatrix;
    XMMATRIX m_scalingMatrix;
    XMMATRIX m_translationMatrix;

#if USE_TEXTURE
    TextureClass* m_pTexture;
#endif
};

#endif

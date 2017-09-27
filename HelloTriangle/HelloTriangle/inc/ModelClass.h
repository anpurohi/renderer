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

private:
    HRESULT InitializeBuffer(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

//    void UpdateRotationMatrix();
//    void UpdateScaleMatrix();
//    void UpdateTranslationMatrix();

private:
    ID3D11Buffer* m_pVertexBuffer;
    ID3D11Buffer* m_pIndexBuffer;

    UINT m_vertexCount;
    UINT m_indexCount;

//    XMFLOAT3 m_rotation;
//    XMFLOAT3 m_scaling;
//    XMFLOAT3 m_translation;

    XMMATRIX m_rotationMatrix;
    XMMATRIX m_scalingMatrix;
    XMMATRIX m_translationMatrix;
};

#endif

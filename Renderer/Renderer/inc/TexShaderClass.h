#pragma once

#ifndef _TEXSHADERCLASS_H_
#define _TEXSHADERCLASS_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

#include "util.h"

using namespace DirectX;
using namespace std;

class TexShaderClass
{
public:
    TexShaderClass();
    TexShaderClass(const TexShaderClass&);
    ~TexShaderClass();

    HRESULT Initialize(ID3D11Device*, HWND);
    void Shutdown();
    HRESULT Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

private:
    HRESULT InitializeShader(ID3D11Device*, HWND, LPCWSTR, LPCWSTR);
    void ShutdownShader();

    HRESULT SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_pVS;
    ID3D11PixelShader*  m_pPS;
    ID3D11InputLayout*  m_pLayout;
    ID3D11Buffer*       m_pMatrixBuffer;
    ID3D11SamplerState* m_pSamplerState;
};

#endif

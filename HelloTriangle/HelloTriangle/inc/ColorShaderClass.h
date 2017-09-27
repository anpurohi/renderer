#pragma once

#ifndef _COLORSHADERCLASS_H_
#define _COLORSHADERCLASS_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

#include "util.h"

using namespace DirectX;
using namespace std;

class ColorShaderClass
{
private:
    ID3D11VertexShader* m_pVS;
    ID3D11PixelShader*  m_pPS;
    ID3D11InputLayout*  m_pLayout;
    ID3D11Buffer*       m_pMatrixBuffer;

public:
    ColorShaderClass();
    ColorShaderClass(const ColorShaderClass&);
    ~ColorShaderClass();

    HRESULT Initialize(ID3D11Device*, HWND);
    void Shutdown();
    HRESULT Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX);

private:
    HRESULT InitializeShader(ID3D11Device*, HWND, LPCWSTR, LPCWSTR);
    void ShutdownShader();

    HRESULT SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX);
    void RenderShader(ID3D11DeviceContext*, int);
};

#endif

#pragma once
#ifndef _DX11CLASS_H_
#define _DX11CLASS_H_

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class Dx11Class
{
public:
    Dx11Class();
    Dx11Class(const Dx11Class&);
    ~Dx11Class();

    HRESULT Initialize(int, int, bool, HWND, bool, float, float);
    void Shutdown();

    void BeginScene(float, float, float, float);
    void EndScene();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    void GetProjectionMatrix(XMMATRIX&);
    void GetWorldMatrix(XMMATRIX&);
    void GetOrthoMatrix(XMMATRIX&);

    void GetVideoCardInfo(char*, int&);

private:
    bool                     m_vsync_enabled;
    int                      m_videoCardMemory;
    char                     m_videoCardDescription[128];

    IDXGISwapChain*          m_pSwapChain;
    ID3D11Device*            m_pDevice;
    ID3D11DeviceContext*     m_pDeviceContext;
    ID3D11RenderTargetView*  m_pRTV;
    ID3D11DepthStencilView*  m_pDSV;
    ID3D11Texture2D*         m_pDepthStencilBuffer;
    ID3D11DepthStencilState* m_pDepthStencilState;
    ID3D11RasterizerState*   m_pRasterizerState;

    XMMATRIX                 m_projectionMatrix;
    XMMATRIX                 m_pWorldMatrix;
    XMMATRIX                 m_pOrthographicMatrix;
};

#endif

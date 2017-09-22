#include "..\inc\Dx11Class.h"

Dx11Class::Dx11Class()
{
    m_pSwapChain          = nullptr;
    m_pDevice             = nullptr;
    m_pDeviceContext      = nullptr;
    m_pRTV                = nullptr;
    m_pDSV                = nullptr;
    m_pDepthStencilBuffer = nullptr;
    m_pDepthStencilState  = nullptr;
    m_pRasterizerState    = nullptr;
}

Dx11Class::Dx11Class(const Dx11Class&)
{
}

Dx11Class::~Dx11Class()
{
}

HRESULT Dx11Class::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
                              float screenDepth, float screenNear)
{
    HRESULT hr;

    float fieldOfView, screenAspect;

    // Create a DirectX graphics interface factory
    IDXGIFactory* pFactory;
    hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
    if (FAILED(hr))
    {
        return hr;
    }

    // Use the factory to create an adapter for the primary 
    // graphics interface (video card)
    IDXGIAdapter* pAdapter;
    hr = pFactory->EnumAdapters(0, &pAdapter);
    if (FAILED(hr))
    {
        return hr;
    }

    // Enumerate the primary adapter output (monitor)
    IDXGIOutput* pAdapterOutput;
    hr = pAdapter->EnumOutputs(0, &pAdapterOutput);
    if (FAILED(hr))
    {
        return hr;
    }

    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM 
    // display format for the adapter output (monitor)
    UINT numModes;
    hr = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
    if (FAILED(hr))
    {
        return hr;
    }

    // Create a list to hold all the possible display modes
    // for this monitor / video card combination
    DXGI_MODE_DESC* pDisplayModeList = new DXGI_MODE_DESC[numModes];
    if (!pDisplayModeList)
    {
        return E_OUTOFMEMORY;
    }

    // Fill the display mode list structures
    hr = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes,
                                            pDisplayModeList);
    if (FAILED(hr))
    {
        return hr;
    }

    UINT numerator, denominator;
    // Now go through all the display modes and find the one that matches the screen width and height.
    for (int i = 0; i < numModes; i++)
    {
        if ((pDisplayModeList[i].Width == (UINT)screenWidth) &&
            (pDisplayModeList[i].Height == (UINT)screenHeight))
        {
            // When a match is found, store the numerator 
            // and the denominator of the refresh rate for that monitor
            numerator = pDisplayModeList[i].RefreshRate.Numerator;
            denominator = pDisplayModeList[i].RefreshRate.Denominator;
        }
    }

    // Get the adapter (video card) description 
    DXGI_ADAPTER_DESC adapterDesc;
    hr = pAdapter->GetDesc(&adapterDesc);
    if (FAILED(hr))
    {
        return hr;
    }

    // Store the dedicated video card memory in megabytes
    m_videoCardMemory = (UINT)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    size_t stringLength;
    const size_t descSize = (sizeof(m_videoCardDescription) / sizeof(char));

    // Convert the name of the video card to a char array and store it
    int error = wcstombs_s(&stringLength, m_videoCardDescription, descSize, adapterDesc.Description, descSize);
    if (!error)
    {
        return E_FAIL;
    }

    // Release the intermediate pointers and lists
    delete[] pDisplayModeList;
    pDisplayModeList = nullptr;

    pAdapterOutput->Release();
    pAdapterOutput = nullptr;

    pAdapter->Release();
    pAdapter = nullptr;

    pFactory->Release();
    pFactory = nullptr;

    // TODO: Initialize the device context
    
    // TODO: Initialize the swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDesc;

    // TODO: Initialize the device

    // TODO: Initialize the Render Target

    // TODO: Initialize the Depth Stencil
}

void Dx11Class::Shutdown()
{
    // TODO: Return to windowed mode before releasing the swap chain

    // TODO: Shutdown / release all the pointers
}

void Dx11Class::BeginScene(float, float, float, float)
{
    // TODO: Paint the entire screen with the same color
}

void Dx11Class::EndScene()
{
    // TODO: Present the back buffer to the screen
}

ID3D11Device* Dx11Class::GetDevice()
{
    return m_pDevice;
}

ID3D11DeviceContext* Dx11Class::GetDeviceContext()
{
    return m_pDeviceContext;
}

void Dx11Class::GetProjectionMatrix(XMMATRIX& pProjectionMatrix)
{
    pProjectionMatrix = m_projectionMatrix;
}

void Dx11Class::GetWorldMatrix(XMMATRIX& pWorldMatrix)
{
    pWorldMatrix = m_pWorldMatrix;
}

void Dx11Class::GetOrthoMatrix(XMMATRIX& pOrthoMatrix)
{
    pOrthoMatrix = m_pOrthographicMatrix;
}

void Dx11Class::GetVideoCardInfo(char* cardName, int& memory)
{
    strcpy_s(cardName, 128, m_videoCardDescription);
    memory = m_videoCardMemory;
}

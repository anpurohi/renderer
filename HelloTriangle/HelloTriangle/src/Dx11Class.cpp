#include "..\inc\Dx11Class.h"
#include <minwinbase.h>

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

    m_vsync_enabled = vsync;

    // Create a DirectX graphics interface factory
    IDXGIFactory* pFactory;
    hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
    CHECK_HR_MSG(hr, hwnd, "Unable to create DXGI Factory");

    // Use the factory to create an adapter for the primary 
    // graphics interface (video card)
    IDXGIAdapter* pAdapter;
    hr = pFactory->EnumAdapters(0, &pAdapter);
    CHECK_HR_MSG(hr, hwnd, "Unable to enumerate adapters (video cards)");

    // Enumerate the primary adapter output (monitor)
    IDXGIOutput* pAdapterOutput;
    hr = pAdapter->EnumOutputs(0, &pAdapterOutput);
    CHECK_HR_MSG(hr, hwnd, "Unable to enumerate outputs (monitors)");

    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM 
    // display format for the adapter output (monitor)
    UINT numModes;
    hr = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
    CHECK_HR_MSG(hr, hwnd, "Unable to locate monitor that supports DXGI_FORMAT_R8G8B8A8_UNORM");

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
    CHECK_HR_MSG(hr, hwnd, "Unable to fetch the display mode list");

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
    CHECK_HR_MSG(hr, hwnd, "Unable to fetch adapter description");

    // Store the dedicated video card memory in megabytes
    m_videoCardMemory = (UINT)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    size_t stringLength;
    const size_t descSize = (sizeof(m_videoCardDescription) / sizeof(char));

    // Convert the name of the video card to a char array and store it
    int error = wcstombs_s(&stringLength, m_videoCardDescription, descSize, adapterDesc.Description, descSize);
    if (error)
    {
        return E_FAIL;
    }

    // Release the intermediate pointers and lists
    delete[] pDisplayModeList;
    pDisplayModeList = nullptr;

    SAFE_RELEASE(pAdapterOutput);
    SAFE_RELEASE(pAdapter);
    SAFE_RELEASE(pFactory);
    
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    // Specify the swap chain descriptor
    swapChainDesc.BufferCount                        = 1;
    swapChainDesc.BufferDesc.Width                   = screenWidth;
    swapChainDesc.BufferDesc.Height                  = screenHeight;
    swapChainDesc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator   = m_vsync_enabled ? numerator : 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = m_vsync_enabled ? denominator : 1;
    swapChainDesc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow                       = hwnd;
    swapChainDesc.SampleDesc.Count                   = 1;
    swapChainDesc.SampleDesc.Quality                 = 0;
    swapChainDesc.Windowed                           = !fullscreen;
    swapChainDesc.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags                              = 0;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    UINT flags;

#ifdef _DEBUG
    flags = D3D11_CREATE_DEVICE_DEBUG;
#else
    flags = 0;
#endif

    // Create the swap chain, Direct3D device and the Direct3D device context
    hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, &featureLevel, 1,
            D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, nullptr, &m_pDeviceContext);
    CHECK_HR_MSG(hr, hwnd, "Unable to create the swap chain, device or device context");

    // Get a pointer to the back buffer
    ID3D11Texture2D* pBackBuffer;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    CHECK_HR_MSG(hr, hwnd, "Unable to fetch the back buffer");

    // Create the render target view with the back buffer pointer
    hr = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRTV);
    CHECK_HR_MSG(hr, hwnd, "Unable to create a Render Target View");

    pBackBuffer->Release();
    pBackBuffer = nullptr;

    // Specify the descriptor for the depth buffer
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    depthBufferDesc.Width              = screenWidth;
    depthBufferDesc.Height             = screenHeight;
    depthBufferDesc.MipLevels          = 1;
    depthBufferDesc.ArraySize          = 1;
    depthBufferDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count   = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage              = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.Usage              = D3D11_USAGE_DEFAULT;
    depthBufferDesc.CPUAccessFlags     = 0;
    depthBufferDesc.MiscFlags          = 0;

    // Create the texture for the depth buffer using the filled out descriptor
    hr = m_pDevice->CreateTexture2D(&depthBufferDesc, nullptr, &m_pDepthStencilBuffer);
    CHECK_HR_MSG(hr, hwnd, "Unable to create a 2D texture");

    // Specify the descriptor for the depth stencil state
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    depthStencilDesc.DepthEnable                  = true;
    depthStencilDesc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc                    = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable                = true;
    depthStencilDesc.StencilReadMask              = 0xFF;
    depthStencilDesc.StencilWriteMask             = 0xFF;
    depthStencilDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc         = D3D11_COMPARISON_ALWAYS;

    // Create the depth stencil state
    hr = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState);
    CHECK_HR_MSG(hr, hwnd, "Unable to create a depth stencil state");

    m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

    // Specify the descriptor for the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    depthStencilViewDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view
    hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDSV);
    CHECK_HR_MSG(hr, hwnd, "Unable to create a depth stencil view");

    // Bind the render target view and the depth stencil view to the output render pipeline
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRTV, m_pDSV);

    // Specify the descriptor for the rasterizer
    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(rasterDesc));

    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode              = D3D11_CULL_BACK;
    rasterDesc.DepthBias             = 0;
    rasterDesc.DepthBiasClamp        = 0.f;
    rasterDesc.DepthClipEnable       = true;
    rasterDesc.FillMode              = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable     = false;
    rasterDesc.ScissorEnable         = false;
    rasterDesc.SlopeScaledDepthBias  = 0.f;

    // Create the rasterizer state from the description we just filled out
    hr = m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterizerState);
    CHECK_HR_MSG(hr, hwnd, "Unable to create a rasterizer state");

    // Set the rasterizer state
    m_pDeviceContext->RSSetState(m_pRasterizerState);

    // Setup the viewport for rendering
    D3D11_VIEWPORT viewport;
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    // Create the viewport
    m_pDeviceContext->RSSetViewports(1, &viewport);

    float fieldOfView = 3.141592654f / 4.f;
    float screenAspect = (float)screenWidth / (float)screenHeight;

    // Create the projection matric for 3D rendering
    m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

    // Initialize the world matrix as the identity matrix
    m_pWorldMatrix = DirectX::XMMatrixIdentity();

    // Create an orthographic matrix for 2D rendering
    m_pOrthographicMatrix = DirectX::XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    return S_OK;
}

void Dx11Class::Shutdown()
{
    // Return to windowed mode before releasing the swap chain
    if (m_pSwapChain)
    {
        m_pSwapChain->SetFullscreenState(false, nullptr);
    }

    SAFE_RELEASE(m_pRasterizerState);
    SAFE_RELEASE(m_pDSV);
    SAFE_RELEASE(m_pDepthStencilState);
    SAFE_RELEASE(m_pDepthStencilBuffer);
    SAFE_RELEASE(m_pRTV);
    SAFE_RELEASE(m_pDeviceContext);
    SAFE_RELEASE(m_pDevice);
    SAFE_RELEASE(m_pSwapChain);
}

void Dx11Class::BeginScene(float red, float green, float blue, float alpha)
{
    float color[4];

    // Setup the color to clear the buffer to
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    // Clear the back buffer
    m_pDeviceContext->ClearRenderTargetView(m_pRTV, color);

    // Clear the depth buffer
    m_pDeviceContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Dx11Class::EndScene()
{
    // TODO: Present the back buffer to the screen
    if (m_vsync_enabled)
    {
        // Lock to the screen refresh rate
        m_pSwapChain->Present(1, 0);
    }
    else
    {
        // Present as fast as possible
        m_pSwapChain->Present(0, 0);
    }
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

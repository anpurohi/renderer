#include "..\inc\GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	m_pDx11        = nullptr;
    m_pCamera      = nullptr;
    m_pModel       = nullptr;
    m_pColorShader = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{
}

GraphicsClass::~GraphicsClass()
{
}

HRESULT GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Allocate memory to the Dx 11 manager object
	m_pDx11 = new Dx11Class();
	if (!m_pDx11)
	{
		return E_OUTOFMEMORY;
	}

	// Initialize the newly create Dx 11 manager
	HRESULT hr = m_pDx11->Initialize(screenWidth, screenHeight, VSYNC, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (FAILED(hr))
	{
        MessageBox(hwnd, "Could not initialize Dx 11 manager", "Error", MB_OK);
		return hr;
	}

    // Create the camera object
    m_pCamera = new CameraClass;
    if (!m_pCamera)
    {
        return E_OUTOFMEMORY;
    }

    // Set the initial position of the camera
    m_pCamera->SetPosition(0.0f, 0.0f, -5.0f);

    // Create the model object
    m_pModel = new ModelClass;
    if (!m_pModel)
    {
        return E_OUTOFMEMORY;
    }

    // Initialize the model object
    hr = m_pModel->Initialize(m_pDx11->GetDevice());
    CHECK_HR_MSG(hr, hwnd, "Unable to initialize the model");

    // Create the color shader object
    m_pColorShader = new ColorShaderClass;
    if (!m_pColorShader)
    {
        return E_OUTOFMEMORY;
    }

    // Initialize the color shader object
    hr = m_pColorShader->Initialize(m_pDx11->GetDevice(), hwnd);
    CHECK_HR_MSG(hr, hwnd, "Unable to initialize the color shader");

	return S_OK;
}

void GraphicsClass::Shutdown()
{
    // Release the color shader object
    if (m_pColorShader)
    {
        m_pColorShader->Shutdown();
        delete m_pColorShader;
        m_pColorShader = nullptr;
    }

    // Release the model object
    if (m_pModel)
    {
        m_pModel->Shutdown();
        delete m_pModel;
        m_pModel = nullptr;
    }

    // Release the camera object
    if (m_pCamera)
    {
        delete m_pCamera;
        m_pCamera = nullptr;
    }

	if (m_pDx11)
	{
		// Shutdown the Dx 11 manager object
		m_pDx11->Shutdown();
		delete m_pDx11;
		m_pDx11 = nullptr;
	}
}

HRESULT GraphicsClass::Frame()
{
	// Render the graphics scene
	return Render();
}

HRESULT GraphicsClass::Render()
{
    HRESULT hr;

    XMMATRIX worldMatrix;
    XMMATRIX viewMatrix;
    XMMATRIX projectionMatrix;

	// Clear the buffers to begin the frame
	m_pDx11->BeginScene(0.5f, 0.9f, 0.9f, 1.0f);

    // Generate the view matrix, based on the camera's position
    m_pCamera->Render();

    // Get the world, view and projection matrices from the camera and the d3d objects
    m_pDx11  ->GetWorldMatrix(worldMatrix);
    m_pCamera->GetViewMatrix(viewMatrix);
    m_pDx11  ->GetProjectionMatrix(projectionMatrix);

    // Put the model vertex and the index buffers on the graphics pipeline to prepare them for drawing
    m_pModel->Render(m_pDx11->GetDeviceContext());

    // Render the model using the color shader
    hr = m_pColorShader->Render(m_pDx11->GetDeviceContext(), m_pModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
    CHECK_HR(hr);

	// Present the rendered scene to the screen
	m_pDx11->EndScene();

	return S_OK;
}
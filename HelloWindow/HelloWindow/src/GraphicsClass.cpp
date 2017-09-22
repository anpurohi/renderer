#include "..\inc\GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	m_dx11 = nullptr;
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
	m_dx11 = new Dx11Class();
	if (!m_dx11)
	{
		return E_OUTOFMEMORY;
	}

	// Initialize the newly create Dx 11 manager
	HRESULT hr = m_dx11->Initialize(screenWidth, screenHeight, hwnd);
	if (FAILED(hr))
	{
        MessageBox(hwnd, "Could not initialize Dx 11 manager", "Error", MB_OK);
		return hr;
	}

	return S_OK;
}

void GraphicsClass::Shutdown()
{
	if (m_dx11)
	{
		// Shutdown the Dx 11 manager object
		m_dx11->Shutdown();
		delete m_dx11;
		m_dx11 = nullptr;
	}
}

HRESULT GraphicsClass::Frame()
{
	// Render the graphics scene
	return Render();
}

HRESULT GraphicsClass::Render()
{
	// Clear the buffers to begin the frame
	m_dx11->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	// Present the rendered scene to the screen
	m_dx11->EndScene();

	return S_OK;
}
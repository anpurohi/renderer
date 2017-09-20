#include "..\inc\SystemClass.h"
#include <minwinbase.h>

SystemClass::SystemClass()
{
	
}

HRESULT SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	HRESULT hr;

	screenHeight = 0;
	screenWidth = 0;

	// Initialize the windows api
	InitializeWindows(screenWidth, screenHeight);

	//// Create the input object. 
	//// This object will be used to handle reading the keyboard input from the user.
	//m_input = new InputClass;
	//if (!m_input)
	//{
	//	return E_FAIL;
	//}

	//// Initialize the input object
	//m_input->Initialize();

	//// Create the graphic object.
	//// This object will handle rendering all the graphics for this application.
	//m_graphics = new GraphicsClass;
	//if (!m_graphics)
	//{
	//	return E_FAIL;
	//}

	//// Initialize the graphics object
	//hr = m_graphics->Initialize();
	//if (FAILED(hr))
	//{
	//	return hr;
	//}

	return S_OK;
}

void SystemClass::Run()
{
	MSG msg;
	HRESULT hr;
	bool done;

	// Initialize message structure
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there's a quit message from the window or the user
	done = false;
	while (!done)
	{
		// Handle the windows messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows singals to end the application, then exit
		if (msg.message == WM_QUIT)
		{
			done = true;
		} 
		else
		{
			// Otherwise, process the frame
			hr = Frame();
			if (FAILED(hr))
			{
				done = true;
			}
		}
	}

	return;
}

void SystemClass::Shutdown()
{
	//// Release the graphics object
	//if (m_graphics)
	//{
	//	m_graphics->Shutdown();
	//	delete m_graphics;
	//	m_graphics = nullptr;
	//}

	//// Release the input object
	//if (m_input)
	//{
	//	m_input->Shutdown();
	//	delete m_input;
	//	m_input = nullptr;
	//}

	// Shutdown the window
	ShutdownWindows();
}

SystemClass::SystemClass(const SystemClass&)
{
}

SystemClass::~SystemClass()
{
}

HRESULT SystemClass::Frame()
{
	HRESULT hr;

	//// Check if the user pressed escape and wants to exit the application
	//if (m_input->isKeyDown(VK_ESCAPE))
	//{
	//	return E_FAIL;
	//}

	//// Do the frame processing for the graphics object
	//hr = m_graphics->Frame();
	//if (FAILED(hr))
	//{
	//	return hr;
	//}

	return S_OK;
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external handle to this object.
	ApplicationHandle = this;

	// Get the instance of this application
	m_hInstance = GetModuleHandle(nullptr);

	// Give the application a name
	m_appName = "Engine";

	// Setup the windows class with default settings
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= m_hInstance;
	wc.hIcon			= LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm			= wc.hIcon;
	wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground	= static_cast<HBRUSH> (GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= m_appName;
	wc.cbSize			= sizeof(WNDCLASSEX);

	// Register the window class
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen
	screenHeight	= GetSystemMetrics(SM_CYSCREEN);
	screenWidth		= GetSystemMetrics(SM_CXSCREEN);

	// Setup the screen settings depending on 
	// whether it is running in full screen or in windowed mode
	//if (FULL_SCREEN)
	//{
	//	// If full screen, set the screen to max size of the users desktop and 32bit
	//	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
	//	dmScreenSettings.dmSize			= sizeof(dmScreenSettings);
	//	dmScreenSettings.dmPelsWidth	= static_cast<unsigned long>(screenWidth);
	//	dmScreenSettings.dmPelsHeight	= static_cast<unsigned long>(screenHeight);
	//	dmScreenSettings.dmBitsPerPel	= 32;
	//	dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	//	// Change the display settings to full screen
	//	ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

	//	// Set the position of the window to the top-left corner
	//	posX = posY = 0;
	//}
	//else
	{
		// If windowed, set it to 800x600 resolution
		screenWidth		= 800;
		screenHeight	= 600;

		// Place the window in the middle of the screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_appName, m_appName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, nullptr, nullptr, m_hInstance, nullptr);

	// Bring the window on the screen and set it as main focus
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	// Hide the mouse cursor
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor
	ShowCursor(true);

	//// Fix the display settings if leaving full screen mode
	//if (FULL_SCREEN)
	//{
	//	ChangeDisplaySettings(nullptr, 0);
	//}

	// Remove the window
	DestroyWindow(m_hWnd);
	m_hWnd = nullptr;

	// Remove the application instance
	UnregisterClass(m_appName, m_hInstance);
	m_hInstance = nullptr;

	// Release the pointer to this class
	ApplicationHandle = nullptr;

	return;
}

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// Check if a key has been pressed on the keyboard
	case WM_KEYDOWN:
		// If a key has been pressed, 
		// send it to the input object so that its state can be recorded
		//m_input->keyDown(static_cast<UINT>(wparam));
		return 0;

		// Check if a key has been released on the keyboard
	case WM_KEYUP:
		// If a key has been released,
		// send it to the input object so that its state can be unset
		//m_input->keyUp(static_cast<UINT>(wparam));
		return 0;

		// Any other messages are to be sent to the default message handler
		// This application won't make use of them
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
}

LRESULT __stdcall WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// Check if the window is being destroyed
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// Check if the window is being closed
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

		// All other messages pass to the message handler in the system class
	default:
		return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
	}
}

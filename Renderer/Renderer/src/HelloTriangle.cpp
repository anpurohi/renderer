#include "..\inc\HelloTriangle.h"

#include "..\inc\SystemClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdLine, int iCmdShow)
{
    SystemClass* System;
    HRESULT hr;

    // Create the object
    System = new SystemClass;
    if (!System)
    {
        return 0;
    }

    // Initialize the system
    hr = System->Initialize();
    if (FAILED(hr))
    {
        return 0;
    }

    System->Run();

    // Shutdown and release the system object
    System->Shutdown();
    delete System;
    System = nullptr;

    return 0;
}
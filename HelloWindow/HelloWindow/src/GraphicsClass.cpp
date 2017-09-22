#include "..\inc\GraphicsClass.h"

HRESULT GraphicsClass::Render()
{
	return S_OK;
}

GraphicsClass::GraphicsClass()
{
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{
}

GraphicsClass::~GraphicsClass()
{
}

HRESULT GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	return S_OK;
}

void GraphicsClass::Shutdown()
{
}

HRESULT GraphicsClass::Frame()
{
	return S_OK;
}

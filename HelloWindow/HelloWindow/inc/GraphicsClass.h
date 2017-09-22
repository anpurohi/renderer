#pragma once
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

// GraphicsClass, as the name suggests, is the class 
// that is responsible for high level rendering of stuff.
// Think of it as the entry point to the rendering engine.

// I hope I don't end up changing the meaning of this class! xD

#include <windows.h>
#include "Dx11Class.h"

const bool FULL_SCREEN = false;
const bool VSYNC = false;
const float SCREEN_DEPTH = 1000.f;
const float SCREEN_NEAR = 0.f;

class GraphicsClass
{
private:
	HRESULT Render();

public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	HRESULT Initialize(int, int, HWND);
	void Shutdown();
	HRESULT Frame();

private:
	Dx11Class *m_dx11;
};
#endif

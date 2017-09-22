#pragma once

#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

// "System" in this case refers to the entire program.
// It can be thought of as the game engine.
// In phase 1 (and possibly another couple of phases) of the project,
// the system is expected to comprise of only an input manager,
// a window manager and a graphics renderer.
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "InputClass.h"
#include "GraphicsClass.h"

class SystemClass
{
public:
	HRESULT Initialize();
	void Run();
	void Shutdown();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

private:
	HRESULT Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCSTR m_appName;
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	InputClass* m_input;
	GraphicsClass* m_graphics;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* ApplicationHandle = 0;
#endif
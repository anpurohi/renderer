#include "..\inc\InputClass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass&)
{
}

InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	// Initialize the state of all 256 keys as unpressed
	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
}

void InputClass::keyUp(unsigned int key)
{
	// Mark current key as unpressed
	m_keys[key] = false;
}

void InputClass::keyDown(unsigned int key)
{
	// Mark current key as pressed
	m_keys[key] = true;
}

bool InputClass::isKeyDown(unsigned int key)
{
	// Return current state of key
	return m_keys[key];
}

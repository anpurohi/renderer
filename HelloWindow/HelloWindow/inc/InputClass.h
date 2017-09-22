#pragma once
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

// InputClass, as the name suggests, is the class responsible
// for managing the inputs received from the user.
class InputClass
{
private:
	bool m_keys[256];

public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();

	void keyUp(unsigned int);
	void keyDown(unsigned int);
	bool isKeyDown(unsigned int);
};

#endif

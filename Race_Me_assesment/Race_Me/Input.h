#pragma once
#include <dinput.h> 
#include"Model.h"
//#include"camera.h"

class Input{
private:

	//adding model source file
	Model*                      model_player;
	Model*                      model_ai;
	Model*                      model_flag;
	Model*                      model_gold[50];
	Model*                      model_tree[40];

	//adding camera source file
//	camera*						camera_player;
//	camera*						camera_ai;

	HINSTANCE	hInst = NULL;
	HWND		hWnd = NULL;

	void Clean();

public:
	Input();
	~Input();

	void ReadInputStates();
	bool IsKeyPressed(unsigned char DI_keycode);
	void Key_Logic();
	HRESULT Initialise_Input(HINSTANCE hInst, HWND hWnd);


	//adding input
	IDirectInput8*			    direct_input;
	IDirectInputDevice8*	    Keyboard_device;
	unsigned char			    keyboard_keys_state[256];



};
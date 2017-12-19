#include "Input.h"

//////////////////////////////////////////////////////////////////////////////////////
// Initialise input 
//////////////////////////////////////////////////////////////////////////////////////
HRESULT Input::Initialise_Input(HINSTANCE hInst,HWND hWnd)
{
	HRESULT hr;
	ZeroMemory(keyboard_keys_state, sizeof(keyboard_keys_state));

	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&direct_input, NULL);
	if (FAILED(hr)) return hr;


	hr = direct_input->CreateDevice(GUID_SysKeyboard, &Keyboard_device, NULL);
	if (FAILED(hr)) return hr;

	hr = Keyboard_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) return hr;

	hr = Keyboard_device->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return hr;

	hr = Keyboard_device->Acquire();
	if (FAILED(hr)) return hr;

	return S_OK;
}

void Input::Clean()
{
	//delete objects
	delete model_player;
	delete model_ai;
	delete model_flag;

	for (int i = 0; i < 50; i++)
	{
		delete model_gold[i];
	}


	for (int i = 0; i < 40; i++)
	{
		delete model_tree[i];
	}


	//delete camera
	//delete camera_player;
	//delete camera_ai;

	//delete keyboard 
	if (Keyboard_device)
	{
		Keyboard_device->Unacquire();
		Keyboard_device->Release();
	}

	//delete input
	if (direct_input)direct_input->Release();

}

Input::Input()
{

}

Input::~Input()
{
	Clean();
}

void  Input::ReadInputStates()
{
	HRESULT hr;
	hr = Keyboard_device->GetDeviceState(sizeof(keyboard_keys_state), (LPVOID)&keyboard_keys_state);

	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
			Keyboard_device->Acquire();
	}
}

bool  Input::IsKeyPressed(unsigned char DI_keycode)
{
	return keyboard_keys_state[DI_keycode] & 0x80;
}


void  Input::Key_Logic()
{
	//close the program
	if (IsKeyPressed(DIK_ESCAPE))
		DestroyWindow(hWnd);

	//change positions X,Z
	if (IsKeyPressed(DIK_D))
	{
		model_player->SetXPos(0.005f);

	}
	if (IsKeyPressed(DIK_A))
	{
		model_player->SetXPos(-0.005f);

	}
	if (IsKeyPressed(DIK_W))
	{
		model_player->SetZPos(0.005f);
		//camera_player->Forward(0.005f);
	}
	if (IsKeyPressed(DIK_S))
	{
		model_player->SetZPos(-0.005f);
		//camera_player->Forward(-0.005f);
	}

	/*if (IsKeyPressed(DIK_E))
	{

		view = camera_ai->GetViewMatrix();
	}

	if (IsKeyPressed(DIK_Q))
	{
		view = camera_player->GetViewMatrix();
	}*/

	//position y
	if (IsKeyPressed(DIK_UP))
		model_player->SetYPos(0.005f);
	if (IsKeyPressed(DIK_DOWN))
		model_player->SetYPos(-0.005f);

	//change rotation
	if (IsKeyPressed(DIK_LEFT))
		model_player->SetXRot(0.005f);
	if (IsKeyPressed(DIK_RIGHT))
		model_player->SetYRot(0.005f);

	//change scaling
	if (IsKeyPressed(DIK_0))
	{
		model_player->SetScale(0.0005f);
		model_ai->SetScale(0.0005f);
	}

	if (IsKeyPressed(DIK_1))
	{
		model_player->SetScale(-0.0005f);
		model_ai->SetScale(-0.0005f);
	}


}

#include"GameManagment.h"
#include "camera.h"
#include "text2D.h"
#include "Input.h"
#include "Render.h"



GameManagment*		gameManagment;
Input*				input;
Render*				render;


//////////////////////////////////////////////////////////////////////////////////////
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	gameManagment = new GameManagment();
	input = new Input();
	render = new Render();

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(gameManagment->InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}


	if (FAILED(gameManagment->InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
		gameManagment->ShutdownD3D();
		return 0;
	}

	//call initialiserGraphics
	if (FAILED(gameManagment->InitialiseGraphics(input)))
	{
		DXTRACE_MSG("Failed to initialise graphics");
		return 0;
	}

	// Main message loop
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			gameManagment->Render();
		}
	}

	gameManagment->ShutdownD3D();
	return (int)msg.wParam;
}











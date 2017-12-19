#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include"Model.h"
#include "camera.h"
#include "text2D.h"
#include "Input.h"

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>


int (WINAPIV * __vsnprintf_s)(char *, size_t, const char*, va_list) = _vsnprintf;


///////////////////////////
//	Global Variables
//////////////////////////////

D3D_DRIVER_TYPE				g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL			g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*				g_pD3DDevice = NULL;

Input*                       input;


HINSTANCE	g_hInst = NULL;
HWND		g_hWnd = NULL;

// Rename for each tutorial
char		g_TutorialName[100] = "";

///////////////////////////////////////////////////////////////////////////////////////////
//	Forward declarations
/////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

CONSTANT_BUFFER0 cb_values;
CONSTANT_BUFFER0 cb_values2;


void ShutdownD3D();



//////////////////////////////////////////////////////////////////////////////////////
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}

	if (FAILED(_Input->Initialise_Input()))
	{
		DXTRACE_MSG("Failed to create Input");
		return 0;
	}

	if (FAILED(InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
		ShutdownD3D();
		return 0;
	}

	//call initialiserGraphics
	if (FAILED(InitialiseGraphics())) 
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
			RenderFrame();
		}
	}

	ShutdownD3D();
	return (int)msg.wParam;
}











#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#define _XM_NO_INTINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

class GameManagment {

public:

	HRESULT InitialiseGraphics(void);
	HRESULT InitialiseD3D();
	HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
	void ShutdownD3D();

	GameManagment();
	~GameManagment();
	

private:
	HINSTANCE g_hInst;
	HWND	  g_hWnd;

	D3D_DRIVER_TYPE				g_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL			g_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*				g_pD3DDevice = NULL;


	//CONSTANT_LightBuffer cb_values;
	//CONSTANT_LightBuffer cb_values2;
	 

	//define vertex structure
	struct POS_COL_TEX_NORM_VERTEX
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Col;
		XMFLOAT2 Texture0;
		XMFLOAT3 Normal;
	};


	struct CONSTANT_LightBuffer
	{
		XMMATRIX WorldViewProjection; 
		XMVECTOR directional_light_vector; 
		XMVECTOR directional_light_colour; 
		XMVECTOR ambient_light_colour; 
									   
	};


};
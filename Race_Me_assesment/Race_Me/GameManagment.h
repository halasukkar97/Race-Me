#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "Model.h"
#include"Render.h"
//#include "camera.h"
#include "text2D.h"
#include "Input.h"
#define _XM_NO_INTINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class GameManagment {

public:

	HRESULT InitialiseGraphics(Input* input);
	HRESULT InitialiseD3D();
	HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
	void ShutdownD3D();

	GameManagment();
	~GameManagment();
	
	Render* render;
	void Render();

private:
	char		GameName[100] = "RaceMe";



	HINSTANCE   hInst;
	HWND	    hWnd;

	D3D_DRIVER_TYPE				driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL			featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*				pD3DDevice = NULL;

	ID3D11DeviceContext*		pImmediateContext = NULL;
	IDXGISwapChain*				pSwapChain = NULL;

	//adding constant buffer and z buffer
	ID3D11Buffer*				pConstantBuffer0;
	ID3D11DepthStencilView*     pZBuffer;

	//adding vertex buffer and shader, pixel shader and input layout
	ID3D11Buffer*				pVertexBuffer;
	ID3D11VertexShader*			pVertexShader;
	ID3D11PixelShader*			pPixelShader;
	ID3D11InputLayout*			pInputLayout;
	ID3D11RenderTargetView*		pBackBufferRTView = NULL;
	


	//adding textures and sampler
	ID3D11ShaderResourceView*   pTexture_player;
	ID3D11ShaderResourceView*   pTexture_ai;
	ID3D11ShaderResourceView*   pTexture_gold;
	ID3D11ShaderResourceView*   pTexture_tree;
	ID3D11ShaderResourceView*   pTexture_flag;
	ID3D11SamplerState*         pSampler;

	//adding lights
	XMVECTOR directionla_light_shines_from;
	XMVECTOR directional_light_colour;
	XMVECTOR ambient_light_colour;

	//adding objects
	Model*                      model_player;
	Model*                      model_ai;
	Model*                      model_flag;
	Model*                      model_gold[50];
	Model*                      model_tree[40];

	//adding camera source file
	//camera*						camera_player;
	//camera*						camera_ai;

	//adding input
	Input*                       input;

	//adding text
	Text2D*  timer;
	Text2D*  moneyCount;

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
#pragma once
#include"Model.h"
#include "camera.h"
#include "text2D.h"
#include "Input.h"

class Render {

public:

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
	ID3D11DeviceContext*        pImmediateContext;


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


	void RenderFrame();
	int  money = 0;

	XMMATRIX projection, world, view;

	//adding text
	Text2D*  timer;
	Text2D*  moneyCount;

	void RenderFrame();
	~Render();


private:

	//addding input
	Input*  input;
	//adding model source file
	Model*                      model_player;
	Model*                      model_ai;
	Model*                      model_flag;
	Model*                      model_gold[50];
	Model*                      model_tree[40];

	//adding camera source file
	camera*						camera_player;
	camera*						camera_ai;

	HINSTANCE					hInst = NULL;
	HWND						hWnd = NULL;
	

	void ClearBackBuffer();
	void SetLighting();
	void SetVertexBuffer();
	void SetShaderObjects();
	void SetTextures();
	void SetMovment();
	void checkCollisions();
	void ShowText();
	void Draw();
	void Clean();

};
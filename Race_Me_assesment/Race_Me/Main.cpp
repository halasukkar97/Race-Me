#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include"Model.h"
#include "camera.h"
#include "text2D.h"
#include <dinput.h>  //for input
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
ID3D11DeviceContext*		g_pImmediateContext = NULL;
IDXGISwapChain*				g_pSwapChain = NULL;
ID3D11RenderTargetView*		g_pBackBufferRTView = NULL;

//adding vertex buffer and shader, pixel shader and input layout
ID3D11Buffer*				g_pVertexBuffer;
ID3D11VertexShader*			g_pVertexShader;
ID3D11PixelShader*			g_pPixelShader;
ID3D11InputLayout*			g_pInputLayout;

//adding constant buffer and z buffer
ID3D11Buffer*				g_pConstantBuffer0;
ID3D11DepthStencilView*     g_pZBuffer;

//adding textures and sampler
ID3D11ShaderResourceView*   g_pTexture_player; 
ID3D11ShaderResourceView*   g_pTexture_ai;
ID3D11ShaderResourceView*   g_pTexture_gold;
ID3D11ShaderResourceView*   g_pTexture_tree;
ID3D11ShaderResourceView*   g_pTexture_flag;
ID3D11SamplerState*         g_pSampler;

//adding model source file
Model*                      g_model_player;
Model*                      g_model_ai;
Model*                      g_model_flag;
Model*                      g_model_gold[50];
Model*                      g_model_tree[40];

//adding camera source file
camera*						camera_player;
camera*						camera_ai;

//adding text
Text2D* g_timer;
Text2D* g_moneyCount;

//adding lights
XMVECTOR g_directionla_light_shines_from;
XMVECTOR g_directional_light_colour;
XMVECTOR g_ambient_light_colour;


//adding input
IDirectInput8*			    g_direct_input;
IDirectInputDevice8*	    g_Keyboard_device;
unsigned char			    g_keyboard_keys_state[256];


//define vertex structure
struct POS_COL_TEX_NORM_VERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT4 Col;
	XMFLOAT2 Texture0;
	XMFLOAT3 Normal;
};

//creating the constant for the lights
struct CONSTANT_BUFFER0
{
	XMMATRIX WorldViewProjection; //64 bytes ( 4x4 = 16 floats x 4 bytes)
	XMVECTOR directional_light_vector; //16 bytes
	XMVECTOR directional_light_colour; //16 bytes
	XMVECTOR ambient_light_colour; //16 bytes
								   // TOTAL SIZE = 112 BYTES
};

HINSTANCE	g_hInst = NULL;
HWND		g_hWnd = NULL;

// Rename for each tutorial
char		g_TutorialName[100] = "";

///////////////////////////////////////////////////////////////////////////////////////////
//	Forward declarations
/////////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseGraphics(void);

HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

CONSTANT_BUFFER0 cb_values;
CONSTANT_BUFFER0 cb_values2;

HRESULT Initialise_Input();
void ReadInputStates();
bool IsKeyPressed(unsigned char DI_keycode);
void Key_Logic();

HRESULT InitialiseD3D();
void RenderFrame(void);
void ShutdownD3D();

float deltaTime = 0.0;
int  money = 0;
int ai_mony = 0;
int watch = 0;

XMMATRIX projection, world, view;

///////////////////////////////////////////////////////////////////////////////////
// Create D3D device and swap chain
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseD3D()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, // comment out this line if you need to test D3D 11.0 functionality on hardware that doesn't support it
		D3D_DRIVER_TYPE_WARP, // comment this out also to use reference device
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain,
			&g_pD3DDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;


	// Get pointer to back buffer texture
	ID3D11Texture2D *pBackBufferTexture;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),(LPVOID*)&pBackBufferTexture);
	


	if (FAILED(hr)) return hr;

	// Use the back buffer texture pointer to create the render target view
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL,
		&g_pBackBufferRTView);
	pBackBufferTexture->Release();

	if (FAILED(hr)) return hr;

	//create a z buffer texture
	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

	tex2dDesc.Width = width;
	tex2dDesc.Height = height;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;


	ID3D11Texture2D *pZBufferTexture;
	hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

	if (FAILED(hr)) return hr;

	//create the Z buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;


	g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
	pZBufferTexture->Release();



	// Set the render target view
	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

	// Set the viewport
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	g_pImmediateContext->RSSetViewports(1, &viewport);


	g_timer = new Text2D("assets/font1.bmp", g_pD3DDevice, g_pImmediateContext);
	g_moneyCount = new Text2D("assets/font1.bmp", g_pD3DDevice, g_pImmediateContext);


	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
// Clean up D3D objects
//////////////////////////////////////////////////////////////////////////////////////
void ShutdownD3D()
{
	//deleting vertex buffer , input layout,vertex shader , pixel shader
	if (g_pVertexBuffer) g_pVertexBuffer->Release(); 
	if (g_pInputLayout)g_pInputLayout->Release(); 
	if (g_pVertexShader)g_pVertexShader->Release(); 
	if (g_pPixelShader)g_pPixelShader->Release(); 

	//delete back bufferTRview
	if (g_pBackBufferRTView) g_pBackBufferRTView->Release();

	//delete swapchain , constan buffer, immediat context
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pConstantBuffer0) g_pConstantBuffer0->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();

	//delete objects
	delete g_model_player;
	delete g_model_ai;
	delete g_model_flag;

	for (int i = 0; i < 50; i++)
	{
		delete g_model_gold[i];
	}


	for (int i = 0; i < 40; i++)
	{
		delete g_model_tree[i];
	}


	//delete camera
	delete camera_player;
	delete camera_ai;

	//delete text
	delete g_timer;
	delete g_moneyCount;

	//delete keyboard 
	if (g_Keyboard_device)
	{
		g_Keyboard_device->Unacquire();
		g_Keyboard_device->Release();
	}

	

	//delete input
	if (g_direct_input)g_direct_input->Release();

	//delete model objects texture
	if (g_pTexture_player)  g_pTexture_player->Release();
	if (g_pTexture_ai)  g_pTexture_ai->Release();
	if (g_pTexture_flag)  g_pTexture_flag->Release();
	if (g_pTexture_gold)  g_pTexture_gold->Release();
	if (g_pTexture_tree)  g_pTexture_tree->Release();

	
	//delete sampler
	if (g_pSampler)  g_pSampler->Release();

	//delete this project
	if (g_pD3DDevice) g_pD3DDevice->Release();
}

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

	if (FAILED(Initialise_Input()))
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

//////////////////////////////////////////////////////////////////////////////////////
// Initialise input 
//////////////////////////////////////////////////////////////////////////////////////
HRESULT Initialise_Input()
{
	HRESULT hr;
	ZeroMemory(g_keyboard_keys_state, sizeof(g_keyboard_keys_state));

	hr = DirectInput8Create(g_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_direct_input, NULL);
	if (FAILED(hr)) return hr;

	hr = g_direct_input->CreateDevice(GUID_SysKeyboard, &g_Keyboard_device, NULL);
	if (FAILED(hr)) return hr;

	hr = g_Keyboard_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) return hr;

	hr = g_Keyboard_device->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return hr;

	hr = g_Keyboard_device->Acquire();
	if (FAILED(hr)) return hr;

	return S_OK;
}

void ReadInputStates()   //void means dont return anything
{
	HRESULT hr;
	hr = g_Keyboard_device->GetDeviceState(sizeof(g_keyboard_keys_state), (LPVOID)&g_keyboard_keys_state);

	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
			g_Keyboard_device->Acquire();
	}
}

bool IsKeyPressed(unsigned char DI_keycode)
{
	return g_keyboard_keys_state[DI_keycode] & 0x80;
}


void Key_Logic()
{
	//close the program
	if (IsKeyPressed(DIK_ESCAPE))
		DestroyWindow(g_hWnd);

	//change positions X,Z
	if (IsKeyPressed(DIK_D))
	{
		g_model_player->SetXPos(0.005f);
		
	}
	if (IsKeyPressed(DIK_A))
	{
		g_model_player->SetXPos(-0.005f);
		
	}
	if (IsKeyPressed(DIK_W))
	{
		g_model_player->SetZPos(0.005f);
		camera_player->Forward(0.005f);
	}
	if (IsKeyPressed(DIK_S))
	{
		g_model_player->SetZPos(-0.005f);
		camera_player->Forward(-0.005f);
	}
	

	//switch between ai and player cameras
	if (IsKeyPressed(DIK_E))
	{
		OutputDebugString("pressed e");
		view = camera_ai->GetViewMatrix();
	}
	
	if (IsKeyPressed(DIK_Q))
	{
		OutputDebugString("pressed q");
		view = camera_player->GetViewMatrix();
	}

	//position y
	if (IsKeyPressed(DIK_UP))
		g_model_player->SetYPos(0.005f);
	if (IsKeyPressed(DIK_DOWN))
		g_model_player->SetYPos(-0.005f);

	//change rotation
	if (IsKeyPressed(DIK_LEFT))
		g_model_player->SetXRot(0.005f);
	if (IsKeyPressed(DIK_RIGHT))
		g_model_player->SetYRot(0.005f);

	//change scaling
	if (IsKeyPressed(DIK_0))
	{
		g_model_player->SetScale(0.0005f);
		g_model_ai->SetScale(0.0005f);
	}

	if (IsKeyPressed(DIK_1))
	{
		g_model_player->SetScale(-0.0005f);
		g_model_ai->SetScale(-0.0005f);
	}


}


//////////////////////////////////////////////////////////////////////////////////////
// Register class and create window
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Give your app window your own name
	char Name[100] = "Tutorial 10 Exercise 01\0";

	// Register class
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//   wcex.hbrBackground = (HBRUSH )( COLOR_WINDOW + 1); // Needed for non-D3D apps
	wcex.lpszClassName = Name;

	if (!RegisterClassEx(&wcex)) return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 1500, 800 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(Name, g_TutorialName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
		rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
// Called every time the application receives a message
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(g_hWnd);
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//init graphics
////////////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT InitialiseGraphics()
{
	HRESULT hr = S_OK;

	//load model player
	g_model_player = new Model(g_pD3DDevice, g_pImmediateContext,0,0,0);
	g_model_player->LoadObjModel("assets/cube.obj");

	//load model sphere
	g_model_ai= new Model(g_pD3DDevice, g_pImmediateContext,5,0,0);
	g_model_ai->LoadObjModel("assets/cube.obj");

	//load flag
	g_model_flag = new Model(g_pD3DDevice, g_pImmediateContext, rand() % 100, 0, rand() % 100);
	g_model_flag->LoadObjModel("assets/cube.obj");


	//load model gold
	for (int i = 0; i < 50; i++)
	{
		g_model_gold[i] = new Model(g_pD3DDevice, g_pImmediateContext, rand() % 100, 0, rand() % 100);
		g_model_gold[i]->LoadObjModel("assets/cube.obj");
	}

	for (int i = 0; i < 40; i++)
	{
		g_model_tree[i] = new Model(g_pD3DDevice, g_pImmediateContext, rand() % 100, 0, rand() % 100);
		g_model_tree[i]->LoadObjModel("assets/cube.obj");
	}

	////create constant buffer //04-1
	//D3D11_BUFFER_DESC constant_buffer_desc;
	//ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	//constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;  //can use updateSubresource() to update
	//constant_buffer_desc.ByteWidth = 112;  //must be a multiple of 16, calculate from CB struct
	//constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;  //use as a constant buffer

	//hr = g_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &g_pConstantBuffer0);

	if (FAILED(hr))
		return hr;
	// adding the sampler 
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	g_pD3DDevice->CreateSamplerState(&sampler_desc, &g_pSampler);

	// adding the camera and values 
	camera_player = new camera(0, 5, -15, 0);
	camera_ai = new camera(5, 5, -18, 0);

	//adding the texture from the assets file
	D3DX11CreateShaderResourceViewFromFile(g_pD3DDevice, "assets/playerCar.jpg", NULL, NULL, &g_pTexture_player, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pD3DDevice, "assets/aiCar.png", NULL, NULL, &g_pTexture_ai, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pD3DDevice, "assets/gold.jpg", NULL, NULL, &g_pTexture_gold, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pD3DDevice, "assets/tree.jpg", NULL, NULL, &g_pTexture_tree, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pD3DDevice, "assets/endpoint.png", NULL, NULL, &g_pTexture_flag, NULL);

	//set the samplers
	g_model_player->set_sampler(g_pSampler);
	g_model_ai->set_sampler(g_pSampler);
	g_model_flag->set_sampler(g_pSampler);
	
	for (int i = 0; i < 50; i++)
	{
		g_model_gold[i]->set_sampler(g_pSampler);
	}

	for (int i = 0; i < 40; i++)
	{
		g_model_tree[i]->set_sampler(g_pSampler);
	}

	// setting the textures
	g_model_player->set_texture(g_pTexture_player);
	g_model_ai->set_texture(g_pTexture_ai);
	g_model_flag->set_texture(g_pTexture_flag);
	
	for (int i = 0; i < 50; i++)
	{
		g_model_gold[i]->set_texture(g_pTexture_gold);
	}

	for (int i = 0; i < 40; i++)
	{
		g_model_tree[i]->set_texture(g_pTexture_tree);
	}


	//copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;



	return S_OK;
}
void RenderFrame(void)
{
	// Clear the back buffer - choose a colour you like
	float rgba_clear_colour[4] = { 0.0f, 0.0f, 0.6f, 1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, rgba_clear_colour);
	g_pImmediateContext->ClearDepthStencilView(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);


	//read input
	ReadInputStates();
	Key_Logic();
	UINT stride = sizeof(g_model_player);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer0);


	// RENDER HERE
	//XMMATRIX projection, world, view;
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), 640.0 / 480.0, 1.0f, 100.0f);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	

	g_directionla_light_shines_from = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	g_directional_light_colour = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	g_ambient_light_colour = XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f);



   // setting the textures
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSampler);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexture_player);
	//set the shader objects as active
	g_pImmediateContext->VSSetShader(g_pVertexShader, 0, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, 0, 0);
	g_pImmediateContext->IASetInputLayout(g_pInputLayout);
	
	//use the camera to view
	view = camera_player->GetViewMatrix();

	//make the camera look at the player
	camera_player->LookAt_XZ(g_model_player->GetXPos(), g_model_player->GetZPos());

	//make the ai car look at the flag and go towerd it
	g_model_ai->LookAt_XZ(g_model_flag->GetXPos(), g_model_flag->GetZPos());
	g_model_ai->MoveForward(0.003f);
	
	//camera ai look at ai car
	camera_ai->LookAt_XZ(g_model_ai->GetXPos(), g_model_ai->GetZPos());

	//keep the time going
	watch += 1;

		
	//if the flag touch the ai car stop the ai car from moving
	if (g_model_flag->CheckCollision(g_model_ai) || g_model_ai->CheckCollision(g_model_flag))
	{
		//stop the ai and the camera movement
		g_model_ai->MoveForward(0.0f);
		camera_ai->Forward(0.000f);

	}
	else
	{
		//keep the camera moving
		camera_ai->Forward(0.003f);
	}

		

	//if the flag touches the player stop the player from moving an the camera
	if (g_model_flag->CheckCollision(g_model_player) || g_model_player->CheckCollision(g_model_flag))
	{
		g_model_player->MoveForward(0.0f);
		camera_player->Forward(0.000f);
	}
		
	// if the player touch the ai car 
	if (g_model_player->CheckCollision(g_model_ai) || g_model_ai->CheckCollision(g_model_player))
	{
			//move the ai car and the camera a little bit to the right
		g_model_ai->SetXPos(0.5f);
		camera_ai->SetXPos(0.5f);
	}

	
		


	//collect money for player
	for (int i = 0; i < 50; i++)
	{
		if (g_model_gold[i]->CheckCollision(g_model_player))
		{
			money += 750;
			g_model_gold[i]->SetDraw(false);
		}
	}

	//collect money for AI
	for (int i = 0; i < 50; i++)
	{
		if (g_model_gold[i]->CheckCollision(g_model_ai))
		{
			ai_mony += 550;
			g_model_gold[i]->SetDraw(false);
		}
	}


	//check tree collition
	for (int i = 0; i < 40; i++)
	{
		if (g_model_tree[i]->CheckCollision(g_model_player))
		{
			g_model_player->MoveForward(-0.5f);
		}
	}

	for (int i = 0; i < 40; i++)
	{
		if (g_model_tree[i]->CheckCollision(g_model_ai))
		{
			g_model_ai->SetXPos(0.5f);
			camera_ai->SetXPos(0.5f);
		}
	}


	//draw the player and ai and flag
	g_model_player->Draw(&view, &projection);
	g_model_ai->Draw(&view, &projection);
	g_model_flag->Draw(&view, &projection);

	//draw 50 gold and 40 trees
	for (int i = 0; i < 50; i++){if (g_model_gold[i]->GetDraw() == true){g_model_gold[i]->Draw(&view, &projection);}}
	for (int i = 0; i < 40; i++){	g_model_tree[i]->Draw(&view, &projection);}

	//print on the screen the time and money amount and stop the game 
	if (g_model_flag->CheckCollision(g_model_ai) || g_model_ai->CheckCollision(g_model_flag))
	{
		Sleep(1000);
		g_timer->AddText("you lost", -1.0, 1, .1);
		g_timer->RenderText();
		
		
	}
	else if (g_model_flag->CheckCollision(g_model_player) || g_model_player->CheckCollision(g_model_flag))
	{
		Sleep(1000);
		g_timer->AddText("you win", -1.0, 1, .1);
		g_timer->RenderText();

		
	}
	else
	{
		g_timer->AddText("time is : " + std::to_string(watch / 1000.0), -1.0, 1, .1);
		g_timer->RenderText();
	}
	

	g_moneyCount->AddText("money : "+ std::to_string(money), -1.0, -0.9, .1);
	g_moneyCount->RenderText();


	// Display what has just been rendered
	g_pSwapChain->Present(0, 0);


}










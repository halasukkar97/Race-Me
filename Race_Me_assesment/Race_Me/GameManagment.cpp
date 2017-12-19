#include "GameManagment.h"

int (WINAPIV * __vsnprintf_s)(char *, size_t, const char*, va_list) = _vsnprintf;

///////////////////////////////////////////////////////////////////////////////////
// Create D3D device and swap chain
//////////////////////////////////////////////////////////////////////////////////////
HRESULT GameManagment::InitialiseD3D()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hWnd, &rc);
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
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &pSwapChain,
			&pD3DDevice, &featureLevel, &pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;


	// Get pointer to back buffer texture
	ID3D11Texture2D *pBackBufferTexture;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture);



	if (FAILED(hr)) return hr;

	// Use the back buffer texture pointer to create the render target view
	hr = pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL,
		&pBackBufferRTView);
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
	hr = pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

	if (FAILED(hr)) return hr;

	//create the Z buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;


	pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &pZBuffer);
	pZBufferTexture->Release();



	// Set the render target view
pImmediateContext->OMSetRenderTargets(1, &pBackBufferRTView, pZBuffer);

	// Set the viewport
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	pImmediateContext->RSSetViewports(1, &viewport);


	timer = new Text2D("assets/font1.bmp", g_pD3DDevice, pImmediateContext);
	moneyCount = new Text2D("assets/font1.bmp", g_pD3DDevice, g_pImmediateContext);


	return S_OK;
}




//////////////////////////////////////////////////////////////////////////////////////
// Clean up D3D objects
//////////////////////////////////////////////////////////////////////////////////////
void GameManagment::ShutdownD3D()
{
	//deleting vertex buffer , input layout,vertex shader , pixel shader
	if (pVertexBuffer)pVertexBuffer->Release();
	if (pInputLayout)pInputLayout->Release();
	if (pVertexShader)pVertexShader->Release();
	if (pPixelShader)pPixelShader->Release();

	//delete back bufferTRview
	if (pBackBufferRTView) pBackBufferRTView->Release();

	//delete swapchain , constan buffer, immediat context
	if (pSwapChain) pSwapChain->Release();
	if (pConstantBuffer0) pConstantBuffer0->Release();
	if (pImmediateContext) pImmediateContext->Release();

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
	delete camera_player;
	delete camera_ai;

	//delete text
	delete timer;
	delete moneyCount;

	//delete keyboard 
	if (_Input->g_Keyboard_device)
	{
		_Input->g_Keyboard_device->Unacquire();
		_Input->g_Keyboard_device->Release();
	}



	//delete input
	if (_Input->g_direct_input)_Input->g_direct_input->Release();

	//delete model objects texture
	if (pTexture_player)pTexture_player->Release();
	if (pTexture_ai) pTexture_ai->Release();
	if (pTexture_flag) pTexture_flag->Release();
	if (pTexture_gold) pTexture_gold->Release();
	if (pTexture_tree) pTexture_tree->Release();


	//delete sampler
	if (pSampler)  pSampler->Release();

	//delete this project
	if (pD3DDevice) pD3DDevice->Release();
}

GameManagment::GameManagment()
{
}

GameManagment::~GameManagment()
{
}



//////////////////////////////////////////////////////////////////////////////////////
// Register class and create window
//////////////////////////////////////////////////////////////////////////////////////
HRESULT GameManagment::InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
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
	hInst = hInstance;
	RECT rc = { 0, 0, 1500, 800 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindow(Name, GameName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
		rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!hWnd)
		return E_FAIL;

	ShowWindow(hWnd, nCmdShow);

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
			DestroyWindow(hWnd);
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//init graphics
////////////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT GameManagment::InitialiseGraphics()
{
	HRESULT hr = S_OK;

	//load model player
	model_player = new Model(pD3DDevice, pImmediateContext, 0, 0, 0);
	model_player->LoadObjModel("assets/cube.obj");

	//load model sphere
	model_ai = new Model(pD3DDevice, pImmediateContext, 5, 0, 0);
	model_ai->LoadObjModel("assets/cube.obj");

	//load flag
	model_flag = new Model(pD3DDevice, pImmediateContext, rand() % 100, 0, rand() % 100);
	model_flag->LoadObjModel("assets/cube.obj");


	//load model gold
	for (int i = 0; i < 50; i++)
	{
		model_gold[i] = new Model(pD3DDevice, pImmediateContext, rand() % 100, 0, rand() % 100);
		model_gold[i]->LoadObjModel("assets/cube.obj");
	}

	for (int i = 0; i < 40; i++)
	{
		model_tree[i] = new Model(pD3DDevice, pImmediateContext, rand() % 100, 0, rand() % 100);
		model_tree[i]->LoadObjModel("assets/cube.obj");
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

	pD3DDevice->CreateSamplerState(&sampler_desc, &pSampler);

	// adding the camera and values 
	camera_player = new camera(0, 5, -15, 0);
	camera_ai = new camera(5, 5, -18, 0);

	//adding the texture from the assets file
	D3DX11CreateShaderResourceViewFromFile(pD3DDevice, "assets/playerCar.jpg", NULL, NULL, &pTexture_player, NULL);
	D3DX11CreateShaderResourceViewFromFile(pD3DDevice, "assets/aiCar.png", NULL, NULL, &pTexture_ai, NULL);
	D3DX11CreateShaderResourceViewFromFile(pD3DDevice, "assets/gold.jpg", NULL, NULL, &pTexture_gold, NULL);
	D3DX11CreateShaderResourceViewFromFile(pD3DDevice, "assets/tree.jpg", NULL, NULL, &pTexture_tree, NULL);
	D3DX11CreateShaderResourceViewFromFile(pD3DDevice, "assets/endpoint.png", NULL, NULL, &pTexture_flag, NULL);

	//set the samplers
	model_player->set_sampler(pSampler);
	model_ai->set_sampler(pSampler);
	model_flag->set_sampler(pSampler);

	for (int i = 0; i < 50; i++)
	{
		model_gold[i]->set_sampler(pSampler);
	}

	for (int i = 0; i < 40; i++)
	{
		model_tree[i]->set_sampler(pSampler);
	}

	// setting the textures
	model_player->set_texture(pTexture_player);
	model_ai->set_texture(pTexture_ai);
	model_flag->set_texture(pTexture_flag);

	for (int i = 0; i < 50; i++)
	{
		model_gold[i]->set_texture(pTexture_gold);
	}

	for (int i = 0; i < 40; i++)
	{
		model_tree[i]->set_texture(pTexture_tree);
	}


	//copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;



	return S_OK;
}
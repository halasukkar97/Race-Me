#include "GameManagment.h"


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
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture);



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

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
	if (_Input->g_Keyboard_device)
	{
		_Input->g_Keyboard_device->Unacquire();
		_Input->g_Keyboard_device->Release();
	}



	//delete input
	if (_Input->g_direct_input)_Input->g_direct_input->Release();

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
	g_model_player = new Model(g_pD3DDevice, g_pImmediateContext, 0, 0, 0);
	g_model_player->LoadObjModel("assets/cube.obj");

	//load model sphere
	g_model_ai = new Model(g_pD3DDevice, g_pImmediateContext, 5, 0, 0);
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
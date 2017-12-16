#include"Model.h"


Model::Model(ID3D11Device * Device, ID3D11DeviceContext * Context,float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_xangle = 0.0f;
	m_zangle = 0.0f;
	m_yangle = 0.0f;
	m_scale = 1.0f;


	m_pD3DDevice = Device;
	m_pImmediateContext = Context;


}

int Model::LoadObjModel(char * filename)
{
	HRESULT hr = S_OK;

	m_pObject = new ObjFileModel(filename, m_pD3DDevice, m_pImmediateContext);
	if (m_pObject->filename == "FILE NOT LOADED") return S_FALSE;

	//create constant buffer 
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;  //can use updateSubresource() to update
	constant_buffer_desc.ByteWidth = 64;  //must be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;  //use as a constant buffer

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	if (FAILED(hr))
		return hr;


	//load and compile pixel and vertex shaders -use vs_5_0 to target DX11 hardware only
	ID3DBlob *VS, *PS, *error;

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);
	if (error != 0)//check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)) //dont fail if error is just a warning
		{
			return hr;
		};
	}

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);
	if (error != 0)//check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)) //dont fail if error is just a warning
		{
			return hr;
		};
	}



	//create shader objects
	hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVShader);

	if (FAILED(hr))
	{
		return hr;
	}


	hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPShader);

	if (FAILED(hr))
	{
		return hr;
	}



	//create and set the input layout object
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL", 0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);

	if (FAILED(hr))
	{
		return hr;
	}

	return 0;
}
void Model::set_texture(ID3D11ShaderResourceView* texture)
{
	m_pTexture = texture;

}

void Model::set_sampler(ID3D11SamplerState* sampler)
{
	m_pSampler0 = sampler;


}
void Model::Draw(XMMATRIX* view, XMMATRIX* projection)
{
	XMMATRIX world;
	world= XMMatrixRotationX(m_xangle);
	world *= XMMatrixRotationY(m_yangle);
	world *= XMMatrixRotationZ(m_zangle);
	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixTranslation(m_x, m_y, m_z);


	MODEL_CONSTANT_BUFFER model_cb_values;
	model_cb_values.WorldViewProjection = world*(*view)*(*projection);

	//upload the new values for the constant buffer
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	//set the shader objects as active
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
	m_pImmediateContext->PSSetShaderResources(0, 1,&m_pTexture);

	m_pObject->Draw();
}

Model::~Model()
{
	if(m_pObject)delete m_pObject;
	if(m_pInputLayout)m_pInputLayout->Release();
	if(m_pVShader)m_pVShader->Release();
	if(m_pPShader)m_pPShader->Release();

	m_pConstantBuffer->Release();
}

void Model::SetXPos(float x) { m_x = x; }
void Model::SetYPos(float y) { m_y = y; }
void Model::SetZPos(float z) { m_z = z; }

void Model::SetXRot(float xRotation) { m_xangle = xRotation; }
void Model::SetYRot(float yRotation) { m_yangle = yRotation; }
void Model::SetZRot(float zRotation) { m_zangle = zRotation; }

void Model::SetScale(float scale) { m_scale = scale; }

float Model::GetXPos() { return m_x; }
float Model::GetYPos() { return m_y; }
float Model::GetZPos() { return m_z; }

float Model::GetXRot() { return m_xangle; }
float Model::GetYRot() { return m_yangle; }
float Model::GetZRot() { return m_zangle; }

float Model::GetScale() { return m_scale; }
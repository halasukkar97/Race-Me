#include"Model.h"


Model::Model(ID3D11Device * Device, ID3D11DeviceContext * Context,float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_xangle = 80.0f;
	m_zangle = 0.0f;
	m_yangle = 0.0f;
	m_scale = 1.0f;


	m_pD3DDevice = Device;
	m_pImmediateContext = Context;
	
	m_Draw = true;

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

	//calling the calculations for the collision
	CalculateModelCentrePoint();
	CalculateBoundingSphereRadius();

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
void Model::LookAt_XZ(float x, float z)
{
	//calculate dx and dz between the object and the look at position passed in.
	float DX = x - m_x;
	float DZ = z - m_z;

	//update m_yangle using the arctangent calculation and converting to degrees.
	m_yangle = atan2(DX, DZ) * (180.0 / XM_PI);

}
void Model::MoveForward(float distance)
{

	//update the model position by using this distance to update the model m_x and m_z positions based on m_yangle
	m_x += sin(m_yangle * (XM_PI / 180.0)) * distance;
	m_z += cos(m_yangle * (XM_PI / 180.0)) * distance;

}
bool Model::CheckCollision(Model * Model)
{

	//Return false if the Model passed in is the same as the current Model - obviously a Model cannot collide with itself
	if (Model == this)
		return false;

	//Use GetBoundingSphereWorldSpacePosition()to get the bounding sphere positions of the current model and the model passed in as a parameter
	XMVECTOR current_Model_position = GetBoundingSphereWorldSpacePosition();
	XMVECTOR other_Model_position = Model->GetBoundingSphereWorldSpacePosition();

	//Use the Pythagorean theorem to calculate the distance between the two positions
	float x1 = XMVectorGetX(current_Model_position);
	float y1 = XMVectorGetY(current_Model_position);
	float z1 = XMVectorGetZ(current_Model_position);

	float x2 = XMVectorGetX(other_Model_position);
	float y2 = XMVectorGetY(other_Model_position);
	float z2 = XMVectorGetZ(other_Model_position);

	float DistanceX = 0;
	float DistanceY = 0;
	float DistanceZ = 0;

	// sphere collision on X plane
	DistanceX = x1 - x2;

	// sphere collision on Y plane
	DistanceY = y1 - y2;

	// sphere collision on Z plane
	DistanceZ = z1 - z2;

	//calculating the distance
	float Distance = pow(DistanceX, 2) + pow(DistanceY, 2) + pow(DistanceZ, 2);

	//adding tow radii together
	float sum2Radii = m_radius + Model->GetBoundingSphereRadius();

	//if the totall > distance then return true
	if (sum2Radii >= Distance)return true;
	else return false;


	return false;
}
XMVECTOR Model::GetBoundingSphereWorldSpacePosition()
{
	//Create an XMMATRIX world matrix
	XMMATRIX world;
	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationX(m_xangle);
	world *= XMMatrixRotationY(m_yangle);
	world *= XMMatrixRotationZ(m_zangle);
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	//Create an XMVECTOR variable offset to store the model space centre offset
	XMVECTOR offset;


	//Set the offset position to that of the centre of the bounding sphere in model space
	offset = XMVectorSet(m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z, 0.0);

	//Transform the vector by the world matrix 
	offset = XMVector3Transform(offset, world);

	return offset;
}

void Model::CalculateModelCentrePoint()
{

	for (int i = 0; i < m_pObject->numverts; i++)
	{
		// if x > MaxX then set MaxX
		if (m_pObject->vertices[i].Pos.x > MaxX)
			MaxX = m_pObject->vertices[i].Pos.x;

		// if x < MinX then set MinX
		if (m_pObject->vertices[i].Pos.x < MinX)
			MinX = m_pObject->vertices[i].Pos.x;

		//if y > MaxY then set MaxY
		if (m_pObject->vertices[i].Pos.x > MaxY)
			MaxY = m_pObject->vertices[i].Pos.x;

		// if y < MinY then set MinY
		if (m_pObject->vertices[i].Pos.x < MinY)
			MinY = m_pObject->vertices[i].Pos.x;

		//if z > MaxZ then set MaxZ
		if (m_pObject->vertices[i].Pos.x > MaxZ)
			MaxZ = m_pObject->vertices[i].Pos.x;

		// if z < MinZ then set MinZ
		if (m_pObject->vertices[i].Pos.x < MinZ)
			MinZ = m_pObject->vertices[i].Pos.x;
	}


	//calculate sphere centre points from min and max values
	m_bounding_sphere_centre_x = MinX + ((MaxX - MinX) / 2);
	m_bounding_sphere_centre_y = MinY + ((MaxY - MinY) / 2);
	m_bounding_sphere_centre_z = MinZ + ((MaxZ - MinZ) / 2);
	m_bounding_sphere_center = { m_bounding_sphere_centre_x ,m_bounding_sphere_centre_y ,m_bounding_sphere_centre_z ,0.0f };
}
void Model::CalculateBoundingSphereRadius()
{
	for (int i = 0; i < m_pObject->numverts; i++)

	{
		distanceX = m_pObject->vertices[i].Pos.x - m_bounding_sphere_center.x;
		distanceY = m_pObject->vertices[i].Pos.y - m_bounding_sphere_center.y;
		distanceZ = m_pObject->vertices[i].Pos.z - m_bounding_sphere_center.z;
	}

	total_distance = pow(distanceX, 2) + pow(distanceY, 2) + pow(distanceZ, 2);
	// set bounding sphere radius with calculated maximum distance
	if (total_distance >= m_radius)
	{
		m_radius = total_distance;
	}
}
void Model::Draw(XMMATRIX* view, XMMATRIX* projection)
{
	XMMATRIX world;
	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationX(m_xangle);
	world *= XMMatrixRotationY(m_yangle);
	world *= XMMatrixRotationZ(m_zangle);

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

void Model::SetXPos(float x) { m_x += x; }
void Model::SetYPos(float y) { m_y += y; }
void Model::SetZPos(float z) { m_z += z; }

void Model::SetXRot(float xRotation) { m_xangle += xRotation; OutputDebugString("SetRotation\n"); }
void Model::SetYRot(float yRotation) { m_yangle += yRotation; }
void Model::SetZRot(float zRotation) { m_zangle = zRotation; }

void Model::SetScale(float scale) { m_scale += scale; }

float Model::GetXPos() { return m_x; }
float Model::GetYPos() { return m_y; }
float Model::GetZPos() { return m_z; }

float Model::GetXRot() { return m_xangle; }
float Model::GetYRot() { return m_yangle; }
float Model::GetZRot() { return m_zangle; }

float Model::GetScale() { return m_scale; }

float Model::GetBoundingSphereCentreX() { return m_bounding_sphere_centre_x; }
float Model::GetBoundingSphereCentreY() { return m_bounding_sphere_centre_y; }
float Model::GetBoundingSphereCentreZ() { return m_bounding_sphere_centre_z; }
float Model::GetBoundingSphereRadius() { return m_radius; }

void Model::SetDraw(bool draw_gold){m_Draw = draw_gold;}
bool Model::GetDraw() {	return m_Draw;}

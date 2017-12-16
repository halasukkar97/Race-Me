#pragma once
#include"ObjFileModel.h"


struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection; //64 bytes (4 x 4 = 16 floats x 4 bytes)
	// total size= 64 bytes
};

class  Model {
public:
	Model(ID3D11Device* Device, ID3D11DeviceContext* Context, float x, float y, float z);
	int LoadObjModel(char* filename);
	void Draw(XMMATRIX* view,XMMATRIX* projection);
	~Model();

   void  SetXPos(float x);
	void SetYPos(float y);
	void SetZPos(float z);

	void SetXRot(float xRotation);
	void SetYRot(float yRotation);
	void SetZRot(float zRotation);

	void SetScale(float scale);


	float GetXPos();
	float GetYPos();
	float GetZPos();

	float GetXRot();
	float GetYRot();
	float GetZRot();

	float GetScale();

	void set_texture(ID3D11ShaderResourceView* m_pTexture);
	void set_sampler(ID3D11SamplerState*   m_pSampler0);


private:
	ID3D11Device* m_pD3DDevice;
	ID3D11DeviceContext* m_pImmediateContext;

	ObjFileModel* m_pObject;
	ID3D11VertexShader* m_pVShader;
	ID3D11PixelShader* m_pPShader;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pConstantBuffer;

	ID3D11ShaderResourceView* m_pTexture;
	ID3D11SamplerState*   m_pSampler0;



	float m_x, m_y, m_z;
	float m_xangle, m_zangle, m_yangle;
	float m_scale;

	
	
};

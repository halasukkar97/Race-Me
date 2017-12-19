#include "Render.h"

Render::~Render()
{
	Clean();
}

Render::Render()
{

}

void Render::ClearBackBuffer()
{
	// Clear the back buffer and set the background color
	float rgba_clear_colour[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	pImmediateContext->ClearRenderTargetView(pBackBufferRTView, rgba_clear_colour);
	pImmediateContext->ClearDepthStencilView(pZBuffer, D3D11_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
}

void Render::SetLighting()
{
	//setting the light
	directionla_light_shines_from = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	directional_light_colour = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	ambient_light_colour = XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f);
}

void Render::SetVertexBuffer()
{
	//// Set vertex buffer 
	UINT stride = sizeof(model_player);
	UINT offset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer0);

	//XMMATRIX projection, world, view;
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), 640.0 / 480.0, 1.0f, 100.0f);
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Render::SetTextures()
{
	// setting the textures
	pImmediateContext->PSSetSamplers(0, 1, &pSampler);
	pImmediateContext->PSSetShaderResources(0, 1, &pTexture_player);
}

void Render::SetShaderObjects()
{
	//Set the Shader Objects as active
	pImmediateContext->VSSetShader(pVertexShader, 0, 0);
	pImmediateContext->PSSetShader(pPixelShader, 0, 0);
	pImmediateContext->IASetInputLayout(pInputLayout);
}

void Render::SetMovment(){

	//make the player camera following the player car
//	camera_player->LookAt_XZ(model_player->GetXPos(), model_player->GetZPos());

	//make the ai car look and go to flag
	model_ai->LookAt_XZ(model_flag->GetXPos(), model_flag->GetZPos());
	model_ai->MoveForward(0.003f);

	//make ai camera follow ai car
//	camera_ai->LookAt_XZ(model_ai->GetXPos(), model_ai->GetZPos());

}

void Render::checkCollisions()
{
	//if the ai go to flag make it go back a step and stop the camera from moving 
	if (model_ai->CheckCollision(model_flag))

	{
		model_ai->MoveForward(-0.5f);
		//camera_ai->Forward(0.000f);
	}
	else //if the ai is not there yet make the ai camera follow the ai car
	{
		//camera_ai->Forward(0.003f);
	}

	//check for collision between model ai and flag and make ai go a step back
	if (model_flag->CheckCollision(model_ai))
		model_ai->MoveForward(-0.5f);

	//if there is a collision between flag and player make player go back a step
	if (model_flag->CheckCollision(model_player))
		model_player->MoveForward(-0.5f);


	//check gold and plyer collision and take a gold to poket
	for (int i = 0; i < 50; i++)
	{
		if (model_gold[i]->CheckCollision(model_player))
		{
			money += 1;

			model_gold[i]->SetDraw(false);
		}
	}

	//check tree and plyer collision and go back a step when there is one
	for (int i = 0; i < 40; i++)
	{
		if (model_tree[i]->CheckCollision(model_player))
		{
			model_player->MoveForward(-0.5f);
		}
	}
}

void Render::ShowText() {

	//show text timer
	timer->AddText("123456", -1.0, -0.9, .1);
	timer->RenderText();

	//show in text how much money the player has
	moneyCount->AddText("money:  " + std::to_string(money), -1.0, 1, .1);
	moneyCount->RenderText();

}

void Render::Draw()
{
	//draw player , ai , flag
	model_player->Draw(&view, &projection);
	model_ai->Draw(&view, &projection);
	model_flag->Draw(&view, &projection);

	//draw gold and trees
	for (int i = 0; i < 50; i++) { if (model_gold[i]->GetDraw() == true) { model_gold[i]->Draw(&view, &projection); } }
	for (int i = 0; i < 40; i++) { model_tree[i]->Draw(&view, &projection); }

}

void Render::Clean()
{
}


void Render::RenderFrame(ID3D11DeviceContext* pImmediateContext, ID3D11RenderTargetView* pBackBufferRTView, ID3D11DepthStencilView*  pZBuffer, IDXGISwapChain*	pSwapChain)
{
	ClearBackBuffer();

	//read input
	input->ReadInputStates();
	input->Key_Logic();

	//calling the functions
	SetVertexBuffer();
	SetLighting();
	SetTextures();
	SetShaderObjects();

	//use the camera to view
	//view = camera_player->GetViewMatrix();

	//calling the functions
	SetMovment();
	checkCollisions();
	ShowText();
	Draw();

	// Display what has just been rendered
	pSwapChain->Present(0, 0);


}



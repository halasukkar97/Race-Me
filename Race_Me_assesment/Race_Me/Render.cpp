#include "Render.h"


void Render::RenderFrame(ID3D11DeviceContext* pImmediateContext)
{
	// Clear the back buffer - choose a colour you like
	float rgba_clear_colour[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	pImmediateContext->ClearRenderTargetView(pBackBufferRTView, rgba_clear_colour);
	pImmediateContext->ClearDepthStencilView(pZBuffer, D3D11_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);


	//read input
	input->ReadInputStates();
	input->Key_Logic();

	UINT stride = sizeof(model_player);
	UINT offset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer0);


	// RENDER HERE
	//XMMATRIX projection, world, view;
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), 640.0 / 480.0, 1.0f, 100.0f);
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	directionla_light_shines_from = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	directional_light_colour = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	ambient_light_colour = XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f);



	// setting the textures
	pImmediateContext->PSSetSamplers(0, 1, &pSampler);
	pImmediateContext->PSSetShaderResources(0, 1, &pTexture_player);
	//set the shader objects as active
	pImmediateContext->VSSetShader(pVertexShader, 0, 0);
	pImmediateContext->PSSetShader(pPixelShader, 0, 0);
	pImmediateContext->IASetInputLayout(pInputLayout);


	//use the camera to view
	view = camera_player->GetViewMatrix();

	camera_player->LookAt_XZ(model_player->GetXPos(), model_player->GetZPos());


	model_ai->LookAt_XZ(model_flag->GetXPos(), model_flag->GetZPos());
	model_ai->MoveForward(0.003f);


	camera_ai->LookAt_XZ(model_ai->GetXPos(), model_ai->GetZPos());





	if (model_ai->CheckCollision(model_player))
	{
		model_ai->MoveForward(-0.5f);
		camera_ai->Forward(0.000f);
	}
	else
	{
		camera_ai->Forward(0.003f);
	}


	if (model_flag->CheckCollision(model_ai))
		model_ai->MoveForward(-0.5f);


	if (model_flag->CheckCollision(model_player))
		model_player->MoveForward(-0.5f);



	for (int i = 0; i < 50; i++)
	{
		if (model_gold[i]->CheckCollision(model_player))
		{
			money += 1;

			model_gold[i]->SetDraw(false);
		}
	}


	for (int i = 0; i < 40; i++)
	{
		if (model_tree[i]->CheckCollision(model_player))
		{
			model_player->MoveForward(-0.5f);
		}
	}



	model_player->Draw(&view, &projection);
	model_ai->Draw(&view, &projection);
	model_flag->Draw(&view, &projection);

	for (int i = 0; i < 50; i++) { if (model_gold[i]->GetDraw() == true) { model_gold[i]->Draw(&view, &projection); } }
	for (int i = 0; i < 40; i++) { model_tree[i]->Draw(&view, &projection); }

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	timer->AddText("123456", -1.0, -0.9, .1);
	timer->RenderText();

	moneyCount->AddText("money:  " + std::to_string(money), -1.0, 1, .1);
	moneyCount->RenderText();


	// Display what has just been rendered
	pSwapChain->Present(0, 0);


}



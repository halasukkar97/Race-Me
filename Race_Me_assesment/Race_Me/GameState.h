#pragma once
#include"Render.h"
#include"Input.h"
#include"Model.h"
#include"Text2D.h"
//#include"camera.h"

class GameState {

public:

	GameState();
	~GameState();

private:

	Render* render;

	//adding objects
	Model*                      model_player;
	Model*                      model_ai;
	Model*                      model_flag;
	Model*                      model_gold[50];
	Model*                      model_tree[40];

	//adding input
	Input*                       input;

	//adding text
	Text2D*  timer;
	Text2D*  moneyCount;



};








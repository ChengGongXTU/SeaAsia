#pragma once
#include"SeeAisa.h"
#include"shape.h"
#include"camera.h"

class RayUnity
{
public:
	Shape* shape;

	void CreateDxUnity(Unity &dxunity);
};

class RayScene
{
public:
	int unityNum;
	RayUnity* unityList;


	Camera camera;

	int lightNum;
	RayLight** lightList;

	float backColor[3];

	RayScene()
	{
		unityList = NULL;
		lightList = NULL;

		backColor[0] = backColor[1] = backColor[2] = 0.0;

	}
	


};

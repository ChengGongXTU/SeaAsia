#pragma once
#include"SeeAisa.h"

class DxCamera {
	
public:

	XMVECTOR eye;
	XMVECTOR at; 
	XMVECTOR up;

	float cNear;
	float cFar;
	float aspect;
	float cWidth;
	float cHeight;

	XMMATRIX mProjection;
	XMMATRIX mView;
	XMMATRIX mView2;

	DxCamera() {}

	void Init(WindowsDevice &Dev);
	void Init(float x, float y, float z, float x1, float y1, float z1, float x2, float y2, float z2,
		float near1, float far1, float aspect1, float w1, float h1);

	void moveView(float x,float y,float z);
	void SetProjectionViewAngle(float newAspect);
	void SetProjectionNear(float nearV);

};

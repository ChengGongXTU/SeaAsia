#pragma once
#include"SeeAisa.h"
#include"geometry.h"
#include "transform.h"

class DxCamera {
	
public:

	Point eye;
	Point at;
	Vector up;
	Vector right;
	Vector dir;

	float cNear;
	float cFar;
	float aspect;
	float cWidth;
	float cHeight;

	Matrix4x4 mWorld;
	Matrix4x4 mProjection;
	Matrix4x4 mView;
	Matrix4x4 mView2;

	XMVECTOR quaternion;

	DxCamera() {}

	void Init(WindowsDevice &Dev);
	void Init(float x, float y, float z, float x1, float y1, float z1, float x2, float y2, float z2,
		float near1, float far1, float aspect1, float w1, float h1);

	void moveView(float x,float y,float z);
	void SetProjectionViewAngle(float newAspect);
	void SetProjectionNear(float nearV);
	void SetProjectionRatio(float ratio);
	void ResizeCamera();

};

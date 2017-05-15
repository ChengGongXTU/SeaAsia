#include"dxcamera.h"

void DxCamera::Init(WindowsDevice & Dev)
{	


	eye = XMVectorSet(0.0f, 120.0f, -200.f, 1.0f);
	at = XMVectorSet(0.0f, 120.0f, 201.0f, 1.0f);
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	aspect = M_PI / 4.f;
	cNear = 1.f;
	cFar = 10000.f;

	cWidth = 1.f;
	cHeight = 1.f;


	mView = XMMatrixLookAtLH(eye, at, up);
	mView2 = XMMatrixIdentity();
	mProjection = XMMatrixPerspectiveFovLH(aspect, cWidth / cHeight, cNear, cFar);
}

void DxCamera::Init(float x, float y, float z, float x1, float y1, float z1, float x2, float y2, float z2, float near1, float far1, float aspect1, float w1, float h1)
{
	eye = XMVectorSet(x, y, z, 1.0f);
	at = XMVectorSet(x1, y1, z1, 1.0f);
	up = XMVectorSet(x2, y2, z2, 0.0f);

	aspect = aspect1;
	cNear = near1;
	cFar = far1;

	cWidth = w1;
	cHeight = h1;


	mView = XMMatrixLookAtLH(eye, at, up);
	mView2 = XMMatrixIdentity();
	mProjection = XMMatrixPerspectiveFovLH(aspect, cWidth / cHeight, cNear, cFar);
}

void DxCamera::moveView(float x, float y, float z) {
	eye = eye + XMVectorSet(x,y, z, 0.0f);
	mView = XMMatrixLookAtLH(eye, at, up);
}

void DxCamera::SetProjectionViewAngle(float newAspect) {
	mProjection = XMMatrixPerspectiveFovLH(newAspect, 8 / (float)6, cNear, cFar);
}

void DxCamera::SetProjectionNear(float nearV) {
	mProjection = XMMatrixPerspectiveFovLH(aspect, 8 / (float)6, nearV, cFar);
}
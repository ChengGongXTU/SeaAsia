#pragma once
#include"dxcamera.h"

void DxCamera::Init(WindowsDevice & Dev)
{


	eye = Point(0.0f, 120.0f, -200.f);
	at = Point(0.0f, 120.0f, 201.0f);
	up = Vector(0.0f, 1.0f, 0.0f);

	aspect = 60;
	cNear = 1.f;
	cFar = 10000.f;

	cWidth = float(Dev.w);
	cHeight = Dev.h;


	mView = LookAt(eye, at, up).m;
	mView2 = MatrixIdentity().m;
	mProjection = XMMatrixPerspectiveFovLH(aspect, cWidth / cHeight, cNear, cFar);
}

void DxCamera::Init(float x, float y, float z, float x1, float y1, float z1, float x2, float y2, float z2, float near1, float far1, float aspect1, float w1, float h1)
{
	eye = Point(x, y, z);
	at = Point(x1, y1, z1);
	up = Vector(x2, y2, z2);

	aspect = aspect1;
	cNear = near1;
	cFar = far1;

	cWidth = w1;
	cHeight = h1;

	dir = Normalize(at - eye);
	up = Normalize(up);

	XMVECTOR xmDir = XMVectorSet(dir.x, dir.y, dir.z, 1.0);
	XMVECTOR xmUp = XMVectorSet(up.x, up.y, up.z, 1.0);
	XMVECTOR xmRight = XMVector3Cross(xmDir, xmUp);
	xmUp = XMVector3Cross(xmRight, xmDir);

	right = Vector(XMVectorGetX(xmRight), XMVectorGetY(xmRight), XMVectorGetZ(xmRight));
	up = Vector(XMVectorGetX(xmUp), XMVectorGetY(xmUp), XMVectorGetZ(xmUp));
	
	mView = LookAt(eye, at, up).m;
	mView2 = MatrixIdentity().m;
	mProjection = MatrixPerspectiveFov(aspect, cWidth / cHeight, cNear, cFar).m;
}

void DxCamera::moveView(float x, float y, float z) {
	eye = eye + Vector(x, y, z);
	mView = LookAt(eye, at, up).m;
}

void DxCamera::SetProjectionViewAngle(float newAspect) {
	mProjection = MatrixPerspectiveFov(newAspect, cWidth / cHeight, cNear, cFar).m;
}

void DxCamera::SetProjectionNear(float nearV) {
	mProjection = MatrixPerspectiveFov(aspect, cWidth / cHeight, nearV, cFar).m;
}
void DxCamera::SetProjectionRatio(float ratio)
{
	mProjection = MatrixPerspectiveFov(aspect, ratio, cNear, cFar).m;
}

void DxCamera::ResizeCamera()
{
	mView = LookAt(eye, at, up).m;
	mView2 = MatrixIdentity().m;
	mProjection = XMMatrixPerspectiveFovLH(aspect, cWidth / cHeight, cNear, cFar);
}

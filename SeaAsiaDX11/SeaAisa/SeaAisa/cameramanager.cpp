#include "cameramanager.h"

void CameraManager::StartUp(DxDevice &dxDev)
{
	ZeroMemory(&bd, sizeof(bd));
	viewTransformBuffer = NULL;
	projTransformBuffer = NULL;
	worldjTransformBuffer = NULL;
	viewTransform.mView = XMMatrixIdentity();
	projectionTransform.mProjection = XMMatrixIdentity();
	CreateWorldBuffer(dxDev);
	CreateViewBuffer(dxDev);
	CreateProjectionBuffer(dxDev);
}


void CameraManager::ShutUp()
{
	ZeroMemory(&bd, sizeof(bd));
	viewTransformBuffer->Release();
	viewTransformBuffer = NULL;
	projTransformBuffer->Release();
	projTransformBuffer = NULL;
	worldjTransformBuffer->Release();
	worldjTransformBuffer = NULL;
	viewTransform.mView = XMMatrixIdentity();
	projectionTransform.mProjection = XMMatrixIdentity();
}

bool CameraManager::CreateViewBuffer(DxDevice & dev)
{

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ViewTransform);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = dev.device->CreateBuffer(&bd, NULL, &viewTransformBuffer);

	if (FAILED(hr))	return false;

	return true;
}

bool CameraManager::CreateProjectionBuffer(DxDevice & dev)
{
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ProjectionTranaform);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = dev.device->CreateBuffer(&bd, NULL, &projTransformBuffer);

	if (FAILED(hr))	return false;

	return true;
}

bool CameraManager::CreateWorldBuffer(DxDevice & dev)
{
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WorldTransform);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = dev.device->CreateBuffer(&bd, NULL, &worldjTransformBuffer);

	if (FAILED(hr))	return false;

	return true;
}

bool CameraManager::LoadCamera(DxDevice &dev, DxCamera & camera)
{	
	worldTransform.mWorld = XMLoadFloat4x4(&camera.mWorld.m);
	worldTransform.mWorld = XMMatrixTranspose(worldTransform.mWorld);
	dev.context->UpdateSubresource(worldjTransformBuffer, 0, NULL, &worldTransform, 0, 0);

	viewTransform.mView = XMLoadFloat4x4(&camera.mView.m);
	viewTransform.mView = XMMatrixTranspose(viewTransform.mView);
	viewTransform.mView2 = XMLoadFloat4x4(&camera.mView.m);
	XMVECTOR vec = XMMatrixDeterminant(viewTransform.mView2);
	viewTransform.mView2 = XMMatrixInverse(&vec, viewTransform.mView2);
	viewTransform.mView2 = XMMatrixTranspose(viewTransform.mView2);
	dev.context->UpdateSubresource(viewTransformBuffer, 0, NULL, &viewTransform, 0, 0);



	projectionTransform.mProjection = XMLoadFloat4x4(&camera.mProjection.m);
	projectionTransform.mProjection = XMMatrixTranspose(projectionTransform.mProjection);
	dev.context->UpdateSubresource(projTransformBuffer, 0, NULL, &projectionTransform, 0, 0);

	return true;
}

bool CameraManager::InputCamera(DxDevice & dev)
{
	//dev.context->VSSetConstantBuffers(0, 1, &worldjTransformBuffer);
	dev.context->VSSetConstantBuffers(1, 1, &viewTransformBuffer);
	dev.context->VSSetConstantBuffers(2, 1, &projTransformBuffer);

	return true;
}


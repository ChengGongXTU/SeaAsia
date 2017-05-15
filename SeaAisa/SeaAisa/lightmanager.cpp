#include "lightmanager.h"

void LightManager::StartUp()
{
	PointLightBuffer = NULL;
	DirLightBuffer = NULL;
}

void LightManager::ShutUp()
{
	PointLightBuffer = NULL;
	DirLightBuffer = NULL;
}

bool LightManager::CreateBuffer(DxDevice & dev,DxDirLight &dl)
{
	D3D11_BUFFER_DESC dlDesc;
	ZeroMemory(&dlDesc, sizeof(dlDesc));

	dlDesc.Usage = D3D11_USAGE_DEFAULT;
	dlDesc.ByteWidth = sizeof(DxDirLight);
	dlDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dlDesc.CPUAccessFlags = 0;

	HRESULT hr = dev.device->CreateBuffer(&dlDesc, NULL, &DirLightBuffer);

	if (FAILED(hr))	return false;

	return true;
}

bool LightManager::CreateBuffer(DxDevice & dev, DxPointLight & pl)
{	
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DxPointLight);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = dev.device->CreateBuffer(&bd, NULL, &PointLightBuffer);

	if (FAILED(hr))	return false;

	return true;
}

bool LightManager::SetDirLight(DxDevice &dev, DxDirLight &dirLight)
{	

	dev.context->UpdateSubresource(DirLightBuffer, 0, NULL,&dirLight, 0, 0);
	dev.context->PSSetConstantBuffers(2, 1, &DirLightBuffer);
	return true;
}

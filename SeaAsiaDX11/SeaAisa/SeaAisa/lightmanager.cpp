#include "lightmanager.h"

void LightManager::StartUp()
{
	PointLightBuffer = new ID3D11Buffer*[1000];
	DirLightBuffer = new ID3D11Buffer*[1000];
	SpotLightBuffer = new ID3D11Buffer*[1000];

	DirLightSRV = new ID3D11ShaderResourceView*[1000];
	PointLightSRV = new ID3D11ShaderResourceView*[1000];
	SpotLightSRV = new ID3D11ShaderResourceView*[1000];

	dxDirLights = new DxDirLight[1000];
	dxPointLights = new DxPointLight[1000];
	dxSpotLights = new DxSpotLight[1000];

	for (int i = 0; i < 1000; i++)
	{
		PointLightBuffer[i] = NULL;
		DirLightBuffer[i] = NULL;
		SpotLightBuffer[i] = NULL;
		DirLightSRV[i] = NULL;
		PointLightSRV[i] = NULL;
		SpotLightSRV[i] = NULL;

		dxDirLights[i].Color = XMFLOAT4(0, 0, 0, 1);
		dxDirLights[i].intensity = 0;
		dxDirLights[i].Dir = XMFLOAT4(0.707, -0.707, 0, 0);
		dxDirLights[i].up = XMFLOAT4(0, 1, 0, 0);
		dxDirLights[i].right = XMFLOAT4(1, 0, 0, 0);

		dxPointLights[i].Color = XMFLOAT4(0, 0, 0, 1);
		dxPointLights[i].intensity = 0;
		dxPointLights[i].Pos = XMFLOAT4(0, 0, 0, 1);
		dxPointLights[i].range = 0;

		dxSpotLights[i].Color = XMFLOAT4(0, 0, 0, 1);
		dxSpotLights[i].intensity = 0;
		dxSpotLights[i].Pos = XMFLOAT4(0, 0, 0, 1);
		dxSpotLights[i].Dir = XMFLOAT4(0.707, -0.707, 0, 0);
		dxSpotLights[i].range = 0;
		dxSpotLights[i].angle_p = 0;
		dxSpotLights[i].angle_u = 1;

		endDirLightID = 0;
		endPointLightID = 0;
		endSpotLightID = 0;

		dirLightCount = 0;
		pointLightCount = 0;
		spotLightCount = 0;
	
	}

	DirLightType = 0;
	PointLightType = 1;
	SpotLightType = 2;

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
	dlDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	dlDesc.CPUAccessFlags = 0;
	dlDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	dlDesc.StructureByteStride = sizeof(DxDirLight);

	HRESULT hr = dev.device->CreateBuffer(&dlDesc, NULL, &DirLightBuffer[endDirLightID]);

	if (FAILED(hr))	return false;

	dev.context->UpdateSubresource(DirLightBuffer[endDirLightID], 0, 0,
		&dxDirLights[endDirLightID], 0, 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = 1;

	hr = dev.device->CreateShaderResourceView(DirLightBuffer[endDirLightID], &desc, &DirLightSRV[endDirLightID]);

	if (FAILED(hr))	return false;

	endDirLightID++;
	dirLightCount++;
	return true;

}

bool LightManager::CreateBuffer(DxDevice & dev, DxPointLight & pl)
{	
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DxPointLight);
	bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd.StructureByteStride = sizeof(DxPointLight);

	HRESULT hr = dev.device->CreateBuffer(&bd, NULL, &PointLightBuffer[endPointLightID]);

	if (FAILED(hr))	return false;

	dev.context->UpdateSubresource(PointLightBuffer[endPointLightID], 0, 0, 
		&dxPointLights[endPointLightID], 0, 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = 1;

	hr = dev.device->CreateShaderResourceView(PointLightBuffer[endPointLightID], &desc, &PointLightSRV[endPointLightID]);

	if (FAILED(hr))	return false;

	endPointLightID++;
	pointLightCount++;
	return true;
}

bool LightManager::CreateBuffer(DxDevice & dev, DxSpotLight & pl)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DxSpotLight);
	bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd.StructureByteStride = sizeof(DxSpotLight);

	HRESULT hr = dev.device->CreateBuffer(&bd, NULL, &SpotLightBuffer[endSpotLightID]);

	if (FAILED(hr))	return false;

	dev.context->UpdateSubresource(SpotLightBuffer[endSpotLightID], 0, 0,
		&dxSpotLights[endSpotLightID], 0, 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = 1;

	hr = dev.device->CreateShaderResourceView(SpotLightBuffer[endSpotLightID], &desc, &SpotLightSRV[endSpotLightID]);

	if (FAILED(hr))	return false;

	endSpotLightID++;
	spotLightCount++;
	return true;
}

bool LightManager::SetDirLight(DxDevice &dev, DxDirLight &dirLight)
{	

	//dev.context->UpdateSubresource(DirLightBuffer, 0, NULL,&dirLight, 0, 0);
	//dev.context->PSSetConstantBuffers(3, 1, &DirLightBuffer);
	return false;
	return true;
}

bool LightManager::AddDirLight(DxDevice & dev, XMFLOAT4 color, float intensity, XMFLOAT4 dir)
{	
	DxDirLight& dirlight = dxDirLights[endDirLightID];
	dirlight.Color = color;
	dirlight.Dir = dir;
	dirlight.intensity = intensity;

	Vector forward = Vector(dirlight.Dir.x, dirlight.Dir.y, dirlight.Dir.z);
	Vector up;
	Vector right;
	if (forward.z >= 0)
	{
		up = Vector(0.0, 1.0, 0.0);
		right = Normalize(Cross(up, forward));

	}
	else
	{
		up = Vector(0.0, -1.0, 0.0);
		right = Normalize(Cross(up, forward));
	}
	up = Normalize(Cross(forward, right));

	dirlight.up = XMFLOAT4(up.x, up.y, up.z, 0.0);
	dirlight.right = XMFLOAT4(right.x, right.y, right.z, 0.0);

	CreateBuffer(dev, dirlight);
	return true;
}

bool LightManager::AddPointLight(DxDevice & dev, XMFLOAT4 color,float intensity, XMFLOAT4 wPos, float range )
{
	DxPointLight& pointlight = dxPointLights[endPointLightID];
	pointlight.Color = color;
	pointlight.Pos = wPos;
	pointlight.intensity = intensity;
	pointlight.range = range;

	CreateBuffer(dev, pointlight);

	return true;
}

bool LightManager::AddPointLight(DxDevice & dev, XMFLOAT4 color, float intensity, XMFLOAT4 dir, XMFLOAT4 wPos, float range, float inner_angle, float outer_angle)
{
	DxSpotLight& spotlight = dxSpotLights[endSpotLightID];
	spotlight.Color = color;
	spotlight.Pos = wPos;
	spotlight.Dir = dir;
	spotlight.intensity = intensity;
	spotlight.range = range;
	spotlight.angle_p = inner_angle;
	spotlight.angle_u = outer_angle;

	CreateBuffer(dev, spotlight);

	return true;
}
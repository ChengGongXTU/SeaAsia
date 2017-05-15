#include"texturemanager.h"

void TextureManager::CreateDxMemory(int ObjNum) {
	texViewPointer = new ID3D11ShaderResourceView*[ObjNum];
	sampDesc = new D3D11_SAMPLER_DESC[ObjNum];
	sampleStatePointer = new ID3D11SamplerState*[ObjNum];
}

void TextureManager::StartUp() {
	totalTexureNumber = 0;
	endTextureId = 0;
	currentTextureId = 0;

	totalSamnpleDescNumber = 0;
	endSampleDescId = 0;
	currentSampleDescId = 0;

	totalSamplerSateNumber = 0;
	endSamplerStateId = 0;
	currentSamplerStateId = 0;

	CreateDxMemory(500);
}

void TextureManager::ShutUp() {
	delete texViewPointer;
	delete sampDesc;
	delete sampleStatePointer;
}

bool TextureManager::DxLoadTexture(wstring fileName, DxDevice & dxDev)
{	
	if (fileName[0] == NULL)	return true;

	 HRESULT hr = D3DX11CreateShaderResourceViewFromFile(dxDev.device, fileName.c_str(), NULL, NULL, &texViewPointer[endTextureId], NULL);
	 if (FAILED(hr))	return 0;

	 currentTextureId = endTextureId;
	 endTextureId++;
	 totalTexureNumber++;
	 return true;
}

bool TextureManager::DxSetSamplerDesc(D3D11_FILTER filterType,
	D3D11_TEXTURE_ADDRESS_MODE adddU,
	D3D11_TEXTURE_ADDRESS_MODE adddV, 
	D3D11_TEXTURE_ADDRESS_MODE adddW,
	D3D11_COMPARISON_FUNC comparType,
	FLOAT minLod,FLOAT maxLod,
	UINT maxAni,
	FLOAT mipLodBias)
{
	sampDesc[endSampleDescId];
	ZeroMemory(&sampDesc[endSampleDescId], sizeof(sampDesc[endSampleDescId]));

	if (sampDesc[endSampleDescId].Filter != 0) return false;

	sampDesc[endSampleDescId].Filter = filterType;
	sampDesc[endSampleDescId].AddressU = adddU;
	sampDesc[endSampleDescId].AddressV = adddV;
	sampDesc[endSampleDescId].AddressW = adddW;
	sampDesc[endSampleDescId].ComparisonFunc = comparType;
	sampDesc[endSampleDescId].MinLOD = minLod;
	sampDesc[endSampleDescId].MaxLOD = maxLod;
	sampDesc[endSampleDescId].MaxAnisotropy = maxAni;
	sampDesc[endSampleDescId].MipLODBias = mipLodBias;

	endSampleDescId++;

	return true;


}

bool TextureManager::DxSetSamplerState(int &samplerDescId, DxDevice &dxDevice)
{
	HRESULT hr = dxDevice.device->CreateSamplerState(&sampDesc[samplerDescId], &sampleStatePointer[samplerDescId]);

	if (FAILED(hr))	return false;

	if (endSamplerStateId < samplerDescId)	endSamplerStateId = samplerDescId;

	return true;
}

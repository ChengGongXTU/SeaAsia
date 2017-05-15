#include "shadermanager.h"

void ShaderManager::StartUp()
{	
	vsBlob = new ID3DBlob*[100];
	psBlob = new ID3DBlob*[100];

	VS = NULL;
	PS = NULL;

	endVSId = 0;
	currentVSId = 0;

	endPSId = 0;
	currentPSId = 0;
}

void ShaderManager::ShutUp()
{
	VS = NULL;
	PS = NULL;

	endVSId = 0;
	currentVSId = 0;

	endPSId = 0;
	currentPSId = 0;

	for (int i = 0; i < endVSId; i++) {
		delete vsBlob[i];
	}
	for (int i = 0; i < endPSId; i++) {
		delete psBlob[i];
	}

	delete vsBlob;
	delete psBlob;
}

bool ShaderManager::LoadVertexShader(WCHAR* shaderName, LPCSTR vsMain,LPCSTR vsVersion)
{	
	if(error != NULL) error->Release();
	HRESULT hr = D3DX11CompileFromFile(shaderName, 0, 0, vsMain, vsVersion,
		D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, NULL, &vsBlob[endVSId], &error, NULL);

	if (FAILED(hr)) {
		return 0;
	}

	if(error)
	{
		MessageBoxA(NULL, (LPCSTR)error->GetBufferPointer(), "1", 0);
	}
	endVSId++;

	return true;
}

bool ShaderManager::CreateVertexShader(DxDevice & dev, int vsId)
{
	HRESULT hr = dev.device->CreateVertexShader(vsBlob[vsId]->GetBufferPointer(), vsBlob[vsId]->GetBufferSize(), 0, &VS);

	if (FAILED(hr))	return 0;

	return true;
}

bool ShaderManager::LoadAndCreateVertexShader(WCHAR* shaderName, LPCSTR vsMain, LPCSTR vsVersion, DxDevice & dev)
{
	if (!LoadVertexShader(shaderName, vsMain,vsVersion))	return false;
	if (!CreateVertexShader(dev, endVSId - 1)) return false;
	return true;
}

bool ShaderManager::LoadPixelShader(WCHAR* shaderName, LPCSTR psMain, LPCSTR psVersion)
{	
	HRESULT hr = D3DX11CompileFromFile(shaderName, 0, 0, psMain, psVersion, D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, 0, &psBlob[endPSId], NULL, 0);

	if (FAILED(hr)) {
		return 0;
	}
	endPSId++;

	return true;
}

bool ShaderManager::CreatePixelShader(DxDevice & dev, int psId)
{
	HRESULT hr = dev.device->CreatePixelShader(psBlob[psId]->GetBufferPointer(), psBlob[psId]->GetBufferSize(), 0, &PS);

	if (FAILED(hr))	return 0;

	return true;
}

bool ShaderManager::LoadAndCreatePixelShader(WCHAR* shaderName, LPCSTR psMain, LPCSTR psVersion, DxDevice & dev)
{
	if (!LoadPixelShader(shaderName,psMain, psVersion))	return false;
	if (!CreatePixelShader(dev, endPSId - 1)) return false;
	return true;
}

bool ShaderManager::InputVertexShader(DxDevice &dev)
{
	dev.context->VSSetShader(VS, NULL, 0);
	return true;
}

bool ShaderManager::InputPixelShader(DxDevice & dev)
{
	dev.context->PSSetShader(PS, NULL, 0);
	return true;
}

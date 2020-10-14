#include "shadermanager.h"

void ShaderManager::StartUp(DxDevice &dev)
{	
	vsBlob = new ID3DBlob*[500];
	psBlob = new ID3DBlob*[500];

	VS = new ID3D11VertexShader*[500];
	PS = new ID3D11PixelShader*[500];

	for (int i = 0; i < 500; i++)
	{
		VS[i] = NULL;
		PS[i] = NULL;
		vsBlob[i] = NULL;
		psBlob[i] = NULL;
	}

	endVSId = 0;
	currentVSId = 0;

	endPSId = 0;
	currentPSId = 0;

	LoadDefaultShader(dev);
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
	HRESULT hr = D3DCompileFromFile(shaderName, 0, 0, vsMain, vsVersion,
		D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vsBlob[endVSId], &error);

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
	HRESULT hr = dev.device->CreateVertexShader(vsBlob[vsId]->GetBufferPointer(), vsBlob[vsId]->GetBufferSize(), 0, &VS[vsId]);

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
	HRESULT hr = D3DCompileFromFile(shaderName, 0, 0, psMain, psVersion, D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &psBlob[endPSId], &error);

	if (FAILED(hr)) {
		return 0;
	}
	endPSId++;

	return true;
}

bool ShaderManager::CreatePixelShader(DxDevice & dev, int psId)
{
	HRESULT hr = dev.device->CreatePixelShader(psBlob[psId]->GetBufferPointer(), psBlob[psId]->GetBufferSize(), 0, &PS[psId]);

	if (FAILED(hr))	return 0;

	return true;
}

bool ShaderManager::LoadAndCreatePixelShader(WCHAR* shaderName, LPCSTR psMain, LPCSTR psVersion, DxDevice & dev)
{
	if (!LoadPixelShader(shaderName,psMain, psVersion))	return false;
	if (!CreatePixelShader(dev, endPSId - 1)) return false;
	return true;
}

bool ShaderManager::InputVertexShader(DxDevice &dev, int vsId)
{
	dev.context->VSSetShader(VS[vsId], NULL, 0);
	return true;
}

bool ShaderManager::InputPixelShader(DxDevice & dev, int psId)
{
	dev.context->PSSetShader(PS[psId], NULL, 0);
	return true;
}

bool ShaderManager::LoadDefaultShader(DxDevice & dev)
{	

	//1
	char PBRGbufferNameChar[128] = "Shader/PBRGeometryPass.fx"; // "hlsl.fx";
	//2
	char PBRShadingNameChar[128] = "Shader/PBRLightShading.fx"; // "hlsl.fx";
	//3
	char SkyboxNameChar[128] = "Shader/Skybox.fx"; // "hlsl.fx";
	//4
	char PostProcessNameChar[128] = "Shader/PostProcess.fx"; // "hlsl.fx";
	//5
	char ShadowNameChar[128] = "Shader/Shadowmap.fx";

	if (!LoadShader(PBRGbufferNameChar, dev))
	{
		ImGui::OpenPopup("PBR Geometry Error");
	}


	if (!LoadShader(PBRShadingNameChar, dev))
	{
		ImGui::OpenPopup("PBR shading Error");
	}

	if (!LoadShader(SkyboxNameChar, dev))
	{
		ImGui::OpenPopup("skybox shading Error");
	}

	if (!LoadShader(PostProcessNameChar, dev))
	{
		ImGui::OpenPopup("pp shading Error");
	}

	if (!LoadShader(ShadowNameChar, dev))
	{
		ImGui::OpenPopup("shadow shading Error");
	}

	return true;
}

bool ShaderManager::LoadShader(char * shaderName, DxDevice & dev)
{	
	char vsName[128] = "VS";
	char vsVersion[128] = "vs_5_0";
	char psName[128] = "PS";
	char psVersion[128] = "ps_5_0";

	wchar_t shaderNameWchar[129] = L"";
	CharToWchar(shaderName, shaderNameWchar);

	if (!LoadAndCreateVertexShader(shaderNameWchar, vsName, vsVersion, dev))
	{
		return false;
	}
	else if (!LoadAndCreatePixelShader(shaderNameWchar, psName, psVersion, dev))
	{
		return false;
	}

	return true;
}

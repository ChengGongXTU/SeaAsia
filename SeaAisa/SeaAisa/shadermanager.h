#pragma once
#include"SeeAisa.h"
#include"dxdevice.h"

class ShaderManager {
public:

	ID3DBlob** vsBlob;
	ID3DBlob** psBlob;

	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3DBlob* error;

	int endVSId;
	int currentVSId;

	int endPSId;
	int currentPSId;

	ShaderManager(){}
	~ShaderManager(){}

	void StartUp();
	void ShutUp();

	bool LoadVertexShader(WCHAR* shaderName, LPCSTR vsMain, LPCSTR vsVersion);
	bool CreateVertexShader(DxDevice &dev, int vsId);
	bool LoadAndCreateVertexShader(WCHAR* shaderName, LPCSTR vsMain, LPCSTR vsVersion, DxDevice &dev);
	bool LoadPixelShader(WCHAR* shaderName, LPCSTR psMain, LPCSTR psVersion);
	bool CreatePixelShader(DxDevice &dev, int psId);
	bool LoadAndCreatePixelShader(WCHAR* shaderName, LPCSTR psMain ,LPCSTR psVersion, DxDevice &dev);
	bool InputVertexShader(DxDevice &dev);
	bool InputPixelShader(DxDevice &dev);
};

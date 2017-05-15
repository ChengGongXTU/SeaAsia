#pragma once

#include"SeeAisa.h"
#include"dxdevice.h"

class TextureManager {

public:
	ID3D11ShaderResourceView** texViewPointer;
	D3D11_SAMPLER_DESC* sampDesc;
	ID3D11SamplerState** sampleStatePointer;

	int totalTexureNumber;
	int endTextureId;
	int currentTextureId;

	int totalSamnpleDescNumber;
	int	endSampleDescId;
	int currentSampleDescId;

	int totalSamplerSateNumber;
	int endSamplerStateId;
	int currentSamplerStateId;

	TextureManager() {}

	~TextureManager() {}

	void CreateDxMemory(int ObjNum);

	void StartUp();

	void ShutUp();

	bool DxLoadTexture(wstring fileName, DxDevice &dxDev);
	bool DxSetSamplerDesc(D3D11_FILTER filterType,
		D3D11_TEXTURE_ADDRESS_MODE adddU,
		D3D11_TEXTURE_ADDRESS_MODE adddV,
		D3D11_TEXTURE_ADDRESS_MODE adddW,
		D3D11_COMPARISON_FUNC comparType,
		FLOAT minLod, FLOAT maxLod,
		UINT maxAni,
		FLOAT mipLodBias);
	bool DxSetSamplerState(int &samplerDescId, DxDevice &dxDevice);
};

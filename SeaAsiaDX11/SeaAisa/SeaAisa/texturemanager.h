#pragma once

#include"SeeAisa.h"
#include"dxdevice.h"

class TextureManager {

public:
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_SUBRESOURCE_DATA sd;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	ID3D11ShaderResourceView** texViewPointer;
	D3D11_SAMPLER_DESC* sampDesc;
	ID3D11SamplerState** sampleStatePointer;
	uint32_t** textures;
	ID3D11Texture2D** dxTexure2D;
	const char** texNames;


	ID3D11ShaderResourceView** cubemapSRVs;
	D3D11_SAMPLER_DESC* cubemapSampDesc;
	ID3D11SamplerState** cubemapSampleStatePointer;
	uint32_t** cubemapTextures;
	ID3D11Texture2D** dxCubemapTexture2D;
	const char** cubemapNames;
	UINT* cubemapMipmapCount;

	int totalTexureNumber;
	int endTextureId;
	int currentTextureId;

	int totalSamnpleDescNumber;
	int	endSampleDescId;
	int currentSampleDescId;

	int totalSamplerSateNumber;
	int endSamplerStateId;
	int currentSamplerStateId;

	int totalCubemapNumber;
	int endCubemapTexId;
	int endCubemapId;
	int currentCubemapTexId;


	TextureManager() {}

	~TextureManager() {}

	void CreateDxMemory(int ObjNum);

	void StartUp(DxDevice &dxDev);

	void ShutUp();

	bool DxLoadTexture(wstring fileName, DxDevice &dxDev);
	bool DxLoadImage(const char* fileName, DxDevice &dxDev , bool sRGB);
	bool DxSetSamplerDesc(D3D11_FILTER filterType,
		D3D11_TEXTURE_ADDRESS_MODE adddU,
		D3D11_TEXTURE_ADDRESS_MODE adddV,
		D3D11_TEXTURE_ADDRESS_MODE adddW,
		D3D11_COMPARISON_FUNC comparType,
		FLOAT minLod, FLOAT maxLod,
		UINT maxAni,
		FLOAT mipLodBias);
	bool LoadDefaultImage(DxDevice & dxDev);
	bool DxSetSamplerState(int &samplerDescId, DxDevice &dxDevice);

	bool DxSetCubeSamplerState(D3D11_FILTER filterType, D3D11_TEXTURE_ADDRESS_MODE adddU, D3D11_TEXTURE_ADDRESS_MODE adddV, D3D11_TEXTURE_ADDRESS_MODE adddW, D3D11_COMPARISON_FUNC comparType, FLOAT minLod, FLOAT maxLod, UINT maxAni, DxDevice &dxDevice);

	bool DxLoadNonHDRCubemap(const char * fileName, DxDevice & dxDev, bool sRGB);

	bool DxLoadHDRImage(const char * fileName, DxDevice & dxDev, bool sRGB);
};

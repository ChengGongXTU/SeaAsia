//come from dx11 coded in web: http://www.rastertek.com/dx11tut42.html

#pragma once
#include"SeeAisa.h"
#include"shadermanager.h"
#include "basicmanager.h"
#include "primitivemanager.h"
#include "lightmanager.h"

class ShadowManager
{
public:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};

	struct LightBufferType2
	{
		XMVECTOR lightPosition;
	};


public:
	ID3D11VertexShader** vertexShader;
	ID3D11PixelShader** pixelShader;
	ID3DBlob** vsBlob;
	ID3DBlob** psBlob;

	ID3D11InputLayout* layout;
	ID3D11SamplerState* sampleStateClamp;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer2;

	ID3D11Texture2D* rt;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* rtSV;

	ID3D11DepthStencilView* dsv;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11DepthStencilState* depthState;
	D3D11_DEPTH_STENCIL_DESC depthDesc;


	D3D11_VIEWPORT vp;

	UINT width;
	UINT height;

	MatrixBufferType transposeLightMatrices;
	LightBufferType2 lightPosition;

	ShadowManager()
	{
		vertexShader = NULL;
		pixelShader = NULL;
		layout = NULL;
		sampleStateClamp = NULL;
		matrixBuffer = NULL;
		lightBuffer2 = NULL;
	}

	bool Init(ID3D11Device* device, DxDevice* dxDev, ShaderManager &shaderMng);
	bool SetShaderParameter(
		ID3D11DeviceContext* context,
		XMMATRIX world, XMMATRIX view, XMMATRIX projection,
		ID3D11ShaderResourceView* vsRV, ID3D11ShaderResourceView* psRV,
		XMVECTOR ambientColor, XMVECTOR diffuseColor, XMVECTOR lightPosition);
	bool DrawShadow(ID3D11DeviceContext* context,
		BasicManager & basicMng, Unity & unity,
		PrimitiveManager& primitiveMng);

	bool DrawSceneShadow(
		BasicManager & basicMng,
		int SceneId, int cameraId, int dlightId,
		PrimitiveManager& primitiveMng,
		LightManager& lightMng);

	bool SetShaderParameter(
		ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
		XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix,
		ID3D11ShaderResourceView* depthMapTexture, XMVECTOR lightPosition);
};

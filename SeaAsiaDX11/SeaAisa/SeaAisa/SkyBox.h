#pragma once
#include"SeeAisa.h"
#include"dxdevice.h"
#include"dxcamera.h"
#include"shadermanager.h"
#include"basicmanager.h"


struct SkyBoxMeshVertex
{	
	SkyBoxMeshVertex(){}
	SkyBoxMeshVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz)
	{
		pos = XMFLOAT3(x, y, z);
		//uv = XMFLOAT2(u, v);
		//normal = XMFLOAT3(nx, ny, nz);
	}
	XMFLOAT3 pos;
	//XMFLOAT2 uv;
	//XMFLOAT3 normal;
};

class SkyBox
{
public:
	SkyBoxMeshVertex* skyBoxMeshVerices;
	DWORD* indices;
	UINT vertexNum;
	UINT faceNum;

	XMMATRIX Rotationx;
	XMMATRIX Rotationy;
	XMMATRIX Scale;
	XMMATRIX Translation;
	XMMATRIX sphereWorld;

	D3D11_INPUT_ELEMENT_DESC* vertexLayout;
	UINT  layoutNum;

	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;

	ID3D11ShaderResourceView* skyboxCubemapSRV;
	ID3D11SamplerState* skyBoxCubemapSampleState;
	//test
	int texIndex = 3;
	ID3D11ShaderResourceView* skyBoxTexViewPointer;
	ID3D11SamplerState* skyBoxTexSampleStatePointer;

	UINT vertexShaderIndex;
	UINT pixelShaderIndex;

	ID3D11InputLayout* skyBoxInputLayout;

	ID3D11Buffer* wvpBuffer;

	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;
	ID3D11RasterizerState* RSCullBack;

	SkyBox() {}
	~SkyBox() {}

	//come from web: https://www.braynzarsoft.net/viewtutorial/q16390-20-cube-mapping-skybox
	void CreateSphereMesh(float radius, int sliceCount, int stackCount, DxDevice &dxDev);
	void Startup(BasicManager &basicMng, ShaderManager & shaderMng);
	void ShutUp();
	void RenderSkyBox(ShaderManager & shaderMng, DxDevice &dxdev, DxCamera & camera);
	void ChooseTexture(TextureManager& texMng, int texureIndex);
};

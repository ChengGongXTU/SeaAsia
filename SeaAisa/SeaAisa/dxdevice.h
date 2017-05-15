#pragma once

#include"SeeAisa.h"

class DxDevice
{
public:
	// data
	UINT creatDeviceFlag;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	D3D_FEATURE_LEVEL featureLevel;
	IDXGIDevice* dxgiDevice;
	IDXGIAdapter* dxgiAdapter;
	IDXGIFactory* dxgiFactory;


	DXGI_SWAP_CHAIN_DESC sd;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* rtv;
	ID3D11DepthStencilView* dsv;

	D3D11_VIEWPORT vp;

	ID3DX11Effect* effect;
	ID3D11InputLayout* inputLayouyt;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constantBuffer;

	ID3D11VertexShader* vertexShaer;
	ID3D11PixelShader* pixelShader;

	XMMATRIX gWorld;
	XMMATRIX gView;
	XMMATRIX gProjection;

	DxDevice() {
		creatDeviceFlag = NULL;
		device = NULL;
		context = NULL;
		dxgiDevice = NULL;
		dxgiAdapter = NULL;
		dxgiFactory = NULL;

		swapChain = NULL;
		rtv = NULL;
		dsv = NULL;

		effect = NULL;
		inputLayouyt = NULL;

		vertexBuffer = NULL;
		indexBuffer = NULL;
		constantBuffer = NULL;

		vertexShaer = NULL;
		pixelShader = NULL;
	}

	//method
	HRESULT CreateDevice();

	HRESULT CreateDeviceAndSwap();

	bool Init(WindowsDevice &Dev);
	
	void CleanupRenderTarget();

	void CreateRenderTarget();

};



#pragma once

#include"SeeAisa.h"
#include"dxdevice.h"

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
	ID3D11Texture2D* rtt[6];
	ID3D11RenderTargetView* rtv[6];
	ID3D11ShaderResourceView* rtsrv[6];
	ID3D11SamplerState* rtSampler[6];
	ID3D11DepthStencilView* dsv;
	ID3D11Texture2D* mDepthStencilBuffer;

	D3D11_VIEWPORT vp;

	//ID3DX11Effect* effect;
	ID3D11InputLayout* inputLayouyt;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constantBuffer;

	ID3D11VertexShader* vertexShaer;
	ID3D11PixelShader* pixelShader;

	XMFLOAT4X4 gWorld;
	XMFLOAT4X4 gView;
	XMFLOAT4X4 gProjection;

	DxDevice() {
		creatDeviceFlag = NULL;
		device = NULL;
		context = NULL;
		dxgiDevice = NULL;
		dxgiAdapter = NULL;
		dxgiFactory = NULL;

		swapChain = NULL;
		rtt[0] = rtt[1] = rtt[2] = rtt[3] = rtt[4] = rtt[5] = NULL;
		rtv[0] = rtv[1] = rtv[2] = rtv[3] = rtv[4] = rtv[5] = NULL;
		rtsrv[0] = rtsrv[1] = rtsrv[2] = rtsrv[3] = rtsrv[4] = rtsrv[5] = NULL;
		rtSampler[0] = rtSampler[1] = rtSampler[2] = rtSampler[3] = rtSampler[4] = rtSampler[5] = NULL;
		dsv = NULL;

		//effect = NULL;
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

	void CreateRenderTarget(WindowsDevice &wnDev);

};



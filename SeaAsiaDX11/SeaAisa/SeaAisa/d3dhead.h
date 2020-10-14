#include<d3d11.h>
#include<xnamath.h>
#include<d3dcompiler.h>
#include<dxgi.h>
#include<d2d1.h>
#include<d3dcommon.h>
#include<D3DX11.h>
#include<D3DX11async.h>
#include<xnamath.h>
#include<d3dx11effect.h>

struct D3D11Device
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
	


	D3D11Device() {
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
	HRESULT CreateDevice() {
		HRESULT hr = NULL;
		hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, creatDeviceFlag, 0, 0,
			D3D11_SDK_VERSION, &device, &featureLevel, &context);
		return hr;
	}

	HRESULT CreateDeviceAndSwap() {
		HRESULT hr = NULL;
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		UINT numFeatureLevels = ARRAYSIZE(featureLevels);
		hr = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, creatDeviceFlag, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &sd,
			&swapChain, &device, &featureLevel, &context);
		return hr;
	}


};


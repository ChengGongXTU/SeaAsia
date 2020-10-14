#include"head.h"

bool InitD3D() {

	// create device and context
	mainDev.creatDeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
	mainDev.featureLevel = D3D_FEATURE_LEVEL_11_0;

	ZeroMemory(&mainDev.sd, sizeof(mainDev.sd));
	mainDev.sd.BufferDesc.Width = w;
	mainDev.sd.BufferDesc.Height = h;
	mainDev.sd.BufferDesc.RefreshRate.Numerator = 60;
	mainDev.sd.BufferDesc.RefreshRate.Denominator = 1;
	mainDev.sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;   //set the pixel's type
	//mainDev.sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;      // raster uses to create an image on a surface (unspecified)
	//mainDev.sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;              //how an image is stretched to fit a given monitor's resolution
	mainDev.sd.SampleDesc.Count = 1;											// not use 4x MSAA
	mainDev.sd.SampleDesc.Quality = 0;
	mainDev.sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;							//Use the surface or resource as a back buffer
	mainDev.sd.BufferCount = 1;													// use one backbuffer for double buffering
	mainDev.sd.OutputWindow = hwnd;									//NOTICE!!! get a window's handle for this swapchain
	mainDev.sd.Windowed = true;													// true for window, false for full-scene;
	//mainDev.sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                           // the contents of the back buffer are discarded after calling Present().
	//mainDev.sd.Flags = 0;

	HRESULT hr = mainDev.CreateDeviceAndSwap();

	// create render target view of back buffer texture
	mainDev.rtv = 0;
	ID3D11Texture2D* backbuffer = 0;
	mainDev.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer));  //backbuffer get data from mSwapChain
	mainDev.device->CreateRenderTargetView(backbuffer, 0, &mainDev.rtv);				// use backbufer as render target
	Rel(backbuffer);
	
	// create depth/stencil buffer and view
	// set the parameter for buffer
	D3D11_TEXTURE2D_DESC dsDesc;
	ZeroMemory(&dsDesc,sizeof(dsDesc));
	dsDesc.Width =w;
	dsDesc.Height = h;
	dsDesc.MipLevels = 1;
	dsDesc.ArraySize = 1;
	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;				// the format of element in array
	dsDesc.SampleDesc.Count = 1;								// MSAA
	dsDesc.SampleDesc.Quality = 0;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;				// this texture is bound as depth and stencil
	dsDesc.Usage = D3D11_USAGE_DEFAULT;							// determine if CPU and GPU can read or write the resource 
	dsDesc.CPUAccessFlags = 0;									// how the CPU access the resource, set with "Usage" method
	dsDesc.MiscFlags = 0;										//Identifies other, less common options for resources.
																// create buffer and view
	ID3D11Texture2D* mDepthStencilBuffer = 0;
	hr = mainDev.device->CreateTexture2D(&dsDesc, NULL, &mDepthStencilBuffer);				// get point to depth buffer
	
	D3D11_DEPTH_STENCIL_VIEW_DESC dsDSV;
	ZeroMemory(&dsDSV, sizeof(dsDSV));
	dsDSV.Format = dsDesc.Format;
	dsDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDSV.Texture2D.MipSlice = 0;
	hr = mainDev.device->CreateDepthStencilView(mDepthStencilBuffer, &dsDSV, &mainDev.dsv);  //get a view for depth buffer resource
																						// bind RTV and DSV to Output Merge Stage																						// bind render target and depth/stencil view to output merger stage
	mainDev.context->OMSetRenderTargets(1, &mainDev.rtv, mainDev.dsv);

	//set the viewport
	//D3D11_VIEWPORT vp;
	mainDev.vp.TopLeftX = 0.0f;
	mainDev.vp.TopLeftY = 0.0f;
	mainDev.vp.Width = float(w);
	mainDev.vp.Height = float(h);
	mainDev.vp.MinDepth = 0.0f;
	mainDev.vp.MaxDepth = 1.0f;
	mainDev.context->RSSetViewports(1, &mainDev.vp);

	//clear the RTV and DSV

	return true;

}


#include"SeeAisa.h"
#include"basicmanager.h"
#include"lowlevelrendermanager.h"

//--------------------------windows app setting ----------------------------------
WindowsDevice winDevice;
MSG msg;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd) {
	
	winDevice.wc.lpfnWndProc = WndProc;
	winDevice.Init(hInstance,nShowCmd);


	BasicManager basicManager;
	if (!(basicManager.StartUp(winDevice))) {
		MessageBox(NULL, L"DxDevice can't be created!", L"ERROR", MB_OK);
		return 0;
	}

	LowLevelRendermanager lowlevelrendermanager;

	if (!lowlevelrendermanager.StartUp()) {
		MessageBox(NULL, L"render can't be created!", L"ERROR", MB_OK);
		return 0;
	}

	//Init the first scene
	DxScene& scene = basicManager.sceneManager.sceneList[0];
	scene.setUp(100, 1, 1, 1);

	//set the cameara, direction light in scene
	scene.cameraList[0].Init(winDevice);

	scene.dlList[0].Dir = XMFLOAT4(0.0, 0.0, 0.5,0.f);
	scene.dlList[0].Color= XMFLOAT4(1, 1, 1,1.f);

	//load a ".obj" trianglemesh and its materials
	if (!lowlevelrendermanager.LoadUnityFromObjFile(L"yosika.obj", L"yosika.mtl", L"ey_yosika.bmp", basicManager.sceneManager.sceneList[0], basicManager)) {
		MessageBox(NULL, L"obj can't be read!", L"ERROR", MB_OK);
		return 0;
	}
	

	//beigin dx11 setting
	//frist, create vs and ps shader:
	if (!lowlevelrendermanager.shaderManager.LoadAndCreateVertexShader(L"Tutorial07.fx", "vs_4_0", basicManager.dxDevice)) {
		MessageBox(NULL, L"vertex shader can't be created!", L"ERROR", MB_OK);
		return 0;
	}

	if (!lowlevelrendermanager.shaderManager.LoadAndCreatePixelShader(L"Tutorial07.fx", "ps_4_0", basicManager.dxDevice)) {
		MessageBox(NULL, L"pixel shader can't be created!", L"ERROR", MB_OK);
		return 0;
	}
	
	//second input layout:
	if (!lowlevelrendermanager.primitiveManager.LoadUnity(basicManager.dxDevice, basicManager.objManager, scene.unityList[0])) {
		MessageBox(NULL, L"unity can't be read!", L"ERROR", MB_OK);
		return 0;
	}
	if (!lowlevelrendermanager.primitiveManager.InputVertexBuffer(basicManager.dxDevice, scene.unityList[0],lowlevelrendermanager.shaderManager)) {
		MessageBox(NULL, L"vertexbuffer can't be input!", L"ERROR", MB_OK);
		return 0;
	}	

	//third, create texture, camera and light constant
	if (!lowlevelrendermanager.lightManager.CreateBuffer(basicManager.dxDevice, scene.dlList[0])) {
		MessageBox(NULL, L"dirlight buffer can't be created!", L"ERROR", MB_OK);
		return 0;
	}


	if (!lowlevelrendermanager.cameraManager.CreateViewBuffer(basicManager.dxDevice)) {
		MessageBox(NULL, L"viewtransform can't be created!", L"ERROR", MB_OK);
		return 0;
	}
	
	if (!lowlevelrendermanager.cameraManager.CreateProjectionBuffer(basicManager.dxDevice)) {
		MessageBox(NULL, L"projectiontransform can't be created!", L"ERROR", MB_OK);
		return 0;
	}
	
	if (!lowlevelrendermanager.cameraManager.LoadCamera(basicManager.dxDevice, scene.cameraList[0])) {
		MessageBox(NULL, L"camera can't be load!", L"ERROR", MB_OK);
		return 0;
	}

	if(!basicManager.textureManager.DxSetSamplerDesc(D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_NEVER, 0, D3D11_FLOAT32_MAX,0,0))
	{
		MessageBox(NULL, L"texture sampler desc setting is failed!", L"ERROR", MB_OK);
		return 0;
	}

	if (!basicManager.textureManager.DxSetSamplerState(scene.unityList[0].samplerStateId,basicManager.dxDevice))
	{
		MessageBox(NULL, L"texture sampler setting is failed!", L"ERROR", MB_OK);
		return 0;
	}





	msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else {

			float color[4] = { 1.f,1.f,1.f,1.0 };
			basicManager.dxDevice.context->ClearRenderTargetView(basicManager.dxDevice.rtv, color);
			basicManager.dxDevice.context->ClearDepthStencilView(basicManager.dxDevice.dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

			lowlevelrendermanager.shaderManager.InputVertexShader(basicManager.dxDevice);
			lowlevelrendermanager.shaderManager.InputPixelShader(basicManager.dxDevice);

			if (!lowlevelrendermanager.lightManager.SetDirLight(basicManager.dxDevice, scene.dlList[0])) {
				MessageBox(NULL, L"dirlight buffer can't be update to contant buffer!", L"ERROR", MB_OK);
				return 0;
			}

			if (!lowlevelrendermanager.cameraManager.InputCamera(basicManager.dxDevice)) {
				MessageBox(NULL, L"camera can't be input!", L"ERROR", MB_OK);
				return 0;
			}

			if (!lowlevelrendermanager.DrawUnity(basicManager, scene.unityList[0])) {
				MessageBox(NULL, L"unity can't be draw!", L"ERROR", MB_OK);
				return 0;
			}

			basicManager.dxDevice.swapChain->Present(0, 0);
		}
	}

	return msg.wParam;

}



LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_PAINT:
		winDevice.hdc = BeginPaint(hwnd, &winDevice.ps);
		EndPaint(hwnd, &winDevice.ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}


	return 0;
}
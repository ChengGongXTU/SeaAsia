#include"SeeAisa.h"
#include"dxdevice.h"
#include"basicmanager.h"
#include"lowlevelrendermanager.h"
#include"mainwindowui.h"
#include"RayTraceManager.h"
#include"GradientPathTracerManager.h"
#include "PreIntegratManager.h"

#include "atlbase.h"
#include "atlstr.h"
//--------------------------windows app setting ----------------------------------
WindowsDevice winDevice;
MSG msg;

static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;
DxDevice *dxdev =NULL;
LowLevelRendermanager* renderMng = NULL;
BasicManager* basicMng = NULL;
WindowsDevice* wnDev = NULL;

void OutputDebugPrintf(const char *strOutputString, ...);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd) {
	
	//initialize windows
	winDevice.wc.lpfnWndProc = WndProc;
	winDevice.Init(hInstance,nShowCmd);

	//initialize basicmanager
	BasicManager basicManager;
	if (!(basicManager.StartUp(winDevice))) {
		MessageBox(NULL, L"DxDevice can't be created!", L"ERROR", MB_OK);
		return 0;
	}

	g_pSwapChain = basicManager.dxDevice.swapChain;
	g_pd3dDevice = basicManager.dxDevice.device;
	g_pd3dDeviceContext = basicManager.dxDevice.context;
	dxdev = &basicManager.dxDevice;
	g_mainRenderTargetView = basicManager.dxDevice.rtv[0];

	// initialize low-level render manager
	LowLevelRendermanager lowlevelrendermanager;
	if (!lowlevelrendermanager.StartUp(basicManager)) {
		MessageBox(NULL, L"render can't be created!", L"ERROR", MB_OK);
		return 0;
	}

	RayTraceManager RayMng;
	PathTracerManager optixPtMng;
	GradientPathTracer optixGptMng;
	PreIntergratManager preInterMng;

	basicMng = &basicManager;
	renderMng = &lowlevelrendermanager;

	//intialize and binding Imgui with dxdevice
	ImGui_ImplDX11_Init(winDevice.hwnd, basicManager.dxDevice.device, basicManager.dxDevice.context);

	//main loop

	ImVec4 clear_col = ImColor(0, 0, 0,0.0);
	bool show_test_window = true;

	float horizontalAngle = 0;
	float verticalAngle = 0;

	RenderSceneId = basicManager.sceneManager.currentSceneId;
	RenderCameraId = basicManager.sceneManager.sceneList[RenderSceneId].currentCameraId;

	ZeroMemory(&msg, sizeof(msg));
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		
		ImGui_ImplDX11_NewFrame();
		/*
		{
			static float f = 0.0f;
			ImGui::Text("Hello, world!");
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
			ImGui::ColorEdit3("clear color", (float*)&clear_col);
			ImGui::Button("Test Window");
			ImGui::Button("Another Window");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		*/

		{
			/*ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);*/     // Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
			MainWindowUI(winDevice, basicManager, lowlevelrendermanager, RayMng,
				optixPtMng, optixGptMng, preInterMng, &show_test_window);
			{	


				if (basicMng->sceneManager.sceneList[basicMng->sceneManager.currentSceneId].endCameraId >0)
				{
					
					DxCamera &cam = basicMng->sceneManager.sceneList[basicMng->sceneManager.currentSceneId].cameraList[basicMng->sceneManager.sceneList[basicMng->sceneManager.currentSceneId].currentCameraId];
					float speed = 0.05;

					if (ImGui::GetIO().KeyShift)
					{
						speed = 0.5;
					}

					if (ImGui::IsKeyDown(87))
					{
						cam.eye += cam.dir* speed;
						cam.at += cam.dir* speed;
					}
					if (ImGui::IsKeyDown(83))
					{
						cam.eye -= cam.dir* speed;
						cam.at -= cam.dir* speed;
					}
					if (ImGui::IsKeyDown(65))
					{
						cam.eye -= cam.right* speed;
						cam.at -= cam.right* speed;
					}
					if (ImGui::IsKeyDown(68))
					{
						cam.eye += cam.right * speed;
						cam.at += cam.right* speed;
					}



					if (ImGui::IsMouseDown(2))
					{

						horizontalAngle = 0.005*ImGui::GetIO().MouseDelta.x;
						verticalAngle = 0.005* ImGui::GetIO().MouseDelta.y;

						cam.eye += cam.right * horizontalAngle;
						cam.at += cam.right* horizontalAngle;
						cam.eye += cam.up* verticalAngle;
						cam.at += cam.up* verticalAngle;
					}

					if (ImGui::IsMouseDown(1))
					{	
						horizontalAngle += 0.001*ImGui::GetIO().MouseDelta.x;
						verticalAngle += 0.001* ImGui::GetIO().MouseDelta.y;
						float distance = (cam.at - cam.eye).Length();
						XMVECTOR zoom = XMVectorSet(0.0, 0.0, -distance, 1.0f);
						XMMATRIX rotation = XMMatrixRotationRollPitchYaw(verticalAngle, horizontalAngle, 0.0f);
						zoom = XMVector3Transform(zoom, rotation);

						cam.at = cam.eye - Vector(XMVectorGetX(zoom), XMVectorGetY(zoom), XMVectorGetZ(zoom));

						XMVECTOR up = XMVectorSet(0.0, 1.0, 0.0, 1.0f);
						up = XMVector3Transform(up, rotation);
						
						cam.up = Vector(XMVectorGetX(up), XMVectorGetY(up), XMVectorGetZ(up));

						XMVECTOR right = XMVectorSet(1.0, 0.0, 0.0, 1.0f);
						right = XMVector3Transform(right, rotation);

						cam.right = Normalize(Vector(XMVectorGetX(right), XMVectorGetY(right), XMVectorGetZ(right)));
						
						cam.dir = Normalize(cam.at - cam.eye);
					}
					
					cam.mView = LookAt(cam.eye, cam.at, cam.up).m;
				}

			}


		}

		
		basicManager.dxDevice.context->ClearRenderTargetView(basicManager.dxDevice.rtv[0], (float*)&clear_col);
		basicManager.dxDevice.context->ClearRenderTargetView(basicManager.dxDevice.rtv[1], (float*)&clear_col);
		basicManager.dxDevice.context->ClearRenderTargetView(basicManager.dxDevice.rtv[2], (float*)&clear_col);
		basicManager.dxDevice.context->ClearRenderTargetView(basicManager.dxDevice.rtv[3], (float*)&clear_col);
		basicManager.dxDevice.context->ClearRenderTargetView(basicManager.dxDevice.rtv[4], (float*)&clear_col);
		basicManager.dxDevice.context->ClearRenderTargetView(basicManager.dxDevice.rtv[5], (float*)&clear_col);
		lowlevelrendermanager.DeferredRenderScene(basicManager,winDevice, RenderSceneId);
		ImGui::Render();
		basicManager.dxDevice.swapChain->Present(0, 0);
	}

	ImGui_ImplDX11_Shutdown();


	return msg.wParam;

}


extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{

	if (ImGui_ImplDX11_WndProcHandler(hwnd, message, wParam, lParam))
		return true;


	switch (message)
	{

	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX11_InvalidateDeviceObjects();
			dxdev->CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			dxdev->CreateRenderTarget(winDevice);

			renderMng->ResizeRenderpipeline(*basicMng,winDevice);
			ImGui_ImplDX11_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}


	return DefWindowProc(hwnd, message, wParam, lParam);
}

void OutputDebugPrintf(const char *strOutputString, ...)
{
	char strBuffer[4096] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
	//vsprintf(strBuffer, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugString(CA2W(strBuffer));
}
#pragma once
#include"imgui.h"
#include"wcharandwstring.h"
#include"basicmanager.h"
#include"dxcamera.h"
#include"windowsDevice.h"
#include"lowlevelrendermanager.h"
#include"dxcamera.h"
#include"RayTraceManager.h"
#include"PathTracerManager.h"
#include"GradientPathTracerManager.h"


#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

static int RenderSceneId = 0;
static int RenderCameraId = 0;

static void ScenenResourceView(WindowsDevice & winDev, BasicManager &basicMng, LowLevelRendermanager &renderMng,bool *p_open);
static void ScenenListView(WindowsDevice & winDev ,BasicManager &basicMng, bool *p_open);
static void SceneSettingView(BasicManager &basicMng, bool *p_open);
static void UnityLoadingView(BasicManager &basicMng, LowLevelRendermanager &renderMng, bool *p_open);
static void CameraSettingView(BasicManager &basicMng, bool *p_open);
static void DirectionLightSettingView(BasicManager &basicMng, LowLevelRendermanager &renderMng,bool *p_open);
static void PointLightSettingView(BasicManager &basicMng, bool *p_open);
static void RenderSettingView(WindowsDevice & winDev, BasicManager &basicMng, LowLevelRendermanager &renderMng, bool *p_open);
static void CameraCangeView(BasicManager &basicMng, bool *p_open);
static void MaterialChangeView(BasicManager &basicMng, LowLevelRendermanager& renderMng,bool *p_open);
static void SceneHierarchy(bool* p_open, BasicManager &basicMng);
static void ShowTextureResource(bool* p_open, BasicManager &basicMng);
static void ShowObjectResource(bool* p_open, BasicManager &basicMng);
static void ShowMaterialResource(bool* p_open, BasicManager &basicMng);
static void ShowLightResource(bool* p_open, BasicManager &basicMng);
static void ShowUnityProperty(bool* p_open, WindowsDevice & winDev, BasicManager &basicMng,LowLevelRendermanager &renderMng, int SelectUnityID);
static void ShowRayTracer(bool* p_open, BasicManager &basicMng, LowLevelRendermanager &renderMng, GradientPathTracer& optixGptMng);

void MainWindowUI(WindowsDevice & winDev,BasicManager &basicMng, LowLevelRendermanager &renderMng, 
	RayTraceManager& RayMng, PathTracerManager& optixPtMng, GradientPathTracer& optixGptMng,bool *p_open);

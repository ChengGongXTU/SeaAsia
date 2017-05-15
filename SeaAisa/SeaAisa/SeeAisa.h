// This software can only be used in Windows system

#pragma once

//VC++ Include Files
#include<Windows.h>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<algorithm>
#include<string>
#include<string.h>
#include<time.h>
#include<random>
#include<fstream>
#include<iostream>
#include<istream>
#include<sstream>

//SDK
//STL
#include<vector>
//DX11
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
#include"windowsDevice.h"
//system support
//#include"rand.h"


//imgui
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

//Macros
#define M_PI 3.1415926
#define Rel(x) {if(x){x->Release();x=0;}}
using namespace std;
using std::swap;
using std::min;
using std::max;

// Basic module
class BasicManager;

// Forward Declarations
// Resource manager

class ObjManager;
class TextureManager;
class SceneManager;
class MaterialsManager;
class FrontManager;
class BoneManager;
class ImpactManager;
class PhysicsParameterManager;

//Software-LowLevel-RenderModule
//Basic Mesh
class Vector;
class Point;
class Normal;
//Transform
struct Matrix4x4;
class Transform;
//camera
class Camera;
//Texture
class Texture;
//shape
class Triangle;
class TriangleMseh;



//D3D-lowlevel-RenderModule
class DxDevice;
class DxObj;
class DxCamera;
class DxLight;
class DxScene;
class DxMaterials;
class Unity;

//extra math
inline float Radians(float deg) {
	return ((float)M_PI / 180.f) * deg;
}

//windows device
//HDC hdc;
//HWND hwnd;
//static MSG msg;
//PAINTSTRUCT ps;
//HBITMAP hBitmap;
//BYTE* pBits;
//BITMAPINFOHEADER bmih;
//BITMAP bitmap;
//HDC hdcmem;
//WNDCLASSEX wc;
//int w, h;
//static WindowsDevice winDevice;


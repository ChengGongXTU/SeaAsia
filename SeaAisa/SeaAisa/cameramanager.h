#pragma once
#include"SeeAisa.h"
#include"dxcamera.h"
#include"dxdevice.h"

struct ViewTransform {
	XMMATRIX mView;
	XMMATRIX mView2;
};

struct ProjectionTranaform {
	XMMATRIX mProjection;
};

class CameraManager {
public:
	D3D11_BUFFER_DESC bd;

	ViewTransform viewTransform;
	ProjectionTranaform projectionTransform;

	ID3D11Buffer* viewTransformBuffer;
	ID3D11Buffer* projTransformBuffer;

	CameraManager(){}
	~CameraManager(){}

	void StartUp();
	void ShutUp();

	bool CreateViewBuffer(DxDevice &dev);
	bool CreateProjectionBuffer(DxDevice &dev);
	bool LoadCamera(DxDevice &dev, DxCamera &camera);
	bool InputCamera(DxDevice &dev);

};

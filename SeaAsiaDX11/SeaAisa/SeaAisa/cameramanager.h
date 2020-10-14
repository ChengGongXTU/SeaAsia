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

struct WorldTransform {
	XMMATRIX mWorld;
};

class CameraManager {
public:
	D3D11_BUFFER_DESC bd;

	ViewTransform viewTransform;
	ProjectionTranaform projectionTransform;
	WorldTransform worldTransform;

	ID3D11Buffer* viewTransformBuffer;
	ID3D11Buffer* projTransformBuffer;
	ID3D11Buffer* worldjTransformBuffer;

	CameraManager(){}
	~CameraManager(){}

	void StartUp(DxDevice &dev);
	void ShutUp();

	bool CreateViewBuffer(DxDevice &dev);
	bool CreateProjectionBuffer(DxDevice &dev);
	bool CreateWorldBuffer(DxDevice & dev);
	bool LoadCamera(DxDevice &dev, DxCamera &camera);
	bool InputCamera(DxDevice &dev);

};

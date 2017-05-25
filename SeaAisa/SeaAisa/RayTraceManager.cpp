#include "RayTraceManager.h"

void RayTraceManager::SetRayTrace(int choos)
{
	if (choos == 0)
		RayTrace = new GlobalIllumination[1];
}

void RayTraceManager::SceneLoad(DxScene & dxscene, BasicManager &basicMng)
{	
	RayTrace->scenePTR = &scene;

	scene.unityNum = dxscene.endUnityId;

	DxCamera &dxcamera = dxscene.cameraList[dxscene.currentCameraId];

	scene.camera.eye = Point(XMVectorGetX(dxcamera.eye),
		XMVectorGetY(dxcamera.eye),
		XMVectorGetZ(dxcamera.eye));
	scene.camera.gaze = Point(XMVectorGetX(dxcamera.at),
		XMVectorGetY(dxcamera.at),
		XMVectorGetZ(dxcamera.at));
	scene.camera.upView = Vector(XMVectorGetX(dxcamera.up),
		XMVectorGetY(dxcamera.up),
		XMVectorGetZ(dxcamera.up));
	scene.camera.oNear = dxcamera.cNear;
	scene.camera.oFar = dxcamera.cFar;
	scene.camera.oLeft = -dxcamera.cNear*tan(dxcamera.aspect / 2.f);
	scene.camera.oRight = -scene.camera.oLeft;
	scene.camera.oBottom = scene.camera.oLeft*dxcamera.cHeight / dxcamera.cWidth;
	scene.camera.oTop = -scene.camera.oBottom;
	scene.camera.nx = dxcamera.cWidth;
	scene.camera.ny = dxcamera.cHeight;

	scene.camera.worldToEye.m = Matrix4x4(dxcamera.mView._11, dxcamera.mView._21, dxcamera.mView._31, dxcamera.mView._41,
		dxcamera.mView._12, dxcamera.mView._22, dxcamera.mView._32, dxcamera.mView._42,
		dxcamera.mView._13, dxcamera.mView._23, dxcamera.mView._33, dxcamera.mView._43,
		dxcamera.mView._14, dxcamera.mView._24, dxcamera.mView._34, dxcamera.mView._44);

	scene.unityList = new RayUnity[scene.unityNum];
	for (int i = 0; i < scene.unityNum; i++)
	{	
		scene.unityList[i].CreateDxUnity(dxscene.unityList[i]);
		scene.unityList[i].shape->LoadDxUnity(dxscene.unityList[i], basicMng);
		scene.unityList[i].shape->ViewTransform(scene.camera.worldToEye);

	}

	scene.lightNum = dxscene.endDlId + dxscene.endPlId;
	scene.lightList = new RayLight*[scene.lightNum];
	for (int i = 0; i < dxscene.endDlId; i++)
	{	
		RayDirLight* ptr;
		ptr = new RayDirLight[1];
		ptr->dir.d = Vector(dxscene.dlList[i].Dir.x, dxscene.dlList[i].Dir.y, dxscene.dlList[i].Dir.z);
		ptr->color[0] = dxscene.dlList[i].Color.x;
		ptr->color[1] = dxscene.dlList[i].Color.y;
		ptr->color[2] = dxscene.dlList[i].Color.z;
		scene.lightList[i] = ptr;		
	}

	for (int i = dxscene.endDlId; i < scene.lightNum; i++)
	{
		RayPointLight* ptr;
		ptr = new RayPointLight[1];
		ptr->location = Point(dxscene.plList[i].Pos.x, dxscene.plList[i].Pos.y, dxscene.plList[i].Pos.z);
		ptr->color[0] = dxscene.plList[i].Color.x;
		ptr->color[1] = dxscene.plList[i].Color.y;
		ptr->color[2] = dxscene.plList[i].Color.z;
	}


}

void RayTraceManager::Render(float &pace)
{


	Camera &camera = scene.camera;

	int w = 320;
	int h = 180;
	int pixelNumber = w*h;

	//create a memory to store render result:
	ColorRGB* image = new ColorRGB[pixelNumber];

	//super sample bias for ray
	int sampleNum = 40;
	int subNum = 2;

	float dx = (camera.oRight - camera.oLeft) / w;
	float dy = (camera.oTop - camera.oBottom) / h;
	float subdx = dx / subNum;
	float subdy = dy / subNum;

	float inv = 1.f / sampleNum;
	float subInv = 1.f / (2*subNum);

	//in each pixel frag
	//create sample ray
	for (int y = 0; y < h; y++)
	{	

		for (int x = 0; x < w; x++)
		{	

			
			//compute the pixel's position in image memory
			int pixelNum = (h - y - 1)*w + x;				
			ColorRGB color;

			//in subpixel
			for (int sy = 0; sy < subNum; sy++)
			{ 
				for (int sx = 0; sx < subNum; sx++)
				{

					float ex = camera.oLeft + dx*x + subdx*(sx + dis(generator));
					float ey = camera.oBottom + dy*y + subdy*(sy + dis(generator));
					//float ex = camera.oLeft + dx*x + subdx*(sx + 0.5);
					//float ey = camera.oBottom + dy*y + subdy*(sy + 0.5);
					float ez = camera.oNear;
					Point e = Point(ex, ey, ez);
					Vector d = Normalize(e - Point(0.f, 0.f, 0.f));

					//create a ray
					Ray r = Ray(e, d, 0, INFINITY, INFINITY);
					r.tError = 0.001;
					r.depth = 0;

					//sample the color sampleNum times for each subpixel
					for (int k = 0; k < sampleNum; k++)
					{
						color += RayTrace->compute(r)*inv;
					}
				}
					
			}
			image[pixelNum] += color*subInv;
			
		}
	}

	// save result in ".ppm"
	FILE* fp = NULL;
	errno_t err = fopen_s(&fp, "Ray Trace render result.ppm", "w");

	if (err) {
		delete image;
		return;
	}

	fprintf(fp, "P3\n%d %d\n%d\n", w, h, 255);

	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			int num = j*w + i;
			fprintf(fp, "%d %d %d ", (int)(min(image[num].R, 1.f) * 255), (int)(min(image[num].G, 1.f) * 255), (int)(min(image[num].B, 1.f) * 255));
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

	delete image;
}

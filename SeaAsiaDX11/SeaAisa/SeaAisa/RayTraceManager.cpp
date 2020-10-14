#include "RayTraceManager.h"

void RayTraceManager::SetRayTrace(int choos)
{
	if (choos == 0)
		RayTrace = new DirectLight[1];
}

void RayTraceManager::SceneLoad(DxScene & dxscene, BasicManager &basicMng)
{
	RayTrace->scenePTR = &scene;

	scene.unityNum = dxscene.endUnityId;

	DxCamera &dxcamera = dxscene.cameraList[dxscene.currentCameraId];

	scene.camera.eye = Point(dxcamera.eye);
	scene.camera.gaze = Point(dxcamera.at);
	scene.camera.upView = Vector(dxcamera.up);
	scene.camera.oNear = dxcamera.cNear;
	scene.camera.oFar = dxcamera.cFar;
	scene.camera.oLeft = -dxcamera.cNear*tan(dxcamera.aspect / 2.f);
	scene.camera.oRight = -scene.camera.oLeft;
	scene.camera.oBottom = scene.camera.oLeft*dxcamera.cHeight / dxcamera.cWidth;
	scene.camera.oTop = -scene.camera.oBottom;
	scene.camera.nx = dxcamera.cWidth;
	scene.camera.ny = dxcamera.cHeight;

	scene.camera.worldToEye.m = Matrix4x4(dxcamera.mView);

	scene.unityList = new RayUnity[scene.unityNum];
	scene.lightNum += dxscene.endDlId + dxscene.endPlId;
	int arealightnum = 0;
	vector<int> areaID;
	for (int i = 0; i < scene.unityNum; i++)
	{
		if (dxscene.unityList[i].type == EmissiveType)
		{
			areaID.push_back(i);
			arealightnum++;
		}
		scene.unityList[i].CreateDxUnity(dxscene.unityList[i]);
		scene.unityList[i].shape->LoadDxUnity(dxscene.unityList[i], basicMng);
		scene.unityList[i].shape->ViewTransform(scene.camera.worldToEye);

	}

	scene.lightNum += arealightnum;


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

	for (int i = dxscene.endDlId; i < dxscene.endDlId + dxscene.endPlId; i++)
	{
		RayPointLight* ptr;
		ptr = new RayPointLight[1];
		ptr->location = Point(dxscene.plList[i].Pos.x, dxscene.plList[i].Pos.y, dxscene.plList[i].Pos.z);
		ptr->color[0] = dxscene.plList[i].Color.x;
		ptr->color[1] = dxscene.plList[i].Color.y;
		ptr->color[2] = dxscene.plList[i].Color.z;
		scene.lightList[i] = ptr;
	}

	for (int i = dxscene.endDlId + dxscene.endPlId; i < scene.lightNum; i++)
	{
		AreaLight* ptr;
		ptr = new AreaLight[1];

		int j = i - dxscene.endDlId + dxscene.endPlId;
		ptr->obj = scene.unityList[areaID[j]].shape;
		ptr->mtl = new Emissive[1];
		ptr->mtl->LoadMaterial(basicMng.materialsManager.dxMaterial[dxscene.unityList[areaID[j]].MaterialsIdIndex[0]]);
		ptr->obj->samplePTR = new Jittere(totalSampleNum);
		ptr->obj->samplePTR->SetupShuffledIndices();
		ptr->obj->samplePTR->MapSquareToHemisphere(1);
		scene.lightList[i] = ptr;

	}
	areaID.clear();
	vector<int>().swap(areaID);

	RayTrace->info.scene = &scene;
}


void RayTraceManager::Render(float &pace)
{


	Camera &camera = scene.camera;

	int w = 640;
	int h = 360;
	int pixelNumber = w * h;

	//create a memory to store render result:
	ColorRGB* image = new ColorRGB[pixelNumber];

	//super sample bias for ray
	int sampleNum = totalSampleNum;;
	SetSceneSample(sampleNum);
	int& subNum = scene.sampleNum;

	float dx = (camera.oRight - camera.oLeft) / w;
	float dy = (camera.oTop - camera.oBottom) / h;
	float subdx = dx / subNum;
	float subdy = dy / subNum;

	float inv = 1.f / sampleNum;

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
			for (int k = 0; k < sampleNum; k++)
			{
				Point p = scene.samplePTR->GetSquareSample();
				float ex = camera.oLeft + dx * x + dx * p.x;
				float ey = camera.oBottom + dy * y + dy * p.y;
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

				color += RayTrace->compute(r)*inv;
			}

			image[pixelNum] += color;

		}
		cout << y << endl;
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
			int num = j * w + i;
			fprintf(fp, "%d %d %d ", (int)(min(image[num].R, 1.f) * 255), (int)(min(image[num].G, 1.f) * 255), (int)(min(image[num].B, 1.f) * 255));
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

	delete image;
}

void RayTraceManager::SetSceneSample(int n)
{
	scene.SetSamples(n);
	scene.samplePTR->SetupShuffledIndices();
}

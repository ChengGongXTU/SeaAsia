#include "RayTraceAlgorithm.h"

ColorRGB GlobalIllumination::compute(Ray & ray)
{	
	info.Init();

	Ray ray2 = ray;

	//intersection testing
	for (int i = 0; i < scenePTR->unityNum; i++)
	{
		if (!scenePTR->unityList[i].shape->Intersection(ray2, &ray2.t, &ray2.tError, info))
			continue;
	}

	// if no intersecting
	if (info.faceID == -1)	return  ColorRGB(scenePTR->backColor[0], scenePTR->backColor[1], scenePTR->backColor[2]);

	//if beyond max trace time;
	else
	{
		info.traceTime++;

		if (info.traceTime > 5)
		{
			ColorRGB Ke = ColorRGB(info.mtl.parameter.Ke.x, info.mtl.parameter.Ke.y, info.mtl.parameter.Ke.z);

			//for (int j = 0; j < scenePTR->lightNum; j++)
			//{
			//	Vector v = scenePTR->lightList[j]->GetDirection(info.p);
			//	float cosA = Dot(info.n, v);
			//	if (cosA < 0)	cosA = 0.f;
			//	
			//	color = color + Kd*cosA;

			//}

			info.traceTime = 0;

			return Ke;
		}
		else
		{

			ColorRGB Ke = ColorRGB(info.mtl.parameter.Ke.x, info.mtl.parameter.Ke.y, info.mtl.parameter.Ke.z);
			ColorRGB Kd = ColorRGB(info.mtl.parameter.Kd.x, info.mtl.parameter.Kd.y, info.mtl.parameter.Kd.z);


			//create a new ray by random parameter;
			Vector w = Vector(info.n.x, info.n.y, info.n.z);
			Vector u = info.tangent;
			Vector v = Normalize(Cross(w, u));

			double r1 = 2 * M_PI*dis(generator);
			double r2 = dis(generator);
			float r2a = sqrtf(r2);
			float r2b = sqrtf(1 - r2);

			Vector d = Normalize(cos(r1)*r2a*u + sin(r1)*r2a*v + r2b*w);

			Ray rt = Ray(info.p + d*ray2.tError,d, ray2.tError, INFINITY, INFINITY);
			rt.tError = ray2.tError;


			return Ke+compute(rt)*Kd;
		}
	}
}

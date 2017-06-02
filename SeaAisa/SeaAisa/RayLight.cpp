#include "RayLight.h"

float RayPointLight::GetDistance(Info & info)
{
	return (location-info.p).Length();
}

Vector RayPointLight::GetDirection(Info &info)
{	
	Vector v = Normalize(location - info.p);

	return v;
}

ColorRGB RayPointLight::GetColor(Info & info)
{	
	return ColorRGB(color[0], color[1], color[2]);
}

float RayDirLight::GetDistance(Info & info)
{
	return INFINITY;
}

Vector RayDirLight::GetDirection(Info &info)
{
	Vector v = Normalize(dir.d);

	return v;
}

ColorRGB RayDirLight::GetColor(Info & info)
{
	return ColorRGB(color[0], color[1], color[2]);
}

Vector AreaLight::GetDirection(Info &info)
{
	Point sp = obj->GetSample(info.p);
	Vector n2 = obj->GetNormal(info.p);
	Vector wi2 = Normalize(sp-info.p);
	n = n2;
	wi = wi2;
	p = sp;
	return -wi2;

}

float AreaLight::GetDistance(Info & info)
{
	return (p-info.p).Length();
}

ColorRGB AreaLight::GetColor(Info & info)
{
	return mtl->GetKe();
}

//bool AreaLight::ShadowText(Ray & ray, Info & info)
//{
//	for (int i = 0; i < info.scene->unityNum; i++)
//	{
//		if (info.scene->unityList[i].shape->Intersection(ray, &ray.t, &ray.tError, info))
//			return true;
//	}
//
//	return false;
//}

float AreaLight::G(Info & info)
{
	float cosA = Dot(wi, -n);
	float dis = (p - info.p).LengthSquared();

	return (cosA / dis);
}

float AreaLight::Pdf(Info & info)
{
	return (obj->Pdf(info));
}

bool RayLight::ShadowText(Ray & ray, Info & info)
{
		for (int i = 0; i < info.scene->unityNum; i++)
		{
			if (info.scene->unityList[i].shape->Intersection(ray, &ray.t, &ray.tError,info))
				return true;
		}
	
		return false;
}

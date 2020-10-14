#include "RayMaterial.h"

void Matte::LoadMaterial(DxMaterials & mtl)
{
	diffuse_brdf->Kd = ColorRGB(mtl.parameter._ColorFactor.x, mtl.parameter._ColorFactor.y, mtl.parameter._ColorFactor.z)  * (1 - mtl.parameter._MetallicFactor);
	diffuse_brdf->SetSampler(new Jittere(4), 1);
}

ColorRGB Matte::Shader(Info & info)
{
	Vector wo = -info.ray.d;
	ColorRGB color;

	for (int i = 0; i < info.scene->lightNum; i++)
	{
		Vector wi = info.scene->lightList[i]->GetDirection(info);
		float cosA = Dot(wi, info.n);
		if (cosA > 0.f)
			color += diffuse_brdf->Samplelight(info, wi, wo)*info.scene->lightList[i]->GetColor(info)*cosA;
	}
	return color;
}

ColorRGB Matte::AreaLightShader(Info & info)
{
	Vector wo = -info.ray.d;
	ColorRGB color;
	for(int i = 0; i < info.scene->lightNum; i++)
	{
		Vector wi = -info.scene->lightList[i]->GetDirection(info);
		float tmax = info.scene->lightList[i]->GetDistance(info);
		float cosA = Dot(wi, info.n);

		if (cosA > 0.f)
		{
			Ray shadowRay = Ray(info.p, wi,0.001, tmax - 0.001, tmax-0.001);
			shadowRay.tError = 0.001;
			Info infoShadow = info;
			bool shadow = info.scene->lightList[i]->ShadowText(shadowRay, infoShadow);

			if(!shadow)
				color += diffuse_brdf->Samplelight(info, wi, wo)*info.scene->lightList[i]->GetColor(info)
					*info.scene->lightList[i]->G(info)*cosA*(1.f/ info.scene->lightList[i]->Pdf(info));
		}
	}

	return color;
	
}

void Phong::LoadMaterial(DxMaterials & mtl)
{
	diffuse_brdf->Kd = ColorRGB(mtl.parameter._ColorFactor.x, mtl.parameter._ColorFactor.y, mtl.parameter._ColorFactor.z) * (1 - mtl.parameter._MetallicFactor);
	specular_brdf->Ks = diffuse_brdf->Kd * mtl.parameter._MetallicFactor;// ColorRGB(mtl.parameter.Ks.x, mtl.parameter.Ks.y, mtl.parameter.Ks.z);
	diffuse_brdf->SetSampler(new Jittere(4), 1);
	specular_brdf->SetSampler(new Jittere(4), 256);
	
}

ColorRGB Phong::Shader(Info & info)
{
	Vector wo = -info.ray.d;
	ColorRGB color;
	
	for (int i = 0; i, info.scene->lightNum; i++)
	{
		Vector wi = info.scene->lightList[i]->GetDirection(info);
		float cosA = Dot(wi, info.n);

		if (cosA > 0.f)
			color += (diffuse_brdf->Samplelight(info, wi, wo)
				+ specular_brdf->Samplelight(info, wi, wo))*info.scene->lightList[i]->GetColor(info)*cosA;
	}
	return color;
}

ColorRGB Phong::AreaLightShader(Info & info)
{
	Vector wo = -info.ray.d;
	ColorRGB color;
	for (int i = 0; i < info.scene->lightNum; i++)
	{
		Vector wi = -info.scene->lightList[i]->GetDirection(info);
		float tmax = info.scene->lightList[i]->GetDistance(info);
		float cosA = Dot(wi, info.n);

		if (cosA > 0.f)
		{
			Ray shadowRay = Ray(info.p, wi, 0.001, tmax-0.001, tmax-0.001);

			bool shadow = info.scene->lightList[i]->ShadowText(shadowRay, info);

			if(!shadow)
				color += (diffuse_brdf->Samplelight(info, wi, wo)+specular_brdf->Samplelight(info, wi, wo))*info.scene->lightList[i]->GetColor(info)
					*info.scene->lightList[i]->G(info)*cosA*(1.f / info.scene->lightList[i]->Pdf(info));
		}
	}

	return color;
}

void Emissive::LoadMaterial(DxMaterials & mtl)
{
	Ke = ColorRGB(1, 1, 1);
	
}

ColorRGB Emissive::AreaLightShader(Info & info)
{	
	if (Dot(-info.n, info.ray.d) > 0.f)
		return Ke;
	else
		return ColorRGB(0.f,0.f,0.f);
}


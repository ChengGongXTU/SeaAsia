#pragma once
#include"SeeAisa.h"
#include"information.h"
#include"BRDF.h"
#include"RayLight.h"
#include"RayScene.h"

class RayMaterial
{
public:

	virtual ColorRGB GetKe() = 0;
	virtual ColorRGB Shader(Info &info) = 0;
	virtual ColorRGB AreaLightShader(Info &info) = 0;
	virtual void LoadMaterial(DxMaterials &mtl) = 0;
};


class Matte:public RayMaterial
{
public:
	Lambertian* diffuse_brdf;
	
	
	Matte()
	{
		diffuse_brdf = new Lambertian[1];
	}

	void LoadMaterial(DxMaterials &mtl);

	ColorRGB GetKe() { return ColorRGB(0, 0, 0); }
	ColorRGB Shader(Info &info);
	ColorRGB AreaLightShader(Info &info);
};

class Phong :public RayMaterial
{
public:
	Lambertian* diffuse_brdf;
	GlossySpecular* specular_brdf;

	Phong()
		:RayMaterial(), diffuse_brdf(new Lambertian), specular_brdf(new GlossySpecular)
	{}

	void LoadMaterial(DxMaterials &mtl);

	ColorRGB GetKe() { return ColorRGB(0, 0, 0); }
	ColorRGB Shader(Info &info);
	ColorRGB AreaLightShader(Info &info);

};


class Emissive :public RayMaterial
{
public:
	ColorRGB Ke;

	Emissive()
		:RayMaterial()
	{}

	ColorRGB GetKe() { return Ke; }
	void LoadMaterial(DxMaterials &mtl);
	ColorRGB Shader(Info &info) { return info.backColor; };
	ColorRGB AreaLightShader(Info &info);
};
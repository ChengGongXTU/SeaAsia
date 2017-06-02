#pragma once
#include"SeeAisa.h"
#include"sample.h"
#include"information.h"
#include"geometry.h"
#include"RGB.h"

class BRDF
{
public:
	Sampler* samplePTR;

	virtual ColorRGB Sample(Info & info, Vector &wi, Vector &wo, float& Pdf) = 0;
	virtual ColorRGB Samplelight(Info & info, Vector &wi, Vector &wo) = 0;
	virtual void SetSampler(Sampler *sp, float e);

};


class Lambertian : public BRDF
{
public:
	ColorRGB Kd;

	Lambertian() { Kd = ColorRGB(0, 0, 0); samplePTR = NULL; }

	ColorRGB Sample(Info & info, Vector &wi, Vector &wo, float& Pdf);
	ColorRGB Samplelight(Info & info, Vector &wi, Vector &wo);
};

class Specular : public BRDF
{
public:
	ColorRGB Ks;

	Specular() { Ks = ColorRGB(0, 0, 0); samplePTR = NULL; }
	ColorRGB Sample(Info & info, Vector &wi, Vector &wo, float& Pdf);
	ColorRGB Samplelight(Info & info, Vector &wi, Vector &wo) { return ColorRGB(0.f, 0.f, 0.f); };
};

class GlossySpecular : public BRDF

{public:
	ColorRGB Ks;
	float exp;

	GlossySpecular() { Ks = ColorRGB(0, 0, 0); samplePTR = NULL; }

	ColorRGB Sample(Info & info, Vector &wi, Vector &wo, float& Pdf);
	ColorRGB Samplelight(Info & info, Vector &wi, Vector &wo);
};
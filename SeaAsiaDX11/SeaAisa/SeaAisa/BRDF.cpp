#include "BRDF.h"



ColorRGB Lambertian::Sample(Info & info, Vector & wi, Vector & wo,float &Pdf)
{	

	Vector w = Vector(info.n.x,info.n.y,info.n.z);
	Vector v = Vector(w.x*0.0034, w.y*1.0, w.z*0.0071);
	v = Normalize(v);
	Vector u = Cross(w, v);

	Point sampleP = samplePTR->GetHmieSphereSample();
	wi = sampleP.x*u + sampleP.y*v + sampleP.z*w;
	wi = Normalize(wi);

	Pdf = Dot(info.n, wi)*Inv_M_PI;

	return ColorRGB(Kd*Inv_M_PI);
}

ColorRGB Lambertian::Samplelight(Info & info, Vector & wi, Vector & wo)
{
	return Kd*Inv_M_PI;
}



ColorRGB Specular::Sample(Info & info, Vector & wi, Vector & wo, float & Pdf)
{	
	float cosA = Dot(info.n, wo);
	wi = -wo + 2.0*Vector(info.n.x,info.n.y,info.n.z)*cosA;


	return (Ks* (1.f / Dot(info.n, wi)));
}

ColorRGB GlossySpecular::Sample(Info & info, Vector & wi, Vector & wo, float & Pdf)
{
	float cosA = Dot(info.n, wo);
	Vector refVec = -wo + 2 * Vector(info.n.x, info.n.y, info.n.z)*cosA;

	Vector w = refVec;
	Vector u = Vector(w.x*0.00424, w.y*1.0, w.z*0.00764);
	u = Normalize(u);
	Vector v = Cross(w, u);

	Point sampleP = samplePTR->GetHmieSphereSample();
	wi = sampleP.x*u + sampleP.y*v + sampleP.z*w;

	if (Dot(wi, info.n) < 0.f)
		wi = -sampleP.x*u - sampleP.y*v + sampleP.z*w;

	float lobe = pow(Dot(refVec, wi), exp);
	Pdf = lobe*Dot(info.n, wi);

	return (Ks*lobe);
}

ColorRGB GlossySpecular::Samplelight(Info & info, Vector & wi, Vector & wo)
{
	ColorRGB color;

	float cosA = Dot(info.n, wi);

	Vector r = -wi + 2.0*Vector(info.n.x, info.n.y, info.n.z)*cosA;

	float cosB = Dot(r, wo);

	if (cosB > 0.f)
		color = Ks*pow(cosB, exp);

	return color;
}

void BRDF::SetSampler(Sampler * sp, float e)
{
	if (samplePTR != NULL)
	{
		delete samplePTR;
		samplePTR = NULL;
	}

	samplePTR = sp;
	samplePTR->SetupShuffledIndices();
	samplePTR->MapSquareToHemisphere(e);
}

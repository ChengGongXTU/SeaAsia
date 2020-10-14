///write depending on PBRT_V3: reflection.cpp
///following gnu rule

#pragma once

#include <optixu/optixu_math_namespace.h>

static
__host__ __device__ __inline__ optix::float3 LocaltoWorld(
	optix::float3 localVector, optix::Onb onb)
{	
	optix::float3 worldVector = localVector;
	onb.inverse_transform(worldVector);
	return worldVector;
}

static
__host__ __device__ __inline__ optix::float3 WorldtoLocal(
	optix::float3 WorldVector, optix::Onb onb)
{
	float x = optix::dot(WorldVector, onb.m_tangent);
	float y = optix::dot(WorldVector, onb.m_binormal);
	float z = optix::dot(WorldVector, onb.m_normal);
	return optix::make_float3(x, y, z);
}

static
__host__ __device__ __inline__ float SchlickWeight(float cosTheta)
{
	float m = optix::clamp(1.f - cosTheta, 0.f, 1.f);
	float m2 = m * m;
	return m2 * m2 *m;
}

static
__host__ __device__ __inline__ bool SameHemisphere(optix::float3 w, optix::float3 wp) {
	return w.z * wp.z > 0;
}

static
__host__ __device__ __inline__ float CosTheta(optix::float3 w) { return w.z; }
static
__host__ __device__ __inline__ float Cos2Theta(optix::float3 w) { return w.z * w.z; }
static
__host__ __device__ __inline__ float AbsCosTheta(optix::float3 w) { return fabs(w.z); }
static
__host__ __device__ __inline__ float Sin2Theta(optix::float3 w)
{
	return optix::fmaxf(0.f, 1.f - Cos2Theta(w));
}
static
__host__ __device__ __inline__ float SinTheta(optix::float3 w) { return sqrtf(Sin2Theta(w)); }
static
__host__ __device__ __inline__ float TanTheta(optix::float3 w) { return SinTheta(w) / CosTheta(w); }
static
__host__ __device__ __inline__ float Tan2Theta(optix::float3 w)
{
	return Sin2Theta(w) / Cos2Theta(w);
}
static
__host__ __device__ __inline__ float CosPhi(optix::float3 w) {
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 1 : optix::clamp(w.x / sinTheta, -1.f, 1.f);
}

static
__host__ __device__ __inline__ float SinPhi(optix::float3 w) {
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 0 : optix::clamp(w.y / sinTheta, -1.f, 1.f);
}
static
__host__ __device__ __inline__ float Cos2Phi(optix::float3 w) { return CosPhi(w) * CosPhi(w); }

static
__host__ __device__ __inline__ float Sin2Phi(optix::float3 w) { return SinPhi(w) * SinPhi(w); }

static
__host__ __device__ __inline__ float3 Refract(
	optix::float3 wo, optix::float3 n, float eta)
{
	if (eta == 1.f)	return -wo;

	float cosThetaI = optix::dot(wo, n);
	if (cosThetaI > 0)	eta = 1 / eta;

	float cosThetaTSqr = 1 - (1 - cosThetaI * cosThetaI) * (eta*eta);

	float sign = cosThetaI > 0 ? 1 : -1;
	
	//return n * (cosThetaI * eta - sign * sqrtf(cosThetaTSqr)) - wo * eta;

	if (cosThetaI < 0) n *= -1;
	return n * (fabs(cosThetaI) * eta - sqrtf(cosThetaTSqr)) - wo * eta;
}

static
__host__ __device__ __inline__ bool Refract(
	optix::float3 wi, optix::float3 n, float eta, optix::float3 &wt) 
{
	float cosThetaI = optix::dot(n, wi);
	float sin2ThetaI = fmaxf(0.0, 1.0 - cosThetaI * cosThetaI);
	float sin2ThetaT = eta * eta * sin2ThetaI;

	if (sin2ThetaT >= 1) return false;
	float cosThetaT = sqrtf(1 - sin2ThetaT);
	wt = eta * -wi + (eta * cosThetaI - cosThetaT) * n;

	return true;
}

static
__host__ __device__ __inline__ void specular_sample_hemisphere(
	const float z1, const float z2, optix::float3 &p, float roughness)
{
	float a = fmaxf(0.001f, roughness);
	float phi = z1 * 2.0f * 3.1415926535898;
	//float cosTheta = (1.f - z2) + z2 * a;
	float cosTheta = sqrtf((1.0f - z2) / (1.0f + (a*a - 1.0f)*z2));
	float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
	float sinPhi = sinf(phi);
	float cosPhi = cosf(phi);

	p.x = sinTheta * cosPhi;
	p.y = sinTheta * sinPhi;
	p.z = cosTheta;
}

static
__host__ __device__ __inline__ optix::float3 SphericalDirection(
	float sinTheta, float cosTheta, float phi) {
	return optix::make_float3(sinTheta * cosf(phi), sinTheta * sinf(phi), cosTheta);
}

static
__host__ __device__ __inline__  float GTR1(float cosTheta, float alpha) {
	float alpha2 = alpha * alpha;
	return (alpha2 - 1) /
		(M_PIf * logf(alpha2) * (1 + (alpha2 - 1) * cosTheta * cosTheta));
}

static
__host__ __device__ __inline__ float GTR2(float cosTheta, float a)
{
	float a2 = a * a;
	float t = 1.0f + (a2 - 1.0f)*cosTheta*cosTheta;
	return a2 / (3.1415926535898 * t*t);
}

static
__host__ __device__ __inline__ float SmithGGX(
	float cosTheta, float alphaG)
{
	float a = alphaG * alphaG;
	float b = cosTheta * cosTheta;
	return 1.0f / (cosTheta + sqrtf(a + b - a * b));
}

static
__host__ __device__ __inline__ optix::float3 SmithGGXF(
	optix::float3 wo, optix::float3 wi, float rough, optix::float3 R0
)
{	
	optix::float3 n = optix::make_float3(0, 0, 1);
	float NDotL = optix::dot(n, wi);
	float NDotV = optix::dot(n, wo);
	if (NDotL <= 0.0f || NDotV <= 0.0f) return optix::make_float3(0.0f);
	optix::float3 wh = optix::normalize(wo + wi);
	float NDotH = fabs(optix::dot(n, wh));
	float LDotH = optix::dot(wi, wh);

	float a = fmaxf(0.001f, rough);
	float Ds = GTR2(NDotH, a);
	float FH = SchlickWeight(LDotH);
	optix::float3 Fs = optix::lerp(R0, optix::make_float3(1.0f, 1.0f, 1.0f), FH);
	float roughg = sqrtf(rough*0.5f + 0.5f);
	float Gs = SmithGGX(NDotL, roughg) * SmithGGX(NDotV, roughg);

	return  Gs * Fs * Ds;
}

static
__host__ __device__ __inline__ float SmithGGXPdf(
	optix::float3 wo, optix::float3 wi, float rough
)
{	
	float specularAlpha = fmaxf(0.001f, rough);
	optix::float3 n = optix::make_float3(0, 0, 1);
	optix::float3 wh = optix::normalize(wi + wo);
	float cosTheta = fabs(optix::dot(wh, n));
	float pdfGTR2 = GTR2(cosTheta, specularAlpha) * cosTheta;
	float pdfSpec = pdfGTR2 / (4.0 * fabs(optix::dot(wi, wh)));
	return pdfSpec;
}


static
__host__ __device__ __inline__ float TrowbridgeReitzD(optix::float3 wh, float alphax, float alphay)
{
	float tan2Theta = Tan2Theta(wh);
	if (isnan(tan2Theta)) return 0.f;
	float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);

	//test
	float ax2 = fmaxf(0.001, alphax * alphax);
	float ay2 = fmaxf(0.001, alphay * alphay);
	float axy = fmaxf(0.001, alphax * alphay);

	float e =
		(Cos2Phi(wh) / ax2 + Sin2Phi(wh) / ay2) *
		tan2Theta;
	return 1 / (M_PIf * axy * cos4Theta * (1 + e) * (1 + e));
}
static
__host__ __device__ __inline__ float TrowbridgeReitzDistributionLambda(optix::float3 w, float alphax, float alphay)
{
	float absTanTheta = fabs(TanTheta(w));
	if (isnan(absTanTheta)) return 0.f;

	//test
	float ax2 = fmaxf(0.001, alphax * alphax);
	float ay2 = fmaxf(0.001, alphay * alphay);

	float alpha =
		sqrtf(Cos2Phi(w) * ax2 + Sin2Phi(w) * ay2);
	float alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
	return (-1.f + sqrtf(1.f + alpha2Tan2Theta)) / 2.f;
}

static
__host__ __device__ __inline__ float TrowbridgeReitzDistributionG1(optix::float3 w, float alphax, float alphay)
{
	return 1.f / (1.f + TrowbridgeReitzDistributionLambda(w, alphax, alphay));
}

static
__host__ __device__ __inline__ float BxdfPdf(optix::float3 wo, optix::float3 wi)
{
	bool sameHemisphere = SameHemisphere(wo, wi);
	float pdf = sameHemisphere ? AbsCosTheta(wi) * M_1_PIf : 0;
	return pdf;
}

static
__host__ __device__ __inline__ float DisneyDiffusePdf(optix::float3 wo, optix::float3 wi)
{
	return BxdfPdf(wo, wi);
}

static
__host__ __device__ __inline__ float DisneyRetroPdf(optix::float3 wo, optix::float3 wi)
{
	return BxdfPdf(wo, wi);
}

static
__host__ __device__ __inline__ float TrowbridgeReitzDistributionPdf(
	optix::float3 wo, optix::float3 wh, float alphax, float alphay)
{
	//return TrowbridgeReitzD(wh, alphax, alphay)
	//	* TrowbridgeReitzDistributionG1(wo, alphax, alphay)
	//	* fabs(optix::dot(wo, wh)) / AbsCosTheta(wo);

	return TrowbridgeReitzD(wh, alphax, alphay)	* AbsCosTheta(wh);

}

static
__host__ __device__ __inline__ float DisneyMicrofacetDistributionPdf(optix::float3 wo, optix::float3 wh, float alphax, float alphay)
{
	return TrowbridgeReitzDistributionPdf(wo, wh, alphax, alphay);
}

static
__host__ __device__ __inline__ float DisneyMicrofacetDistributionG(
	optix::float3 wo, optix::float3 wi, float alphax, float alphay
)
{
	return TrowbridgeReitzDistributionG1(wo, alphax, alphay)
		* TrowbridgeReitzDistributionG1(wi, alphax, alphay);
}


static
__host__ __device__ __inline__ float DisneyMicrofacetReflectionPdf(optix::float3 wo, optix::float3 wi, float alphax, float alphay)
{
	if (!SameHemisphere(wo, wi)) return 0;
	optix::float3 wh = optix::normalize(wo + wi);
	return DisneyMicrofacetDistributionPdf(wo, wh, alphax, alphay) / (4 * optix::dot(wo, wh));
}

static
__host__ __device__ __inline__ optix::float3 DisneyDiffuseF(optix::float3 wo, optix::float3 wi, optix::float3 R)
{
	float fo = SchlickWeight(AbsCosTheta(wo));
	float fi = SchlickWeight(AbsCosTheta(wi));
	return R * M_1_PIf * (1.f - fo / 2.f) * (1.f - fi / 2.f);
}

static
__host__ __device__ __inline__ optix::float3 DisneyRetroF(optix::float3 wo, optix::float3 wi, optix::float3 R, float roughness)
{
	optix::float3 wh = wi + wo;
	if (wh.x == 0 && wh.y == 0 && wh.z == 0) return optix::make_float3(0.f);
	wh = optix::normalize(wh);
	float cosThetaD = optix::dot(wi, wh);

	float fo = SchlickWeight(AbsCosTheta(wo));
	float fi = SchlickWeight(AbsCosTheta(wi));
	float Rr = 2 * roughness * cosThetaD * cosThetaD;

	return R * M_1_PIf * Rr * (fo + fi + fo * fi * (Rr - 1));
}

static
__host__ __device__ __inline__ optix::float3 DisneyBaseDiffuseF(optix::float3 wo, optix::float3 wi, optix::float3 R, float roughness)
{
	optix::float3 wh = wi + wo;
	if (wh.x == 0 && wh.y == 0 && wh.z == 0) return optix::make_float3(0.f);
	wh = optix::normalize(wh);
	float cosThetaD = optix::dot(wi, wh);

	float fo = SchlickWeight(AbsCosTheta(wo));
	float fi = SchlickWeight(AbsCosTheta(wi));
	float Rr = 0.5 +  2 * roughness * cosThetaD * cosThetaD;

	return R * M_1_PIf *
		(1 + (Rr - 1) * fo) *
		(1 + (Rr - 1) * fi);
}

static 
__host__ __device__ __inline__ float FrDielectric(
	float cosThetaI, float etaI, float etaT)
{
	cosThetaI = optix::clamp(cosThetaI, -1.f, 1.f);
	// Potentially swap indices of refraction
	bool entering = cosThetaI > 0.f;
	if (!entering) {
		float temp = etaI;
		etaI= etaT;
		etaT = temp;
		cosThetaI = fabs(cosThetaI);
	}

	// Compute _cosThetaT_ using Snell's law
	float sinThetaI = sqrtf(optix::fmaxf((float)0, 1 - cosThetaI * cosThetaI));
	float sinThetaT = etaI / etaT * sinThetaI;

	// Handle total internal reflection
	if (sinThetaT >= 1) return 1;
	float cosThetaT = sqrtf(optix::fmaxf((float)0, 1 - sinThetaT * sinThetaT));
	float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
		((etaT * cosThetaI) + (etaI * cosThetaT));
	float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
		((etaI * cosThetaI) + (etaT * cosThetaT));
	return (Rparl * Rparl + Rperp * Rperp) / 2;
}

static
__host__ __device__ __inline__ optix::float3 FrSchlick(
	const optix::float3 R0, float cosTheta)
{
	return optix::lerp(R0, optix::make_float3(1.f), SchlickWeight(cosTheta));
}

static
__host__ __device__ __inline__ optix::float3 DisneyFresnelEvaluate(
	float cosI, optix::float3 R0, float metallic, float eta)
{
	return optix::lerp(optix::make_float3(FrDielectric(cosI, 1, eta)),
		FrSchlick(R0, cosI), metallic);
}

static
__host__ __device__ __inline__ optix::float3 FresnelDielectricEvaluate(
	float etaI, float etaT, float cosThetaI)
{
	float f = FrDielectric(cosThetaI, etaI, etaT);
	return optix::make_float3(f);
}

static
__host__ __device__ __inline__ optix::float3 faceforward(
	optix::float3 v, optix::float3 v2
)
{
	return (optix::dot(v, v2) < 0.f) ? -v : v;
}

/// Refraction in local coordinates
static
__host__ __device__ __inline__ optix::float3 refract(optix::float3 &wi, float cosThetaT, float eta) {
	float scale = -(cosThetaT < 0 ? 1.0 / eta : eta);
	return optix::make_float3(scale*wi.x, scale*wi.y, cosThetaT);
}

static
__host__ __device__ __inline__ optix::float3 DisneyMicrofacetReflectionF(
	optix::float3 wo, optix::float3 wi, 
	optix::float3 R, optix::float3 R0,
	float alphax, float alphay,
	float metallic, float eta)
{
	float cosThetaO = AbsCosTheta(wo), cosThetaI = AbsCosTheta(wi);
	optix::float3 wh = wi + wo;
	if (cosThetaI == 0 || cosThetaO == 0) return optix::make_float3(0.f);
	if (wh.x == 0 && wh.y == 0 && wh.z == 0) return optix::make_float3(0.f);
	wh = optix::normalize(wh);
	//Spectrum F = fresnel->Evaluate(Dot(wi, Faceforward(wh, Vector3f(0, 0, 1))));
	optix::float3 nor = optix::make_float3(0.f, 0.f, 1.f);
	float cosI = optix::dot(wi, faceforward(wh, nor));
	optix::float3 F = DisneyFresnelEvaluate(cosI, R0, metallic, eta);
	
	return R * TrowbridgeReitzD(wh, alphax, alphay) 
		* DisneyMicrofacetDistributionG(wo, wi, alphax, alphay) * F /
		(4 * cosThetaI * cosThetaO);
}

static
__host__ __device__ __inline__ optix::float3 DisneyDiffuseSampleF(
	optix::float3 wo, optix::float3 &wi,
	optix::float3 R, float &pdf, optix::float2 randomU)
{
	optix::cosine_sample_hemisphere(randomU.x, randomU.y, wi);
	if (wo.z < 0) wi.z *= -1.f;
	pdf = DisneyDiffusePdf(wo, wi);
	return DisneyDiffuseF(wo, wi, R);
}

static
__host__ __device__ __inline__ optix::float3 DisneyRetroSampleF(
	optix::float3 wo, optix::float3 &wi,
	optix::float3 R, float roughness, float &pdf, optix::float2 randomU)
{
	optix::cosine_sample_hemisphere(randomU.x, randomU.y, wi);
	if (wo.z < 0) wi.z *= -1.f;
	pdf = DisneyRetroPdf(wo, wi);
	return DisneyRetroF(wo, wi, R, roughness);
}

static void TrowbridgeReitzSample11(float cosTheta, float U1, float U2,
	float &slope_x, float &slope_y) {
	// special case (normal incidence)
	if (cosTheta > .9999) {
		float r = sqrtf(U1 / (1 - U1));
		float phi = 6.28318530718 * U2;
		slope_x = r * cosf(phi);
		slope_y = r * sinf(phi);
		return;
	}

	float sinTheta =
		sqrtf(optix::fmaxf((float)0, (float)1 - cosTheta * cosTheta));
	float tanTheta = sinTheta / cosTheta;
	float a = 1 / tanTheta;
	float G1 = 2 / (1 + sqrtf(1.f + 1.f / (a * a)));

	// sample slope_x
	float A = 2 * U1 / G1 - 1;
	float tmp = 1.f / (A * A - 1.f);
	if (tmp > 1e10) tmp = 1e10;
	float B = tanTheta;
	float D = sqrtf(
		optix::fmaxf(float(B * B * tmp * tmp - (A * A - B * B) * tmp), float(0)));
	float slope_x_1 = B * tmp - D;
	float slope_x_2 = B * tmp + D;
	slope_x = (A < 0 || slope_x_2 > 1.f / tanTheta) ? slope_x_1 : slope_x_2;

	// sample slope_y
	float S;
	if (U2 > 0.5f) {
		S = 1.f;
		U2 = 2.f * (U2 - .5f);
	}
	else {
		S = -1.f;
		U2 = 2.f * (.5f - U2);
	}
	float z =
		(U2 * (U2 * (U2 * 0.27385f - 0.73369f) + 0.46341f)) /
		(U2 * (U2 * (U2 * 0.093073f + 0.309420f) - 1.000000f) + 0.597999f);
	slope_y = S * z * sqrtf(1.f + slope_x * slope_x);
}

static
__host__ __device__ __inline__ optix::float3 TrowbridgeReitzSample(
	optix::float3 wi, float alphax, float alphay, float u0, float u1
)
{
	// 1. stretch wi
	optix::float3 wiStretched =
		optix::normalize(optix::make_float3(alphax * wi.x, alphay * wi.y, wi.z));

	// 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
	float slope_x, slope_y;
	TrowbridgeReitzSample11(CosTheta(wiStretched), u0, u1, slope_x, slope_y);

	// 3. rotate
	float tmp = CosPhi(wiStretched) * slope_x - SinPhi(wiStretched) * slope_y;
	slope_y = SinPhi(wiStretched) * slope_x + CosPhi(wiStretched) * slope_y;
	slope_x = tmp;

	// 4. unstretch
	slope_x = alphax * slope_x;
	slope_y = alphay * slope_y;

	// 5. compute normal
	return optix::normalize(optix::make_float3(-slope_x, -slope_y, 1.f));
}

static
__host__ __device__ __inline__ optix::float3 TrowbridgeReitzDistributionSampleWh(
	optix::float3 wo, optix::float2 randomU, float alphax, float alphay
)
{	
	optix::float3 wh;
	float cosTheta = 0, phi = (2 * M_PIf) * randomU.y;
	if (alphax == alphay) {
		float tanTheta2 = alphax * alphax * randomU.x/ (1.0f - randomU.x);
		cosTheta = 1 / sqrtf(1 + tanTheta2);
	}
	else {
		phi = atanf(alphay / alphax * tanf(2 * M_PIf * randomU.y + .5f * M_PIf));
		if (randomU.y > .5f) phi += M_PIf;
		float sinPhi = sinf(phi), cosPhi = cosf(phi);
		float alphax2 = alphax * alphax, alphay2 = alphay * alphay;
		float alpha2 = 1 / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
		float tanTheta2 = alpha2 * randomU.x / (1 - randomU.x);
		cosTheta = 1 / sqrtf(1 + tanTheta2);
	}
	float sinTheta = sqrtf(fmaxf(0.0, 1.f - cosTheta * cosTheta));
	wh = SphericalDirection(sinTheta, cosTheta, phi);
	if (!SameHemisphere(wo, wh)) wh = -wh;
	return wh;
}

static
__host__ __device__ __inline__ optix::float3 DisneyMicrofacetDistributionSampleWh(
	optix::float3 wo, optix::float2 randomU, float alphax, float alphay
)
{
	return TrowbridgeReitzDistributionSampleWh(wo, randomU, alphax, alphay);
}

static
__host__ __device__ __inline__ optix::float3 DisneyMicrofacetReflectionSampleF(
	optix::float3 wo, optix::float3 &wi,
	optix::float3 R, float roughness, float &pdf, optix::float2 randomU,
	float alphax, float alphay, 
	optix::float3 R0, float metallic, float eta
)
{
	if (wo.z <= 0.0001) return optix::make_float3(0.f);
	optix::float3 wh = DisneyMicrofacetDistributionSampleWh(wo, randomU, alphax, alphay);
	if (optix::dot(wo, wh) <= 0.0001) return optix::make_float3(0.f);   // Should be rare
	//wi = optix::reflect(wo, wh);
	wi = 2.0f * optix::dot(wo, wh) * wh - wo;
	if (!SameHemisphere(wo, wi)) return optix::make_float3(0.f);

	pdf = DisneyMicrofacetDistributionPdf(wo, wi, alphax, alphay) / (4 * optix::dot(wo, wh));

	return DisneyMicrofacetReflectionF(wo, wi, optix::make_float3(1.f), R0, alphax, alphay, metallic, eta);
}

//test
static
__host__ __device__ __inline__ optix::float3 SmithGGXSampleF(
	optix::float3 wo, optix::float3 &wi,
	optix::float3 R, float roughness, float &pdf, optix::float2 randomU,
	optix::float3 R0
)
{
	if (wo.z == 0) return optix::make_float3(0.f);
	optix::float3 p;
	specular_sample_hemisphere(randomU.x, randomU.y, p, roughness);
	if (optix::dot(wo, p) < 0) return optix::make_float3(0.f);
	wi = 2.0f * optix::dot(wo, p) * p - wo;
	wi = optix::normalize(wi);
	if (!SameHemisphere(wo, wi)) return optix::make_float3(0.f);

	pdf = SmithGGXPdf(wo, wi, roughness);

	return SmithGGXF(wo, wi, roughness, R0);
}

static
__host__ __device__ __inline__ float SchlickR0FromEta(float eta) 
{ return sqrtf(eta - 1) / sqrtf(eta + 1); }

static
__host__ __device__ __inline__ optix::float3 DisneyFakeSSF(
	optix::float3 wo, optix::float3 wi, optix::float3 R, float rough)
{
	optix::float3 wh = wi + wo;
	if (wh.x == 0 && wh.y == 0 && wh.z == 0)
		return optix::make_float3(0, 0, 0);
	wh = optix::normalize(wh);
	float cosThetaD = optix::dot(wi, wh);
	float Fss90 = cosThetaD * cosThetaD * rough;
	float Fo = SchlickWeight(AbsCosTheta(wo)),
		Fi = SchlickWeight(AbsCosTheta(wi));
	float Fss = optix::lerp(Fo, 1.0, Fss90) * optix::lerp(Fi, 1.0, Fss90);
	float ss = 1.25f * (Fss * (1 / (AbsCosTheta(wo) + AbsCosTheta(wi)) - .5f) + .5f);

	return R * M_1_PIf * ss;
}

static
__host__ __device__ __inline__ float DisneyFakeSSPdf(
	optix::float3 wo, optix::float3 wi
)
{
	return BxdfPdf(wo, wi);
}

static
__host__ __device__ __inline__ optix::float3 DisneyFakeSSSampleF(
	optix::float3 wo, optix::float3 &wi,
	optix::float3 R, float rough,
	float &pdf, optix::float2 randomU)
{
	optix::cosine_sample_hemisphere(randomU.x, randomU.y, wi);
	if (wo.z < 0) wi.z *= -1.f;
	pdf = DisneyFakeSSPdf(wo, wi);
	return DisneyFakeSSF(wo, wi, R, rough);
}

static
__host__ __device__ __inline__ optix::float3 DisneySheenF(
	optix::float3 wo, optix::float3 wi, optix::float3 R)
{
	optix::float3 wh = wi + wo;
	if (wh.x == 0 && wh.y == 0 && wh.z == 0) 
		return optix::make_float3(0, 0, 0);
	wh = optix::normalize(wh);
	float cosThetaD = optix::dot(wi, wh);

	return R * SchlickWeight(cosThetaD);
}

static
__host__ __device__ __inline__ float DisneySheenPdf(
	optix::float3 wo, optix::float3 wi
)
{
	return BxdfPdf(wo, wi);
}

static
__host__ __device__ __inline__ optix::float3 DisneySheenSampleF(
	optix::float3 wo, optix::float3 &wi,
	optix::float3 R, float &pdf, optix::float2 randomU)
{
	optix::cosine_sample_hemisphere(randomU.x, randomU.y, wi);
	if (wo.z < 0) wi.z *= -1.f;
	pdf = DisneySheenPdf(wo, wi);
	return DisneySheenF(wo, wi, R);
}

static
__host__ __device__ __inline__ optix::float3 DisneyClearcoatF(
	optix::float3 wo, optix::float3 wi, optix::float3 weight, float gloss)
{	
	//test
	//return SmithGGXF(wo, wi, gloss, weight);

	optix::float3 wh = wi + wo;
	if (wh.x == 0 && wh.y == 0 && wh.z == 0) 
		return optix::make_float3(0, 0, 0);
	wh = optix::normalize(wh);
	float Dr = GTR1(AbsCosTheta(wh), gloss);
	optix::float3 Fr = FrSchlick(
		optix::make_float3(0.04),optix::dot(wo, wh));
	float Gr = SmithGGX(AbsCosTheta(wo), .25) * 
			SmithGGX(AbsCosTheta(wi), .25);
	return weight * Gr * Fr * Dr / 4;
}

static
__host__ __device__ __inline__ float DisneyClearcoatPdf(
	optix::float3 wo, optix::float3 wi, float gloss
)
{	
	//test
	//return SmithGGXPdf(wo, wi, gloss);

	if (!SameHemisphere(wo, wi)) return 0;
	optix::float3 wh = wi + wo;
	if (wh.x == 0 && wh.y == 0 && wh.z == 0) 
		return 0;
	wh = optix::normalize(wh);
	float Dr = GTR1(AbsCosTheta(wh), gloss);
	return Dr * AbsCosTheta(wh) / (4 * optix::dot(wo, wh));
}

static
__host__ __device__ __inline__ optix::float3 DisneyClearcoatSampleF(
	optix::float3 wo, optix::float3 &wi,
	optix::float3 weight, float gloss, 
	float &pdf, optix::float2 randomU)
{	
	//test
	//return SmithGGXSampleF(wo, wi, optix::make_float3(0), gloss, pdf, randomU, weight);

	if (wo.z == 0) 
		return optix::make_float3(0);
	float alpha2 = gloss * gloss;
	float cosTheta = sqrtf(
		fmaxf(0.0, (1 - powf(alpha2, 1.0 - randomU.x)) / (1 - alpha2)));
	float sinTheta = sqrtf(
		fmaxf(0.f, 1 - cosTheta * cosTheta));
	float phi = 2 * M_PIf * randomU.y;
	optix::float3 wh = SphericalDirection(sinTheta, cosTheta, phi);
	if (!SameHemisphere(wo, wh)) wh = -wh;
	wi = 2.0f * optix::dot(wo, wh) * wh - wo;
	if (!SameHemisphere(wo, wi)) 
		return optix::make_float3(0);
	pdf = DisneyClearcoatPdf(wo, wi, gloss);
	return DisneyClearcoatF(wo, wi, weight, gloss);
}

static
__host__ __device__ __inline__ optix::float3 DisneyMicrofacetTransmissionF(
	optix::float3 wo, optix::float3 wi, 
	optix::float3 T, float alphax, float alphay,
	float etaA, float etaB)
{	
	float m_eta = etaA / etaB;
	float m_invEta = 1 / m_eta;

	float eta = CosTheta(wo) > 0 ? m_eta : m_invEta;

	optix::float3 wh = optix::normalize(wo + eta * wi);

	if (wh.z < 0) wh *= -1;

	//dist pdf: pdf all, transmission use GGX dist(trow-reitz)
	float D = 0;
	{
		if (CosTheta(wh) > 0)
		{
			float cosTheta2 = Cos2Theta(wh);
			float beckmannExponent = ((wh.x*wh.x) / (alphax * alphax)
				+ (wh.y*wh.y) / (alphay * alphay)) / cosTheta2;

			float root = (1 + beckmannExponent) * cosTheta2;
			float D = 1.0 / (M_PIf * alphax * alphay * root * root);
		}
	}

	//fresnel
	float F = 0;
	{
		float cosThetaI = optix::dot(wo, wh);
		if (m_eta != 1)
		{
			float scale = (cosThetaI > 0) ? 1 / m_eta : m_eta;
			float cosThetaTSqr = 1 - (1 - cosThetaI * cosThetaI) * (scale*scale);

			if (cosThetaTSqr <= 0.0f)
			{
				F = 1.0;
			}
			else
			{
				float cosThetaIb = fabs(cosThetaI);
				float cosThetaT = sqrtf(cosThetaTSqr);

				float Rs = (cosThetaIb - m_eta * cosThetaT)
					/ (cosThetaIb + m_eta * cosThetaT);
				float Rp = (m_eta * cosThetaIb - cosThetaT)
					/ (m_eta * cosThetaIb + cosThetaT);

				F = 0.5 * (Rs * Rs + Rp * Rp);
			}
		}
	}

	//shadow-maksing :smith ggx
	float G = 0;
	{
		if (optix::dot(wo, wh) *CosTheta(wo) <= 0)
		{
			G = 0;
		}
		else
		{
			float tanTheta = fabs(TanTheta(wo));
			if (tanTheta == 0.0f)
			{
				G = 1;
			}
			else
			{
				float invSinTheta2 = 1 / Sin2Theta(wo);
				float cosPhi2 = wo.x * wo.x * invSinTheta2;
				float sinPhi2 = wo.y * wo.y * invSinTheta2;
				float alpha = sqrtf(cosPhi2 * alphax * alphax
					+ sinPhi2 * alphay * alphay);

				float root = alpha * tanTheta;
				G = 2.0f / (1.0f + hypotf(1.0, root));
			}
		}
	}

	float sqrtDenom = optix::dot(wo, wh) + eta * optix::dot(wi, wh);
	float value = ((1 - F) * D * G * eta * eta
		*  optix::dot(wo, wh) *  optix::dot(wi, wh)) /
		(CosTheta(wo) * sqrtDenom * sqrtDenom);

	float factor = CosTheta(wo) > 0 ? m_invEta : m_eta;

	return T * fabs(value * factor * factor);

	//if (SameHemisphere(wo, wi))
	//	return optix::make_float3(0);

	//float cosThetaO = CosTheta(wo);
	//float cosThetaI = CosTheta(wi);
	//if (cosThetaI == 0 || cosThetaO == 0) 
	//	return optix::make_float3(0);

	//float eta = CosTheta(wo) > 0 ? (etaB / etaA) : (etaA / etaB);
	//optix::float3 wh = optix::normalize(wo + wi * eta);
	//if (wh.z < 0) wh = -wh;

	//optix::float3 F = FresnelDielectricEvaluate(etaA, etaB,optix::dot(wo, wh));

	//float sqrtDenom = optix::dot(wo, wh) + eta * optix::dot(wi, wh);
	//float factor = 1 / eta;

	//return (optix::make_float3(1.f) - F) * T *
	//		fabs(TrowbridgeReitzD(wh, alphax, alphay) 
	//		* DisneyMicrofacetDistributionG(wo,wi,alphax,alphay) * eta * eta *
	//		fabs(optix::dot(wi, wh)) * fabs(optix::dot(wo, wh))
	//			* factor * factor /
	//		(cosThetaI * cosThetaO * sqrtDenom * sqrtDenom));
}

static
__host__ __device__ __inline__ float DisneyMicrofacetTransmissionPdf(
	optix::float3 wo, optix::float3 wi, float etaA, float etaB,
	float alphax, float alphay)
{	
	float m_eta = etaA / etaB;
	float m_invEta = 1 / m_eta;

	float eta = CosTheta(wo) > 0 ? m_eta : m_invEta;
	
	optix::float3 wh = optix::normalize(wo + eta * wi);

	float sqrtDenom = optix::dot(wo, wh) + eta * optix::dot(wi, wh);
	float dwh_dwi = (eta * eta * optix::dot(wi, wh)) / (sqrtDenom*sqrtDenom);

	if (wh.z < 0) wh *= -1;

	float scaleAlpha = 1.2 - 0.2 * sqrtf(AbsCosTheta(wo));
	alphax *= scaleAlpha;
	alphay *= scaleAlpha;

	//dist pdf: pdf all, transmission use GGX dist(trow-reitz)
	if (CosTheta(wh) <= 0) return 0;

	float cosTheta2 = Cos2Theta(wh);
	float beckmannExponent = ((wh.x*wh.x) / (alphax * alphax)
		+ (wh.y*wh.y) / (alphay * alphay)) / cosTheta2;

	float root = (1 + beckmannExponent) * cosTheta2;
	float prob = 1.0 / (M_PIf * alphax * alphay * root * root);
	prob *= CosTheta(wh);

	//fresnel
	float F = 0;
	float cosThetaI = optix::dot(wo, wh);
	if (m_eta != 1)
	{
		float scale = (cosThetaI > 0) ? 1 / m_eta : m_eta;
		float cosThetaTSqr = 1 - (1 - cosThetaI * cosThetaI) * (scale*scale);

		if (cosThetaTSqr <= 0.0f)
		{
			F = 1.0;
		}
		else
		{
			float cosThetaIb = fabs(cosThetaI);
			float cosThetaT = sqrtf(cosThetaTSqr);

			float Rs = (cosThetaIb - m_eta * cosThetaT)
				/ (cosThetaIb + m_eta * cosThetaT);
			float Rp = (m_eta * cosThetaIb - cosThetaT)
				/ (m_eta * cosThetaIb + cosThetaT);

			F = 0.5 * (Rs * Rs + Rp * Rp);
		}
	}

	float pdf = prob * (1 - F);
	
	if (pdf < 1e-20f)	return 0;
	
	return pdf;
}

static
__host__ __device__ __inline__ optix::float3 GGXDistSampleAll(
	optix::float2 sample, float &pdf, float alphax, float alphay
)
{
	float cosThetaM = 0.0f;
	float sinPhiM, cosPhiM;
	float alphaSqr;

	/* Sample phi component (anisotropic case) */
	float phiM = atanf(alphay / alphax *
		tanf(M_PIf + 2 * M_PIf*sample.y)) + M_PIf * floorf(2 * sample.y + 0.5f);
	sinPhiM = sinf(phiM);
	cosPhiM = cosf(phiM);

	float cosSc = cosPhiM / alphax, sinSc = sinPhiM / alphay;
	alphaSqr = 1.0f / (cosSc*cosSc + sinSc * sinSc);

	/* Sample theta component */
	float tanThetaMSqr = alphaSqr * sample.x / (1.0f - sample.x);
	cosThetaM = 1.0f / sqrtf(1.0f + tanThetaMSqr);

	/* Compute probability density of the sampled position */
	float temp = 1 + tanThetaMSqr / alphaSqr;
	pdf = M_1_PIf / (alphax*alphay*cosThetaM*cosThetaM*cosThetaM*temp*temp);
	
	if (pdf < 1e-20f)
		pdf = 0;

	float sinThetaM = sqrtf(
		fmaxf((float)0, 1 - cosThetaM * cosThetaM));

	return optix::make_float3(
		sinThetaM * cosPhiM,
		sinThetaM * sinPhiM,
		cosThetaM);
}

float fresnelDielectricExt(float cosThetaI_, float &cosThetaT_, float eta) {
	if (eta == 1) {
		cosThetaT_ = -cosThetaI_;
		return 0.0f;
	}

	/* Using Snell's law, calculate the squared sine of the
	   angle between the normal and the transmitted ray */
	float scale = (cosThetaI_ > 0) ? 1 / eta : eta,
		cosThetaTSqr = 1 - (1 - cosThetaI_ * cosThetaI_) * (scale*scale);

	/* Check for total internal reflection */
	if (cosThetaTSqr <= 0.0f) {
		cosThetaT_ = 0.0f;
		return 1.0f;
	}

	/* Find the absolute cosines of the incident/transmitted rays */
	float cosThetaI = fabs(cosThetaI_);
	float cosThetaT = sqrtf(cosThetaTSqr);

	float Rs = (cosThetaI - eta * cosThetaT)
		/ (cosThetaI + eta * cosThetaT);
	float Rp = (eta * cosThetaI - cosThetaT)
		/ (eta * cosThetaI + cosThetaT);

	cosThetaT_ = (cosThetaI_ > 0) ? -cosThetaT : cosThetaT;

	/* No polarization -- return the unpolarized reflectance */
	return 0.5f * (Rs * Rs + Rp * Rp);
}

static
__host__ __device__ __inline__ optix::float3 refract(
	optix::float3 wo, optix::float3 n, float eta, float cosThetaT) {
	if (cosThetaT < 0)
		eta = 1 / eta;

	return n * (optix::dot(wo, n) * eta + cosThetaT) - wo * eta;
}

static
__host__ __device__ __inline__ float GGXDistEval(optix::float3 m,
	float m_alphaU, float m_alphaV)
{
	if (CosTheta(m) <= 0)
		return 0.0f;

	float cosTheta2 = Cos2Theta(m);
	float beckmannExponent = ((m.x*m.x) / (m_alphaU * m_alphaU)
		+ (m.y*m.y) / (m_alphaV * m_alphaV)) / cosTheta2;

	float result;
	/* GGX / Trowbridge-Reitz distribution function for rough surfaces */
	float root = ((float)1 + beckmannExponent) * cosTheta2;
	result = (float)1 / (M_PIf * m_alphaU * m_alphaV * root * root);

	if (result * CosTheta(m) < 1e-20f)
		result = 0;

	return result;
}

inline float projectRoughness(
	optix::float3&v, float m_alphaU, float m_alphaV)
{
	float invSinTheta2 = 1 /Sin2Theta(v);

	if (invSinTheta2 <= 0)
		return m_alphaU;

	float cosPhi2 = v.x * v.x * invSinTheta2;
	float sinPhi2 = v.y * v.y * invSinTheta2;

	return sqrtf(
		cosPhi2 * m_alphaU * m_alphaU 
		+ sinPhi2 * m_alphaV * m_alphaV);
}

static
__host__ __device__ __inline__ float SmithG1(
	optix::float3 v, const optix::float3 m , float m_alphaU, float m_alphaV)
{
	if (optix::dot(v, m) * CosTheta(v) <= 0)
		return 0.0f;

	/* Perpendicular incidence -- no shadowing/masking */
	float tanTheta = fabs(TanTheta(v));
	if (tanTheta == 0.0f)
		return 1.0f;

	float alpha = projectRoughness(v, m_alphaU, m_alphaV);

	float root = alpha * tanTheta;
	return 2.0f / (1.0f + hypotf(1.0f, root));
}

static
__host__ __device__ __inline__ float GGXDistG(
	optix::float3 wi, optix::float3 wo, optix::float3 m, 
	float m_alphaU, float m_alphaV)
{
	return SmithG1(wi, m, m_alphaU, m_alphaV) 
		* SmithG1(wo, m, m_alphaU, m_alphaV);
}

static
__host__ __device__ __inline__ optix::float3 Refract(
	optix::float3 wi, float cosThetaT, float m_eta)
{
	float scale = -(cosThetaT < 0 ? 1 / m_eta : m_eta);
	return optix::make_float3(scale*wi.x, scale*wi.y, cosThetaT);
}

static
__host__ __device__ __inline__ optix::float3 DisneyMicrofacetTransmissionSampleF(
	optix::float3 wo, optix::float3 &wi, float etaA, float etaB,
	float alphax, float alphay, optix::float3 T,
	float &pdf, optix::float2 randomU)
{	
	float m_eta = etaB / etaA;
	float m_invEta = 1 / m_eta;

	float cosThetaT;
	float F = fresnelDielectricExt(CosTheta(wo), cosThetaT, m_eta);

	wi = Refract(wo, cosThetaT, m_eta);
	wi = optix::normalize(wi);
	float eta = cosThetaT < 0 ? m_eta : m_invEta;
	pdf = 1;

	float factor = cosThetaT < 0 ? m_invEta : m_eta;

	return T;// *(factor * factor);

	//float m_eta = etaA / etaB;
	//float m_invEta = 1 / m_eta;
	//float scaleAlpha = 1.2 - 0.2 * sqrtf(AbsCosTheta(wo));
	//alphax *= scaleAlpha;
	//alphay *= scaleAlpha;

	///* Sample M, the microfacet normal */
	//float microfacetPDF;
	//optix::float3 m = GGXDistSampleAll(randomU, microfacetPDF, alphax, alphay);
	//if (microfacetPDF == 0)
	//	return optix::make_float3(0);
	//pdf = microfacetPDF;

	//float cosThetaT;
	//float F = fresnelDielectricExt(optix::dot(wo, m), cosThetaT, m_eta);
	//optix::float3 weight = optix::make_float3(1.0f);

	//weight *=  (1 - F);

	//float dwh_dwo;

	//if (cosThetaT == 0)
	//	return optix::make_float3(0);

	///* Perfect specular transmission based on the microfacet normal */
	//wi = refract(wo, m, m_eta, cosThetaT);
	//float eta = cosThetaT < 0 ? m_eta : m_invEta;

	///* Side check */
	//if (CosTheta(wo) * CosTheta(wi) >= 0)
	//	return optix::make_float3(0);

	///* Radiance must be scaled to account for the solid angle compression
	//	that occurs when crossing the interface. */
	//float factor =  cosThetaT < 0 ? m_invEta : m_eta;

	//weight *= T * (factor * factor);

	///* Jacobian of the half-direction mapping */
	//float sqrtDenom = optix::dot(wo, m) + eta * optix::dot(wi, m);
	//dwh_dwo = (eta*eta * optix::dot(wi, m)) / (sqrtDenom*sqrtDenom);

	//weight *= fabs(GGXDistEval(m, alphax, alphay) 
	//	*GGXDistG(wo, wi, m, alphax, alphay)
	//	* optix::dot(wo, m) / (microfacetPDF * CosTheta(wo)));

	//pdf *= fabs(dwh_dwo);

	//return weight;

	//pdf = DisneyMicrofacetTransmissionPdf(
	//wo, wi, etaA, etaB, alphax, alphay);
	//return DisneyMicrofacetTransmissionF(
	//	wo, wi, T, alphax, alphay, etaA, etaB);

	//if (wo.z == 0)
	//	return optix::make_float3(0);

	//optix::float3 wh = DisneyMicrofacetDistributionSampleWh(
	//	wo, randomU, alphax, alphay);
	//if (optix::dot(wo, wh) < 0) 
	//	return optix::make_float3(0);

	//wh = optix::normalize(wh);

	//float eta = CosTheta(wo) > 0 ? (etaA / etaB) : (etaB / etaA);
	//if (!Refract(wo, wh, eta, wi)) return 
	//	optix::make_float3(0);

	//pdf = DisneyMicrofacetTransmissionPdf(
	//	wo, wi, etaA, etaB, alphax, alphay);
	//return DisneyMicrofacetTransmissionF(
	//	wo, wi, T, alphax, alphay, etaA, etaB);
}

static
__host__ __device__ __inline__ optix::float3 LambertianTransmissionF(
	optix::float3 wo, optix::float3 wi, optix::float3 T
)
{
	return T * M_1_PIf;
}

static
__host__ __device__ __inline__ float LambertianTransmissionPdf(
	optix::float3 wo, optix::float3 wi)
{
	return !SameHemisphere(wo, wi) ? AbsCosTheta(wi) * M_1_PIf : 0;
}

static
__host__ __device__ __inline__ optix::float3 LambertianTransmissionSampleF(
	optix::float3 wo, optix::float3 wi,
	optix::float2 randomU, float &pdf, optix::float3 T)
{
	optix::cosine_sample_hemisphere(randomU.x, randomU.y, wi);
	if (wo.z > 0) wi.z *= -1;
	pdf = LambertianTransmissionPdf(wo, wi);
	return LambertianTransmissionF(wo, wi, T);
}


static
__host__ __device__ __inline__ optix::float3 DisneyBsdfF(
	optix::float3 woWorld, optix::float3 wiWorld,
	optix::float3 wo, optix::float3 wi,
	optix::float3 ng, optix::float3 R,
	optix::float3 R0, float alphax, float alphay,
	float rough, float metallic, float eta,
	optix::float3 Rss,
	optix::float3 Rsheen,
	optix::float3 Rcc, float gloss,
	optix::float3 Rt, float etaA, float etaB,
	optix::float3 RLamTrans)
{	
	if (wo.z == 0) return optix::make_float3(0.f);
	bool reflect = (optix::dot(wiWorld, ng) * optix::dot(woWorld, ng)) > 0;
	optix::float3 f = optix::make_float3(0.f);

	if (reflect)
	{	
		f += DisneyDiffuseF(wo, wi, R);	
		f += DisneyRetroF(wo, wi, R, rough);		
		f += DisneyMicrofacetReflectionF(
			wo, wi, optix::make_float3(1.f), R0, alphax, alphay, metallic, eta);
		f += DisneyFakeSSF(wo, wi, Rss, rough);
		f += DisneySheenF(wo, wi, Rsheen);
		f += DisneyClearcoatF(wo, wi, Rcc, gloss);
	}
	else
	{
		//f += DisneyMicrofacetTransmissionF(
		//	wo, wi, Rt, alphax, alphay, etaA, etaB);
		//f += LambertianTransmissionF(wo, wi, RLamTrans);
	}
	return f;
}

static
__host__ __device__ __inline__ float DisneyBsdfPdf(
	optix::float3 woWorld, optix::float3 wiWorld,
	optix::float3 wo, optix::float3 wi,
	int matchingComps, float alphax, float alphay
)
{
	if (wo.z == 0.f) return 0.f;
	float pdf = 0.f;
	
	pdf += DisneyDiffusePdf(wo, wi);
	pdf += DisneyRetroPdf(wo, wi);
	pdf += DisneyMicrofacetReflectionPdf(wo, wi, alphax, alphay);

	float v = pdf / matchingComps;
	return v;
}
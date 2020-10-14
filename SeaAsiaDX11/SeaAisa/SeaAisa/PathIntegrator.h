#pragma once
#include <optix_world.h>
#include <optixu/optixu_math_namespace.h>
#include "PathReflection.h"
#include "PathTracerLight.h"
#include "GradientPath.h"
#include <optix_cuda.h>

static
__device__ optix::float3 EstimateDirect(
	DirectionalLight light,
	optix::float3 woWorld, 
	optix::float3 wo,
	optix::float3 ng, optix::float3 ns, optix::float3 R,
	optix::float3 R0, float alphax, float alphay,
	float rough, float metallic, float eta,
	optix::float3 Rss,
	optix::float3 Rsheen,
	optix::float3 Rcc, float gloss,
	optix::float3 Rt, float etaA, float etaB,
	optix::float3 RLamTrans,
	int compNum,
	optix::float3 &lightDir, float &lightDist,
	optix::Onb onb
)
{
	optix::float3 Ld = optix::make_float3(0.f);
	optix::float3 wiWorld;
	float wiDist;
	float lightPdf = 0.f, scatteringPdf = 0;
	//visibility tets isn't here, it is outside this function
	optix::float3 Li = LightSampleLi(wiWorld, lightPdf, light, wiDist);
	optix::float3 wi = optix::normalize(WorldtoLocal(wiWorld, onb));
	if (lightPdf > 0 && 
		(Li.x > 0.f || Li.y > 0.f || Li.z > 0.f))
	{
		optix::float3 f;
		f = DisneyBsdfF(
			woWorld, wiWorld, wo, wi, ns, R, R0,
			alphax, alphay, rough, metallic, eta,
			Rss,
			Rsheen,
			Rcc, gloss,
			Rt, etaA, etaB,
			RLamTrans)
			* abs(optix::dot(wiWorld, ns));

		scatteringPdf = DisneyBsdfPdf(woWorld, wiWorld, wo, wi, compNum, alphax, alphay);

		if (Li.x > 0.f || Li.y > 0.f || Li.z > 0.f)
		{
			Ld += f * Li / lightPdf;
		}
	}

	lightDist = wiDist;
	lightDir = -wiWorld;

	return Ld;
}

static
__device__ optix::float3 EstimateDirect(
	DirectionalLight light,
	optix::float3 woWorld,
	optix::float3 wo,
	optix::float3 ng, optix::float3 ns, optix::float3 R,
	optix::float3 R0, float alphax, float alphay,
	float rough, float metallic, float eta,
	optix::float3 Rss,
	optix::float3 Rsheen,
	optix::float3 Rcc, float gloss,
	optix::float3 Rt, float etaA, float etaB,
	optix::float3 RLamTrans,
	int compNum,
	DirectSampleLight & lightSample,
	optix::Onb onb
)
{
	optix::float3 Ld = optix::make_float3(0.f);
	optix::float3 wiWorld;
	float wiDist;
	float lightPdf = 0.f, scatteringPdf = 0;
	optix::float3 f = optix::make_float3(0);
	//visibility tets isn't here, it is outside this function
	optix::float3 Li = LightSampleLi(wiWorld, lightPdf, light, wiDist);
	optix::float3 wi = optix::normalize(WorldtoLocal(wiWorld, onb));
	if (lightPdf > 0 &&
		(Li.x > 0.f || Li.y > 0.f || Li.z > 0.f))
	{
		f = DisneyBsdfF(
			woWorld, wiWorld, wo, wi, ns, R, R0,
			alphax, alphay, rough, metallic, eta,
			Rss,
			Rsheen,
			Rcc, gloss,
			Rt, etaA, etaB,
			RLamTrans)
			* abs(optix::dot(wiWorld, ns));

		scatteringPdf = DisneyBsdfPdf(woWorld, wiWorld, wo, wi, compNum, alphax, alphay);

		if (Li.x > 0.f || Li.y > 0.f || Li.z > 0.f)
		{
			Ld += f * Li / lightPdf;
		}
	}

	lightSample.lum = Li;
	lightSample.lumPdf = lightPdf;
	lightSample.dir = -wiWorld;
	lightSample.dist = wiDist;
	lightSample.bsdfResult.wo = woWorld;
	lightSample.bsdfResult.wi = wiWorld;
	lightSample.bsdfResult.f = f;
	lightSample.bsdfResult.weight = f * fabs(CosTheta(wi));
	lightSample.bsdfResult.pdf = 0; //delta and no area light pdf = 0
	lightSample.bsdfResult.eta = etaA / etaB;
	lightSample.lightIntersect.normal = -wi;

	return Ld;
}
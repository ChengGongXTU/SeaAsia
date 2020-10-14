#pragma once
#include <optixu/optixu_math_namespace.h>      

struct RtMaterial
{
	//parameter
	optix::float3 colorFactor;
	optix::float3 ambientClor;
	optix::float1 metallicFactor;
	optix::float1 roughnessFactor;
	optix::float1 IBLFactor;

	/*
	type:
	0 - disney diffuse + GGX specular brdf
	*/
	optix::uint1 type;

	//texture ID
	optix::uint1 albedoID;
	optix::uint1 normalID;
	optix::uint1 mraID;	//metallic: r, roughness: g, AO: b.

	optix::float2 scale;
	optix::float2 offset;

	//disney lerp para
	optix::float1 eta;
	optix::float1 strans;
	optix::float1 dt;
	optix::float1 sheenWeight;
	optix::float1 sTint;
	optix::float1 flat;
	optix::float1 anisotropic;
	optix::float1 specTint;
	optix::float1 cc;
	optix::float1 glossTint;
};

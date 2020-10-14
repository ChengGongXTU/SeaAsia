#pragma once
#include <optixu/optixu_math_namespace.h>      

struct DirectionalLight
{
	optix::float3 dir;
	optix::float3 intensity;
};

struct PointLight
{
	optix::float3 corner;
	optix::float3 intensity;
	optix::float1 range;
};

struct SpotLight
{
	optix::float3 corner;
	optix::float3 dir;
	optix::float3 intensity;
	optix::float1 range;
	optix::float1 angle_u;
	optix::float1 angle_p;
};

static
__host__ __device__ __inline__ optix::float3 LightSampleLi(
	optix::float3 &wiWorld, float &pdf, DirectionalLight light,
	float &lightDist
)
{
	wiWorld = optix::normalize(-light.dir);
	pdf = 1;
	lightDist = 9999.f;
	return light.intensity;
}

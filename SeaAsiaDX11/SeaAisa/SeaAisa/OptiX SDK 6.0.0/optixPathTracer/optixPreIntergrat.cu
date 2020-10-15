#include <optixu/optixu_math_namespace.h>

#define RADIANCE_RAY_TYPE 0
#define SHADOW_RAY_TYPE 1

#include "../../PathTracerLight.h"
#include "../../random.h"
#include "../../PathTracerMaterial.h"
#include "../../PathReflection.h"
#include "../../PathIntegrator.h"
#include "../../GradientPath.h"

using namespace optix;

struct PerRayData_pathtrace_shadow
{
    bool inShadow;
};

// Scene wide variables
rtDeclareVariable(float,         scene_epsilon, , );
rtDeclareVariable(rtObject,      top_object, , );
rtDeclareVariable(uint2,         launch_index, rtLaunchIndex, );
rtDeclareVariable(GradientPath, current_prd, rtPayload, );
//-----------------------------------------------------------------------------
//
//  Camera program -- main ray tracing loop
//
//-----------------------------------------------------------------------------
rtDeclareVariable(unsigned int, mipmapIndex, , );

rtDeclareVariable(float3,        eye, , );
rtDeclareVariable(float3,        U, , );
rtDeclareVariable(float3,        V, , );
rtDeclareVariable(float3,        W, , );
rtDeclareVariable(float3,        bad_color, , );
rtDeclareVariable(unsigned int,  frame_number, , );
rtDeclareVariable(unsigned int,  sqrt_num_samples, , );
rtDeclareVariable(unsigned int,  rr_begin_depth, , );
rtDeclareVariable(unsigned int, loop_depth, , );
rtDeclareVariable(unsigned int, bool_compute_gradients, , );

rtBuffer<float4, 2>					output_buffer;
rtBuffer<DirectionalLight>			dirLights;
rtBuffer<RtMaterial>					materials;
rtBuffer<unsigned int>						texture_id;
rtDeclareVariable(int,         skybox_id, , );

rtDeclareVariable(float3, bg_color, , );

rtTextureSampler<float4, 2> envmap;

rtDeclareVariable(float3,     diffuse_color, , );
rtDeclareVariable(float3,     geometric_normal, attribute geometric_normal, );
rtDeclareVariable(float3,     shading_normal,   attribute shading_normal, );
rtDeclareVariable( float3, texcoord,         attribute texcoord, );
rtDeclareVariable(optix::Ray, ray,              rtCurrentRay, );
rtDeclareVariable(float,      t_hit,            rtIntersectionDistance, );
rtDeclareVariable(unsigned int, primitive_mat_ind, attribute primitive_mat_ind, );
rtTextureSampler<float4, 2> albedoTex;
rtTextureSampler<float4, 2> normalTex;
rtTextureSampler<float4, 2> mraTex;

rtDeclareVariable(PerRayData_pathtrace_shadow, current_prd_shadow, rtPayload, );

RT_PROGRAM void pathtrace_camera()
{	
    size_t2 screen = output_buffer.size();
    float2 inv_screen = 1.0f/make_float2(screen);
	float2 pixel = (make_float2(launch_index) + make_float2(0.5)) * inv_screen;
    float2 jitter_scale = make_float2(1.0f / sqrt_num_samples);
	unsigned int samples_per_pixel = sqrt_num_samples*sqrt_num_samples;
	
	float3 result = make_float3(0.0);

	//float theta = atan2f(ray.direction.x, ray.direction.z);
	//float phi = M_PIf * 0.5f - acosf(ray.direction.y);
	//float u = (theta + M_PIf) * (0.5f * M_1_PIf);
	//float v = 0.5f * (1.0f + sin(phi));
	//float4 hdrEnv = tex2D(envmap, u, v);
	float u = pixel.x;
	float v = pixel.y;
	float theta = u * 2.0f * M_PIf - M_PIf;
	float phi = asinf(v * 2.0f - 1.0f);
	float y = cosf(M_PIf * 0.5f - phi);
	float xDeivZ = tanf(theta);

	float3 normal;
	//optix::cosine_sample_hemisphere(u, v, normal);
	normal.x = sinf(v * M_PIf) * cosf(u * 2 * M_PIf);
	normal.y = cosf(v * M_PIf);
	normal.z = sinf(v * M_PIf) * sinf(u * 2 * M_PIf);
	normal = normalize(normal);

    unsigned int seed = tea<16>(screen.x*launch_index.y+launch_index.x, frame_number);
    do 
    {	
		optix::Onb onb(normal);
		float2 uv = make_float2(rnd(seed), rnd(seed));
		float3 direction;
		float rough =  mipmapIndex / 5.0f;
		specular_sample_hemisphere(uv.x, uv.y, direction, rough);
		direction = normalize(direction);
		direction = normalize(LocaltoWorld(direction, onb));
		direction = 2.0f * dot(normal, direction) * direction - normal;

		float theta = atan2f(direction.x, direction.z);
		float phi = M_PIf * 0.5f - acosf(direction.y);
		float u = (theta + M_PIf) * (0.5f * M_1_PIf);
		float v = 0.5f * (1.0f + sin(phi));
		float4 hdrEnv = tex2D(envmap, u, v);
		float3 emitterRadiance = make_float3(hdrEnv.x, hdrEnv.y, hdrEnv.z);
		
		result += fmaxf(dot(direction, normal), 0.0f) * emitterRadiance;
    } while (--samples_per_pixel);

    //
    // Update the output buffer
    //
    float3 pixel_color = result/(sqrt_num_samples*sqrt_num_samples);

    output_buffer[launch_index] = make_float4(pixel_color, 1.0f);
}

RT_PROGRAM void disneyBRDF()
{

}

  
//-----------------------------------------------------------------------------
//
//  Shadow any-hit
//
//-----------------------------------------------------------------------------



RT_PROGRAM void shadow()
{
    current_prd_shadow.inShadow = true;
    rtTerminateRay();
}


//-----------------------------------------------------------------------------
//
//  Exception program
//
//-----------------------------------------------------------------------------

RT_PROGRAM void exception()
{
    output_buffer[launch_index] = make_float4(bad_color, 1.0f);
}


//-----------------------------------------------------------------------------
//
//  Miss program
//
//-----------------------------------------------------------------------------


RT_PROGRAM void miss()
{	
	float theta = atan2f( ray.direction.x, ray.direction.z );
	float phi   = M_PIf * 0.5f -  acosf( ray.direction.y );
	float u     = (theta + M_PIf) * (0.5f * M_1_PIf);
	float v     = 0.5f * ( 1.0f + sin(phi) );
	float4 hdrEnv = tex2D(envmap, u, v);

    current_prd.emitterRadiance = make_float3(hdrEnv.x, hdrEnv.y, hdrEnv.z);
	current_prd.emitterPdf = 0;
	current_prd.hitType = ENV;
    current_prd.done = true;

	if (current_prd.depth == 0)
	{	
		current_prd.feature.albedo = make_float3(0,0,0);
		current_prd.feature.normal = -ray.direction;
		current_prd.feature.depth = make_float3(1000);
	}
}



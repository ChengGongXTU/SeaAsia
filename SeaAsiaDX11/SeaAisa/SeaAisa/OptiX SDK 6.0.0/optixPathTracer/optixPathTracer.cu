/* 
 * Copyright (c) 2018, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <optixu/optixu_math_namespace.h>

#define RADIANCE_RAY_TYPE 0
#define SHADOW_RAY_TYPE 1

#include "../../PathTracerLight.h"
#include "../../random.h"
#include "../../PathTracerMaterial.h"
#include "../../PathReflection.h"
#include "../../PathIntegrator.h"

using namespace optix;

struct PerRayData_pathtrace
{
    float3 result;
    float3 radiance;
    float3 attenuation;
    float3 origin;
    float3 direction;
    unsigned int seed;
    int depth;
    int countEmitted;
    int done;
};

struct PerRayData_pathtrace_shadow
{
    bool inShadow;
};

// Scene wide variables
rtDeclareVariable(float,         scene_epsilon, , );
rtDeclareVariable(rtObject,      top_object, , );
rtDeclareVariable(uint2,         launch_index, rtLaunchIndex, );

rtDeclareVariable(PerRayData_pathtrace, current_prd, rtPayload, );



//-----------------------------------------------------------------------------
//
//  Camera program -- main ray tracing loop
//
//-----------------------------------------------------------------------------

rtDeclareVariable(float3,        eye, , );
rtDeclareVariable(float3,        U, , );
rtDeclareVariable(float3,        V, , );
rtDeclareVariable(float3,        W, , );
rtDeclareVariable(float3,        bad_color, , );
rtDeclareVariable(unsigned int,  frame_number, , );
rtDeclareVariable(unsigned int,  sqrt_num_samples, , );
rtDeclareVariable(unsigned int,  rr_begin_depth, , );

rtBuffer<float4, 2>					output_buffer;
rtBuffer<DirectionalLight>			dirLights;
rtBuffer<RtMaterial>					materials;
rtBuffer<unsigned int>						texture_id;
rtDeclareVariable(int,         skybox_id, , );

RT_PROGRAM void pathtrace_camera()
{
    size_t2 screen = output_buffer.size();

    float2 inv_screen = 1.0f/make_float2(screen) * 2.f;
    float2 pixel = (make_float2(launch_index)) * inv_screen - 1.f;

    float2 jitter_scale = inv_screen / sqrt_num_samples;
    unsigned int samples_per_pixel = sqrt_num_samples*sqrt_num_samples;
    float3 result = make_float3(0.0f);

    unsigned int seed = tea<16>(screen.x*launch_index.y+launch_index.x, frame_number);
    do 
    {
        //
        // Sample pixel using jittering
        //
        unsigned int x = samples_per_pixel%sqrt_num_samples;
        unsigned int y = samples_per_pixel/sqrt_num_samples;
        float2 jitter = make_float2(x-rnd(seed), y-rnd(seed));
        float2 d = pixel + jitter*jitter_scale;
        float3 ray_origin = eye;
        float3 ray_direction = normalize(d.x*U + d.y*V + W);

        // Initialze per-ray data
        PerRayData_pathtrace prd;
        prd.result = make_float3(0.f);
        prd.attenuation = make_float3(1.f);
        prd.countEmitted = true;
        prd.done = false;
        prd.seed = seed;
        prd.depth = 0;
		float3 total_attenuation = make_float3(1, 1, 1);

        // Each iteration is a segment of the ray path.  The closest hit will
        // return new segments to be traced here.
        for(int i = 0; i < 100; i++)
        {
            Ray ray = make_Ray(ray_origin, ray_direction, RADIANCE_RAY_TYPE, scene_epsilon, RT_DEFAULT_MAX);
            rtTrace(top_object, ray, prd);

            if(prd.done)
            {
                // We have hit the background or a luminaire
                prd.result += prd.radiance * total_attenuation;
                break;
            }

			
            // Russian roulette termination 
            if(prd.depth >= rr_begin_depth)
            {
                float pcont = fmaxf(total_attenuation );
                if(rnd(prd.seed) >= pcont)
                    break;
               total_attenuation /= pcont;
            }
			

            prd.depth++;
            prd.result += prd.radiance * total_attenuation;

			total_attenuation *= prd.attenuation;

            // Update ray data for the next path segment
            ray_origin = prd.origin;
            ray_direction = prd.direction;
        }

        result += prd.result;
        seed = prd.seed;
    } while (--samples_per_pixel);

    //
    // Update the output buffer
    //
    float3 pixel_color = result/(sqrt_num_samples*sqrt_num_samples);

	//tonemapping
	//tone mapping
	float A = 2.51;
	float B = 0.03;
	float C = 2.43;
	float D = 0.59;
	float E = 0.14;
	float adapted_lumn = 1.0;
	pixel_color *= adapted_lumn;
	pixel_color = (pixel_color * (A * pixel_color + B)) / (pixel_color * (C * pixel_color + D) + E);

	pixel_color = fmaxf(pixel_color, make_float3(0.f));
	pixel_color.x = fmaxf(1.055 * powf(pixel_color.x, 0.416666667) - 0.055f, 0.f);
	pixel_color.y = fmaxf(1.055 * powf(pixel_color.y, 0.416666667) - 0.055f, 0.f);
	pixel_color.z = fmaxf(1.055 * powf(pixel_color.z, 0.416666667) - 0.055f, 0.f);


    if (frame_number > 1)
    {
        float a = 1.0f / (float)frame_number;
        float3 old_color = make_float3(output_buffer[launch_index]);
        output_buffer[launch_index] = make_float4( lerp( old_color, pixel_color, a ), 1.0f );
    }
    else
    {	
        output_buffer[launch_index] = make_float4(pixel_color, 1.0f);
    }
}


//-----------------------------------------------------------------------------
//
//  Emissive surface closest-hit
//
//-----------------------------------------------------------------------------

/*
rtDeclareVariable(float3,        emission_color, , );

RT_PROGRAM void diffuseEmitter()
{
    current_prd.radiance = current_prd.countEmitted ? emission_color : make_float3(0.f);
    current_prd.done = true;
}
*/

//-----------------------------------------------------------------------------
//
//  Lambertian surface closest-hit
//
//-----------------------------------------------------------------------------

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
  
RT_PROGRAM void disneyBRDF()
{	

	//normal
    //float3 ns = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, shading_normal ) );
   // float3 ng = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, geometric_normal ) );
    //float3 ffnormal = faceforward( ns, -ray.direction, ng );
	//optix::Onb onb( ffnormal );

	//hit point
    float3 hitpoint = ray.origin + t_hit * ray.direction;

	//bump
	//none

	//material
	RtMaterial mat = materials[primitive_mat_ind];
	
	//texture
	float2 uv = make_float2(texcoord.x, texcoord.y);
	uv = uv * mat.scale + mat.offset;

	float4 albedoBytes = tex2D(albedoTex, uv.x, uv.y);
	float3 albedo = make_float3(
		albedoBytes.x ,
		albedoBytes.y ,
		albedoBytes.z
	);
	float4 normalBytes = tex2D(normalTex, uv.x, uv.y);
	float3 normal = make_float3(
		normalBytes.x ,
		normalBytes.y ,
		normalBytes.z
	);

	normal = 2 * normal - make_float3(1.f);
	normal = normalize(normal);
	//normal = make_float3(normal.x,normal.z, normal.y);

	//test
    float3 ns = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, shading_normal ) );
    float3 ng = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, geometric_normal ) );
    float3 ffnormal = faceforward( ns, -ray.direction, ng );
	optix::Onb onb( ffnormal );

	ns = normalize(LocaltoWorld(normal, onb));
	onb = optix::Onb(ns);

	//test
	//current_prd.attenuation = make_float3(1.0);
	//current_prd.radiance =  ns;
	//current_prd.countEmitted = false;
	//current_prd.done = true;
	//return;

	float4 mraBytes = tex2D(mraTex, uv.x, uv.y);
	float3 mra = make_float3(
		mraBytes.x ,
		mraBytes.y ,
		mraBytes.z
	);

		
	//diffuse
	float3 c = mat.colorFactor * albedo;
	float metallicWeight = mat.metallicFactor.x * mra.x;
	float e = mat.eta.x;	//index of refraction
	float strans = mat.strans.x;	// specular transmitted
	float diffuseWeight = (1.f - metallicWeight) * (1.f - strans);
	float dt = mat.dt.x;	// diffuse transmitted
	float rough = mat.roughnessFactor.x * mra.y;
	float lum = luminance(c);
	float3 cTint = lum > 0 ? (c / lum ) : make_float3(1.f, 1.f, 1.f);

	//sheen
	float sheenWeight = mat.sheenWeight.x;
	float sTint = mat.sTint.x;
	float3 cSheen = lerp(make_float3(1 , 1, 1), cTint, sTint);

	//thin and ss Color
	float flat =  mat.flat.x;
	float3 R = diffuseWeight * (1 - flat) * (1 - dt) * c;
	float3 Rss = diffuseWeight * flat * (1 - dt) * c;

	//sheen Color
	float3 Rsheen =  diffuseWeight * sheenWeight * cSheen;

	//microfacet distribution for Trowbridge-Reitz specular
	float anisotropic = mat.anisotropic.x;
	float aspect = sqrtf(1.f - 0.9 * anisotropic);
	float ax = fmaxf(0.001f, rough * rough / aspect);
	float ay = fmaxf(0.001f, rough * rough * aspect);

	//Trowbridge-Reitz specular's constant
	float specTint = mat.specTint.x;
	float3 specMin = lerp(make_float3(1.f), cTint, specTint);
	float3 cSpec0 = lerp(SchlickR0FromEta(e) * specMin, c, metallicWeight);
	//test
	float3 R0 = lerp(specMin * 0.05, c, metallicWeight);

	//Clearcoat
	float cc = mat.cc.x;
	float3 Rcc = make_float3(cc);
	float glossTint = mat.glossTint.x;
	float gloss = lerp(0.1, 0.001, glossTint);
	
	//BTDF
	float3 Rt = strans * 
				make_float3(sqrtf(c.x), sqrtf(c.y), sqrtf(c.z));

	//lambertian transmission
	float3 RLamTrans = dt * c;

	//sample BRDF
	float3 woWorld = normalize(-ray.direction);
	float3 wiWorld = woWorld;
	float pdf = 0;
	float3 f = make_float3(0.f);
	float u0 = rnd(current_prd.seed);
    float u1 = rnd(current_prd.seed);
	float3 wo = normalize(WorldtoLocal(woWorld, onb));
	float3 wi = wo;

	//choose a bxdf:
	//0 - DisneyDiffuse(R|D)
	//1 - DisneyRetro(R|D)
	//2 - MicrofacetReflection(R|G)
	//3 - DisneyFakeSS(R|D)
	//4 - DisneySheen(R|D)
	//5 - DisneyClearcoat(R|G)
	//6 - MicrofacetTransmission(T|G)
	//7 - LambertianTransmission(T|D)
	int matchingComps = 6;

	//test//float u3 = rnd(current_prd.seed);
	//test//int comp = floor(matchingComps * u3);
	int comp = 0;

	//remap random value
	float2 uRemapped = make_float2(u0, u1);

	// sample illumination from light
    unsigned int nLights = dirLights.size();
    float3 Ld = make_float3(0.0f);	
    if(nLights > 0)
	{
		float u4 =rnd(current_prd.seed);
		int lightIndex = min((int)(u4 * nLights), nLights - 1);
		float lightPdf = 1.f / nLights;
		DirectionalLight light = dirLights[lightIndex];
		float3 lightDir;
		float lightDist;

		Ld = EstimateDirect(light, woWorld, wo, ng , ns, 
			R, R0, ax, ay, rough, metallicWeight, e,
			Rss,
			Rsheen,
			Rcc, gloss,
			Rt, 1.f, e,
			RLamTrans,
			matchingComps,lightDir, lightDist, onb ) / lightPdf;

		//shadow-ray test
		PerRayData_pathtrace_shadow shadow_prd;
		shadow_prd.inShadow = false;
		const float3 L = normalize(-lightDir);
		Ray shadow_ray = make_Ray( hitpoint, L, SHADOW_RAY_TYPE, scene_epsilon, lightDist - scene_epsilon );
		rtTrace(top_object, shadow_ray, shadow_prd);
		if(shadow_prd.inShadow)
		{
			Ld = make_float3(0.0f);	
		}
	}
	current_prd.radiance = Ld;

	//sample chosen bxdf
	if(wo.z == 0.f)
	{
		current_prd.attenuation = make_float3(1.0);
		current_prd.radiance = make_float3(0.f);
		current_prd.countEmitted = false;
		current_prd.done = true;
		return;
	}

	//test: layered-sample multi-brdf
	float totalRatio = 1 + cc;
	float baseRatio = 1 / totalRatio;
	float ccRatio = cc / totalRatio;
	float diffuseRatio = 0.5 *(1.f - metallicWeight);
	float specularRatio = 1 - diffuseRatio;	
	float sheenRatio = sheenWeight /(1 + sheenWeight);
	float ssRatio = flat;
	float lambertRatiao = 0.5;
	float retroRatio = 1 - lambertRatiao;
	
	float pdf4 = baseRatio * diffuseRatio * sheenRatio;
	float pdf3 = baseRatio * diffuseRatio * (1 - sheenRatio) * ssRatio;
	float pdf0 = baseRatio * diffuseRatio * (1 - sheenRatio) * (1 - ssRatio) * lambertRatiao;
	float pdf1 = baseRatio * diffuseRatio * (1 - sheenRatio) * (1 - ssRatio) * retroRatio;
	float pdf2 = baseRatio * specularRatio;
	float pdf5 = ccRatio;

	if(rnd(current_prd.seed) < baseRatio)
	{
		if( rnd(current_prd.seed) < diffuseRatio)
		{		
			if( rnd(current_prd.seed) < sheenRatio)
			{
				comp = 4;
			}
			else
			{
				if( rnd(current_prd.seed) < ssRatio)
				{
					comp = 3;
				}
				else
				{
					if( rnd(current_prd.seed) < lambertRatiao)
					{
						comp = 0;
					}
					else
					{
						comp = 1;
					}
				}
			}
		}
		else
		{
			comp = 2;
		}		
	}
	else
	{
		comp = 5;
	}

	//test
	float cosThetaT;
	float stransRatio = 
		1 - fresnelDielectricExt(CosTheta(wo), cosThetaT, e);//0.5 *strans;
	stransRatio *= strans;
	if(rnd(current_prd.seed) < stransRatio)
	{
		comp = 6;
	}

	//sample_choosen_brdf
	if(comp == 0)	//DisneyDiffuse
	{	
		f = DisneyDiffuseSampleF(wo, wi, R, pdf, uRemapped);
	}
	if(comp == 1)	//DisneyRetro
	{
		f = DisneyRetroSampleF(wo, wi, R, rough, pdf, uRemapped);
	}	
	if(comp == 2)	//gloss reflection
 	{	
		f = DisneyMicrofacetReflectionSampleF(
			wo, wi, R, rough, pdf, uRemapped, ax, ay, R0, metallicWeight, e);
	}
	if(comp == 3)	//DisneyFakeSS
	{
		f = DisneyFakeSSSampleF(wo, wi, Rss, rough, pdf, uRemapped);
	}
	if(comp == 4)	//DisneySheen
	{
		f = DisneySheenSampleF(wo, wi, Rsheen, pdf, uRemapped);
	}
	if(comp == 5)	//DisneyClearcoat
	{
		f = DisneyClearcoatSampleF(wo, wi, Rcc, gloss, pdf, uRemapped);
	}
	if(comp == 6)	//MicrofacetTransmission
	{
		f = DisneyMicrofacetTransmissionSampleF(
			wo, wi, 1.f, e,  
			ax, ay, Rt, 
			pdf, uRemapped);
	}
	if(comp == 7)	//LambertianTransmission
	{
		f = LambertianTransmissionSampleF(
			wo, wi, uRemapped, pdf, RLamTrans);
	}
	if(fmaxf(f) == 0.f)
	{
		current_prd.attenuation = make_float3(1.0);
		current_prd.done = true;
		return;
	}
	wiWorld = normalize(LocaltoWorld(wi, onb));

	//Compute overall PDF with all matching _BxDF_spec

	//test
	float transPdf = 0;
	float3 transF = make_float3(0);
	float3 wi2;
	transF = DisneyMicrofacetTransmissionSampleF(
			wo, wi2, 1.f, e,  
			ax, ay, Rt, 
			transPdf, uRemapped);

	pdf = 0.0;
	pdf += DisneyRetroPdf(wo, wi) * pdf0;
	pdf += DisneyDiffusePdf(wo, wi)* pdf1;
	pdf += DisneyMicrofacetReflectionPdf(wo, wi, ax, ay)* pdf2;
	pdf += DisneyFakeSSPdf(wo, wi)* pdf3;
	pdf += DisneySheenPdf(wo, wi)* pdf4;
	pdf += DisneyClearcoatPdf(wo, wi, gloss)* pdf5;

	//test
	pdf = pdf * (1 - stransRatio);
	pdf += transPdf * stransRatio;

	//pdf += LambertianTransmissionPdf(wo, wi);
	//test//pdf /= matchingComps;
	
	// Compute value of BSDF for sampled direction
	f = make_float3(0.0);
	f = DisneyBsdfF(woWorld, wiWorld, 
			wo, wi, 
			ns , R, 
			R0, ax, ay, 
			rough, metallicWeight, e,
			Rss,
			Rsheen,
			Rcc, gloss,
			Rt, 1.f, e,
			RLamTrans);

	//test
	f += transF;
	
	if(pdf > 0.f && (f.x > 0.f || f.y > 0.f || f.z > 0.f))
	{	
		current_prd.attenuation *= f * abs(dot(wiWorld, ns)) / pdf;
		current_prd.origin = hitpoint;
		current_prd.direction = wiWorld;
		current_prd.countEmitted = false;
	}
	else
	{
		current_prd.attenuation = make_float3(1.0);
		current_prd.radiance = make_float3(0.f);
		current_prd.countEmitted = false;
		current_prd.done = true;
		return;
	}
}

RT_PROGRAM void DisneyGGXBRDF()
{	
	//normal
    float3 ns = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, shading_normal ) );
    float3 ng = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, geometric_normal ) );
    float3 ffnormal = faceforward( ns, -ray.direction, ng );
	optix::Onb onb( ffnormal );

	//hit point
    float3 hitpoint = ray.origin + t_hit * ray.direction;

	current_prd.origin = hitpoint;

	//material
	RtMaterial mat = materials[primitive_mat_ind];
	
	//texture
	float4 albedoBytes = tex2D(albedoTex, texcoord.x, texcoord.y);
	float3 albedo = make_float3(
		albedoBytes.x ,
		albedoBytes.y ,
		albedoBytes.z
	);
	float4 normalBytes = tex2D(normalTex, texcoord.x, texcoord.y);
	float3 normal = make_float3(
		normalBytes.x ,
		normalBytes.y ,
		normalBytes.z
	);

	normal = 2 * normal - make_float3(1.f);
	ns = normalize(LocaltoWorld(normal, onb));
	onb = optix::Onb(ns);

	float4 mraBytes = tex2D(mraTex, texcoord.x, texcoord.y);
	float3 mra = make_float3(
		mraBytes.x ,
		mraBytes.y ,
		mraBytes.z
	);

	//diffuse
	float3 c = mat.colorFactor * albedo;
	float metallicWeight = mat.metallicFactor.x * mra.x;
	float e = 1.5f;	//index of refraction
	float strans = 0.f;	// specular transmitted
	float diffuseWeight = (1.f - metallicWeight) * (1.f - strans);
	float dt = 0.f;	// diffuse transmitted
	float rough = mat.roughnessFactor.x * mra.y;
	float lum = luminance(c);
	//normalize lum
	float3 cTint = lum > 0 ? (c / lum ) : make_float3(1.f, 1.f, 1.f);
	float specTint = 0.0f;
	float3 specMin = lerp(make_float3(1.f), cTint, specTint);
	float3 cSpec0 = lerp(SchlickR0FromEta(e) * specMin, c, metallicWeight);

	//test
	cSpec0 = lerp(make_float3(0.05), c, metallicWeight);

	float3 R0 = cSpec0;
	float3 R = diffuseWeight * c;

	//sample para
	float3 woWorld = normalize(-ray.direction);
	float3 wiWorld = woWorld;
	float pdf = 0;
	float3 f = make_float3(0.f);
	float u0 =rnd(current_prd.seed);
    float u1 =rnd(current_prd.seed);
	float2 randomU = make_float2(u0, u1);
	float3 wo = normalize(WorldtoLocal(woWorld, onb));
	float3 wi = wo;

	// sample illumination from light
    unsigned int nLights = dirLights.size();
    float3 Ld = make_float3(0.0f);	
    if(nLights > 0)
	{
		float u3 =rnd(current_prd.seed);
		int lightIndex = min((int)(u3 * nLights), nLights - 1);
		float lightChoosePdf = 1.f / nLights;
		DirectionalLight light = dirLights[lightIndex];

		float3 wiWorldL;
		float3 lightDir;
		float lightDist;
		float lightPdf = 0.f, scatteringPdf = 0;

		float3 Li = LightSampleLi(wiWorldL, lightPdf, light, lightDist);
		float3 wiL = normalize(WorldtoLocal(wiWorldL, onb));
		lightDir = -wiWorldL;

		if(lightPdf > 0)
		{
			float3 fL = DisneyBaseDiffuseF(wo, wiL, R, rough);
			fL += SmithGGXF(wo, wiL, rough, R0);

			Ld += fL * Li *  abs(dot(wiWorldL, ns))/ (lightChoosePdf * lightPdf);
		}

		//shadow-ray test
		PerRayData_pathtrace_shadow shadow_prd;
		shadow_prd.inShadow = false;
		const float3 L = normalize(-lightDir);
		Ray shadow_ray = make_Ray( hitpoint, L, SHADOW_RAY_TYPE, scene_epsilon, lightDist - scene_epsilon );
		rtTrace(top_object, shadow_ray, shadow_prd);
		if(shadow_prd.inShadow)
		{
			Ld = make_float3(0.0f);	
		}
	}
	current_prd.radiance = Ld;

	//sample bxdf for wi direction
	float difProbability = rnd(current_prd.seed);
	float diffuseRatio = 0.5 *(1.f - metallicWeight);

	if (difProbability < diffuseRatio)
	{
		f = DisneyDiffuseSampleF(wo, wi, R, pdf, randomU);
	}
	else
	{
		f = SmithGGXSampleF(wo,wi,R,rough,pdf,randomU,R0);
		if(fmaxf(f) == 0.f)
		{
			current_prd.attenuation = make_float3(1.0);
			current_prd.done = true;
			return;
		}
	}
	wiWorld = normalize(LocaltoWorld(wi, onb));
	current_prd.direction = wiWorld;
	current_prd.origin = current_prd.origin + current_prd.direction * scene_epsilon;
	current_prd.countEmitted = false;

	//pdf
	float specularRatio = 1.f - diffuseRatio;
	float pdfDiff = DisneyDiffusePdf(wo, wi) + DisneyRetroPdf(wo, wi);
	pdfDiff *= 0.5;
	float pdfSpec = SmithGGXPdf(wo, wi, rough);
	pdf = diffuseRatio * pdfDiff + specularRatio * pdfSpec;

	//f
	float3 fDiff = DisneyBaseDiffuseF(wo, wi, R, rough);
	float3 fSpec = SmithGGXF(wo, wi, rough, R0);
	f = fDiff + fSpec;


	if(pdf > 0.0)
	{
		current_prd.attenuation = f * abs(dot(wiWorld, ns)) / pdf;
	}
	else
	{
		current_prd.attenuation = make_float3(1.0);
		current_prd.radiance = make_float3(0.f);
		current_prd.done = true;
		return;
	}

}

//-----------------------------------------------------------------------------
//
//  Shadow any-hit
//
//-----------------------------------------------------------------------------

rtDeclareVariable(PerRayData_pathtrace_shadow, current_prd_shadow, rtPayload, );

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

rtDeclareVariable(float3, bg_color, , );

rtTextureSampler<float4, 2> envmap;
RT_PROGRAM void miss()
{	
	float theta = atan2f( ray.direction.x, ray.direction.z );
	float phi   = M_PIf * 0.5f -  acosf( ray.direction.y );
	float u     = (theta + M_PIf) * (0.5f * M_1_PIf);
	float v     = 0.5f * ( 1.0f + sin(phi) );
	float4 hdrEnv = tex2D(envmap, u, v);
    current_prd.radiance = make_float3(hdrEnv.x, hdrEnv.y, hdrEnv.z);
    current_prd.done = true;
}



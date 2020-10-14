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

rtDeclareVariable(float3,        eye, , );
rtDeclareVariable(float3,        U, , );
rtDeclareVariable(float3,        V, , );
rtDeclareVariable(float3,        W, , );
rtDeclareVariable(float3,        bad_color, , );
rtDeclareVariable(unsigned int,  frame_number, , );
rtDeclareVariable(unsigned int,  sqrt_num_samples, , );
rtDeclareVariable(unsigned int,  rr_begin_depth, , );

rtBuffer<float4, 2>					output_buffer;
rtBuffer<float4, 2>					gradient_dx_buffer;
rtBuffer<float4, 2>					gradient_dy_buffer;
rtBuffer<float4, 2>					gradient_output_buffer;
rtBuffer<float4, 2>					albedo_output_buffer;
rtBuffer<float4, 2>					normal_output_buffer;
rtBuffer<float4, 2>					depth_output_buffer;
rtBuffer<DirectionalLight>			dirLights;
rtBuffer<RtMaterial>					materials;
rtBuffer<unsigned int>						texture_id;
rtDeclareVariable(int,         skybox_id, , );

RT_PROGRAM void pathtrace_camera()
{	
    size_t2 screen = output_buffer.size();
    float2 inv_screen = 1.0f/make_float2(screen) * 2.f;
    float2 jitter_scale = inv_screen / sqrt_num_samples;
	unsigned int samples_per_pixel = sqrt_num_samples*sqrt_num_samples;

	uint2 shiftPixel[4];
	uint2 offsetX = make_uint2(1, 0);
	uint2 offsetY = make_uint2(0, 1);
	uint minIndex = 0;
	//left
	shiftPixel[0] = launch_index -offsetX;
	shiftPixel[0].x = clamp(shiftPixel[0].x, (uint)0, (uint)(screen.x - 1));
	//shiftPixel[0].x = shiftPixel[0].x < minIndex ? minIndex : shiftPixel[0].x;
	//right
	shiftPixel[1] = launch_index + offsetX;
	shiftPixel[1].x = clamp(shiftPixel[1].x, (uint)0, (uint)(screen.x - 1));
	//shiftPixel[1].x = shiftPixel[1].x > (screen.x - 1) ? screen.x - 1 : shiftPixel[1].x;
	//top
	shiftPixel[2] = launch_index + offsetY;
	shiftPixel[2].y = clamp(shiftPixel[2].y, (uint)0, (uint)(screen.y - 1));
	//shiftPixel[2].y = shiftPixel[2].y > (screen.y - 1) ? screen.y - 1 : shiftPixel[2].y;
	//bottom
	shiftPixel[3] = launch_index - offsetY;
	shiftPixel[3].y = clamp(shiftPixel[3].y, (uint)0, (uint)(screen.y - 1));
	//shiftPixel[3].y = shiftPixel[3].y < minIndex ? minIndex : shiftPixel[3].y;
      
	float3 result = make_float3(0.0f);
	Variance albedo;
	Variance normal;
	Variance depth;
	float3 gradients[4];
	float3 shiftResult[4];
	for (int i = 0; i < 4; ++i)
	{	
		gradients[i] = make_float3(0.0f);
		shiftResult[i] = make_float3(0.0f);
	}
	Variance varGradient[4];


    unsigned int seed = tea<16>(screen.x*launch_index.y+launch_index.x, frame_number);
    do 
    {	
        //pixel sample point
        float3 ray_origin = eye;
		float3 ray_direction = RayDifferential(
			samples_per_pixel, sqrt_num_samples, launch_index,
			seed, inv_screen, jitter_scale, U, V, W);

        // Initialze main ray
        GradientPath mainPath;
        InitializeRay(ray_origin, ray_direction, mainPath, seed, true);

		//Initialze shift ray
		GradientPath shiftedPrd[4];
		for (int i = 0; i < 4; ++i) 
		{	
			float3 shift_origin = eye;
			float3 shift_direction =  RayDifferential(
				samples_per_pixel, sqrt_num_samples, shiftPixel[i],
				seed, inv_screen, jitter_scale, U, V, W);

			InitializeRay(shift_origin, shift_direction, shiftedPrd[i], seed + i, false);
			shiftedPrd[i].alphas = (float)sqrt_num_samples / (sqrt_num_samples + sqrt_num_samples);

			//test
			shiftedPrd[i] = mainPath;
			shiftedPrd[i].type = OFFSET_PATH;

		}
		
		bool StopMainPathLoop = false;
		bool StopOffsetPathLoop = false;
		bool isBreak = false;
		int loopCount = 12;
		int doneIndex = loopCount + 1;

		//begin tracing ray
        for(int i = 0; i < loopCount; i++)
        {	
			//store past main path info
			GradientPath lastMainPath = mainPath;

			// main path's intersection from camera
			if (StopMainPathLoop == false)
			{
				Ray ray = make_Ray(
					mainPath.ray.origin, mainPath.ray.direction,
					RADIANCE_RAY_TYPE, scene_epsilon, RT_DEFAULT_MAX);

				rtTrace(top_object, ray, mainPath);

				//main path : if hit env or emitter or turn off
				if (mainPath.hitType == ENV || mainPath.hitType == EMITTER)
				{
					float3 mainContribute = make_float3(0);
					float weight = 0;

					float bsdfPdf = mainPath.bsdfResult.pdf;
					float previousPathPdf = mainPath.pdf / bsdfPdf;
					float emitterPdf = mainPath.emitterPdf;

					float weightNumerator = previousPathPdf * bsdfPdf;
					float weightDenominator = (previousPathPdf * previousPathPdf)
						* ((emitterPdf * emitterPdf) + (bsdfPdf * bsdfPdf));

					weight = weightNumerator / (scene_epsilon + weightDenominator);
					mainContribute = mainPath.throughput *  mainPath.emitterRadiance;

					AddRadiance(mainPath.radiance, mainContribute, weight);
				}

				//main path : if hit diffsue surface, sample direct light
				if(mainPath.hitType != ENV
					&& mainPath.hitType != HIT_NONE
					&& mainPath.hitType != EMITTER
					&& mainPath.light.lumPdf > 0.f 
					&& fmaxf(mainPath.light.lum) > 0.f)
				{	
					float3 mainLumContri = make_float3(0.0);
					float3 shiftLumContri = make_float3(0.0);
					float weight = 0;

					//main path
					float bsdfPdf = mainPath.light.bsdfResult.pdf;
					float lightPdf = mainPath.light.lumPdf;

					float mainWeightNumerator = mainPath.pdf * lightPdf;
					float mainWeightDenominator = (mainPath.pdf * mainPath.pdf) *
						((lightPdf * lightPdf) + (bsdfPdf * bsdfPdf));
					weight = mainWeightNumerator / (scene_epsilon + mainWeightDenominator);

					mainLumContri =
						mainPath.throughput * mainPath.light.bsdfResult.weight * mainPath.light.lum;

					AddRadiance(mainPath.radiance, mainLumContri, weight);
				}

				if (mainPath.done == true)
				{
					StopMainPathLoop = true;
					doneIndex = i;
				}
			}

			if (StopMainPathLoop == false)
			{	
				//main path :  Russian roulette termination 
				if(mainPath.depth >= rr_begin_depth)
				{
					float pcont = fmaxf(mainPath.throughput / mainPath.pdf);

					if(rnd(mainPath.seed) >= pcont)
						break;
				
					mainPath.throughput /= pcont;
				}
			
				//main path : update para
				mainPath.depth++;
				mainPath.throughput *= mainPath.bsdfResult.weight;
				mainPath.pdf *= mainPath.bsdfResult.pdf;
				mainPath.eta *= mainPath.bsdfResult.eta;
			}

			//main path beyond offset path for one step
			if (i == 0) continue;
			
			//offset path store main path and next main path info
			for (int offInd = 0; offInd < 4; offInd++)
			{
				GradientPath& offsetPath = shiftedPrd[offInd];
				if (offsetPath.isAlive == false) continue;

				offsetPath.mainDirectLight = lastMainPath.light;
				offsetPath.mainIntersect = lastMainPath.intersect;
				offsetPath.mainBsdf = lastMainPath.bsdfResult;

				offsetPath.nextMainIntersect = mainPath.intersect;
				offsetPath.nextMainBsdf = mainPath.bsdfResult;

				offsetPath.nextDone = mainPath.done;
				offsetPath.nextIsAlive = mainPath.isAlive;

				offsetPath.mainHitType = lastMainPath.hitType;
				offsetPath.nextMainHitType = mainPath.hitType;
			}

			// offset path intersect
			for (int offInd = 0; offInd < 4; offInd++)
			{	
				GradientPath& offsetPath = shiftedPrd[offInd];
				if (offsetPath.isAlive == false) continue;
			
				Ray ray = make_Ray(
					offsetPath.ray.origin, offsetPath.ray.direction,
					RADIANCE_RAY_TYPE, scene_epsilon, RT_DEFAULT_MAX);

				rtTrace(top_object, ray, offsetPath);
			}

			//offset path hit env or faild trace
			for (int offInd = 0; offInd < 4; offInd++)
			{
				GradientPath& offsetPath = shiftedPrd[offInd];
				
				float baseAlpha = offsetPath.alphas;
				float baseAlphaSqr = baseAlpha * baseAlpha;
				float offsetAlphaSqr = (1.f - baseAlpha) * (1.f - baseAlpha);
				float3 mainContribute = make_float3(0);
				float3 offsetContribute = make_float3(0);
				float weight = 0;

				float bsdfPdf = lastMainPath.bsdfResult.pdf;
				float previousPathPdf = lastMainPath.pdf / bsdfPdf;
				float emitterPdf = lastMainPath.emitterPdf;

				float mainWeightNumerator = previousPathPdf * bsdfPdf;
				float mainWeightDenominator = (previousPathPdf * previousPathPdf)
					* ((emitterPdf * emitterPdf) + (bsdfPdf * bsdfPdf));

				//main path has intensity, but offset path don't have
				if (lastMainPath.done == true && (lastMainPath.hitType == ENV || lastMainPath.hitType == EMITTER))
				{

					if (offsetPath.isAlive == false
						&& offsetPath.connectionState != RAY_NOT_CONNECTED)
					{
						weight = mainWeightNumerator * baseAlpha /
							(scene_epsilon + mainWeightDenominator * baseAlphaSqr);

						mainContribute = lastMainPath.throughput * lastMainPath.emitterRadiance;
						offsetContribute = make_float3(0, 0, 0);
					}

					if (offsetPath.isAlive == false
						&& offsetPath.connectionState == RAY_NOT_CONNECTED)
					{
						weight = 1.f /
							(scene_epsilon + lastMainPath.pdf * baseAlphaSqr);
						
						mainContribute = lastMainPath.throughput * lastMainPath.emitterRadiance;
						offsetContribute = make_float3(0, 0, 0);
						offsetPath.connectionState = RAY_CONNECTED;
					}
				}

				if (offsetPath.isAlive == true && (offsetPath.hitType == ENV || offsetPath.hitType == EMITTER)
					&& offsetPath.connectionState != RAY_NOT_CONNECTED
					)
				{	
					float previousOffsetPathPdf = offsetPath.pdf / offsetPath.bsdfResult.pdf;	
					float offsetWeightDenominator = 
						(previousOffsetPathPdf * previousOffsetPathPdf) *
						((offsetPath.emitterPdf * offsetPath.emitterPdf) 
							+ (offsetPath.bsdfResult.pdf * offsetPath.bsdfResult.pdf));				

					weight = mainWeightNumerator * baseAlpha / 
						(scene_epsilon + offsetWeightDenominator * offsetAlphaSqr + mainWeightDenominator * baseAlphaSqr);
					
					mainContribute = lastMainPath.throughput * lastMainPath.emitterRadiance;
					offsetContribute = offsetPath.throughput * offsetPath.emitterRadiance;

					if (offsetPath.hitType == ENV || offsetPath.hitType == EMITTER)	offsetPath.isAlive = false;
				}

				if (offsetPath.isAlive == true && (offsetPath.hitType == ENV || offsetPath.hitType == EMITTER)
					&& offsetPath.connectionState == RAY_NOT_CONNECTED)
				{
					weight = lastMainPath.pdf * baseAlpha /
						(scene_epsilon + offsetPath.pdf * offsetAlphaSqr + lastMainPath.pdf * baseAlphaSqr);

					mainContribute = lastMainPath.throughput * lastMainPath.emitterRadiance;
					offsetContribute = offsetPath.throughput * offsetPath.emitterRadiance;

					if (offsetPath.hitType == ENV || offsetPath.hitType == EMITTER)	offsetPath.isAlive = false;
					offsetPath.connectionState = RAY_CONNECTED;
				}

				AddGradient(offsetPath.gradient, offsetContribute - mainContribute, weight);
				AddRadiance(offsetPath.radiance, offsetContribute, weight);

				//test
				//if (i == 2)	offsetPath.radiance = offsetPath.throughput * offsetPath.emitterRadiance;
			}
			
			////offset path direct lum for main path
			//for (int offInd = 0; offInd < 4; offInd++)
			//{
			//	GradientPath& offsetPath = shiftedPrd[offInd];

			//	float baseAlpha = offsetPath.alphas;
			//	float baseAlphaSqr = baseAlpha * baseAlpha;
			//	float offsetAlphaSqr = (1.f - baseAlpha) * (1.f - baseAlpha);

			//	float3 mainLumContri = make_float3(0.0);
			//	float3 shiftLumContri = make_float3(0.0);
			//	float weight = 0;

			//	float bsdfPdf = lastMainPath.light.bsdfResult.pdf;
			//	float lightPdf = lastMainPath.light.lumPdf;
			//	float mainWeightNumerator = lastMainPath.pdf * lastMainPath.light.lumPdf;
			//	float mainWeightDenominator = (lastMainPath.pdf * lastMainPath.pdf) *
			//		((lightPdf * lightPdf) + (bsdfPdf * bsdfPdf));

			//	if (offsetPath.isAlive == true)
			//	{	
			//		if (offsetPath.hitType != ENV
			//			&& offsetPath.hitType != HIT_NONE
			//			&& offsetPath.hitType != EMITTER)
			//		{
			//			float mainOpposingCosine = 1;
			//			//dot(offsetPath.mainIntersect.normal, offsetPath.mainDirectLight.dir);
			//			float offsetOpposingCosine = 1;
			//			//dot(offsetPath.intersect.normal, offsetPath.light.dir);
			//			float mainDistSquare = offsetPath.mainDirectLight.dist * offsetPath.mainDirectLight.dist;
			//			float offsetDistSquare = offsetPath.light.dist * offsetPath.light.dist;

			//			float jacobian =
			//				fabs((offsetOpposingCosine * mainDistSquare)) /
			//				(scene_epsilon + fabs(mainOpposingCosine * offsetDistSquare));

			//			float offsetWeightDenominator = (jacobian * offsetPath.pdf) * (jacobian * offsetPath.pdf)
			//				* ((offsetPath.light.lumPdf * offsetPath.light.lumPdf)
			//					+ (offsetPath.light.bsdfResult.pdf * offsetPath.light.bsdfResult.pdf));

			//			weight = mainWeightNumerator * baseAlpha /
			//				(scene_epsilon + offsetWeightDenominator * offsetAlphaSqr
			//					+ mainWeightDenominator * baseAlphaSqr);

			//			shiftLumContri = jacobian * offsetPath.throughput
			//				* offsetPath.light.bsdfResult.weight * offsetPath.light.lum;
			//		}

			//	}
			//	else
			//	{
			//		weight = mainWeightNumerator * baseAlpha /
			//			(scene_epsilon + mainWeightDenominator * baseAlphaSqr);
			//	}

			//	AddGradient(offsetPath.gradient, shiftLumContri - mainLumContri, weight);
			//	AddGradient(offsetPath.radiance, shiftLumContri, weight);
			//}
			
			//offset path update para
			for (int offInd = 0; offInd < 4; offInd++)
			{
				GradientPath& offsetPath = shiftedPrd[offInd];
				if (!offsetPath.isAlive) continue;

				//main path :  Russian roulette termination 
				//if (offsetPath.depth >= rr_begin_depth)
				//{
				//	float pcont = fmaxf(offsetPath.throughput / offsetPath.pdf);

				//	if (rnd(offsetPath.seed) >= pcont)
				//	{
				//		TraceFaild(offsetPath, offsetPath.ray.origin,offsetPath.intersect.normal,offsetPath.intersect.uv);
				//		continue;
				//	}
				//	offsetPath.throughput /= pcont;
				//}

				offsetPath.depth++;
				offsetPath.throughput *= offsetPath.bsdfResult.weight *offsetPath.jacobian;
				offsetPath.pdf *= offsetPath.bsdfResult.pdf * offsetPath.jacobian;
				offsetPath.jacobianDet *= offsetPath.jacobian;
			}
	
			if (isBreak 
				|| doneIndex == 0)
				break;

			if (StopMainPathLoop == true)
			{	
				isBreak = true;
				i = i == 0 ? i : i - 1;
			}
        }

        result += mainPath.radiance;
		albedo.addSample(mainPath.feature.albedo);
		normal.addSample(mainPath.feature.normal);
		depth.addSample(mainPath.feature.depth);

		for (int i = 0; i < 4; i++)
		{
			gradients[i] += shiftedPrd[i].gradient;
			shiftResult[i] += shiftedPrd[i].radiance;
			varGradient[i].addSample(shiftedPrd[i].gradient);	
		}

        seed = mainPath.seed;


    } while (--samples_per_pixel);

    //
    // Update the output buffer
    //
    float3 pixel_color = result/(sqrt_num_samples*sqrt_num_samples);
	float3 albedo_color = albedo.mean;
	float3 normal_color = normal.mean;
	float3 depth_color = depth.mean;

	normal_color = make_float3(
		CameraTransform(eye, U, V, W).inverse() * make_float4(normal_color, 0));
	normal_color = (normal_color + make_float3(1)) * 0.5;
	depth_color /= 1000.f;

	float3 pixel_gradient_left = gradients[0] / (sqrt_num_samples*sqrt_num_samples);
	float3 pixel_gradient_right = gradients[1] / (sqrt_num_samples*sqrt_num_samples);
	float3 pixel_gradient_top = gradients[2] / (sqrt_num_samples*sqrt_num_samples);
	float3 pixel_gradient_button = gradients[3] / (sqrt_num_samples*sqrt_num_samples);

	float3 color_gradient = shiftResult[0] / (sqrt_num_samples*sqrt_num_samples);

	////tone mapping
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

	color_gradient *= adapted_lumn;
	color_gradient = (color_gradient * (A * color_gradient + B)) / (color_gradient * (C * color_gradient + D) + E);

	color_gradient = fmaxf(color_gradient, make_float3(0.f));
	color_gradient.x = fmaxf(1.055 * powf(color_gradient.x, 0.416666667) - 0.055f, 0.f);
	color_gradient.y = fmaxf(1.055 * powf(color_gradient.y, 0.416666667) - 0.055f, 0.f);
	color_gradient.z = fmaxf(1.055 * powf(color_gradient.z, 0.416666667) - 0.055f, 0.f);


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

	albedo_output_buffer[launch_index] = make_float4(albedo_color, 1.0f);
	normal_output_buffer[launch_index] = make_float4(normal_color, 1.0f);
	depth_output_buffer[launch_index] = make_float4(depth_color, 1.0f);

	gradient_dx_buffer[shiftPixel[0]] += -make_float4(pixel_gradient_left, 1.0f);
	gradient_dx_buffer[launch_index] += make_float4(pixel_gradient_right, 1.0f);
	gradient_dy_buffer[shiftPixel[2]] += -make_float4(pixel_gradient_top, 1.0f);
	gradient_dy_buffer[launch_index] += make_float4(pixel_gradient_button, 1.0f);

	//float3 leftVarGradient = 0.5f * varGradient[0].getSampleVarianceOfMean();
	//float3 rightVarGradient = 0.5f * varGradient[1].getSampleVarianceOfMean();
	//float3 topVarGradient = 0.5f * varGradient[2].getSampleVarianceOfMean();
	//float3 bottomVarGradient = 0.5f * varGradient[3].getSampleVarianceOfMean();

	//gradient_dx_buffer[shiftPixel[0]] += make_float4(leftVarGradient, 1.0f);
	//gradient_dx_buffer[launch_index] += make_float4(rightVarGradient, 1.0f);
	//gradient_dy_buffer[shiftPixel[2]] += make_float4(topVarGradient, 1.0f);
	//gradient_dy_buffer[launch_index] += make_float4(bottomVarGradient, 1.0f);

	gradient_output_buffer[launch_index] = make_float4(color_gradient, 1.0f);
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

	//test
    float3 ns = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, shading_normal ) );
    float3 ng = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, geometric_normal ) );
    float3 ffnormal = faceforward( ns, -ray.direction, ng );
	optix::Onb onb( ffnormal );

	ns = normalize(LocaltoWorld(normal, onb));
	onb = optix::Onb(ns);

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

	//save for ngpt
	if (current_prd.depth == 0)
	{
		current_prd.feature.albedo = R;
		current_prd.feature.normal = ns;
		current_prd.feature.depth = make_float3(t_hit);;
	}

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
	int comp = 0;

	//remap random value
	float2 uRemapped = make_float2(u0, u1);

	// sample illumination from light
	unsigned int nLights = dirLights.size();
	float3 Ld = make_float3(0.0f);	
	if(nLights > 0)
	{	
		float u4 = rnd(current_prd.seed);
		int lightIndex = min((int)(u4 * nLights), nLights - 1);
		current_prd.light.sample = make_float2(rnd(current_prd.seed), rnd(current_prd.seed));
		
		if (current_prd.type == OFFSET_PATH)
		{
			lightIndex = current_prd.mainDirectLight.index;
			current_prd.light.sample = current_prd.mainDirectLight.sample;
		}

		float lightPdf = 1.f / nLights;
		DirectionalLight light = dirLights[lightIndex];
	
		Ld = EstimateDirect(light, woWorld, wo, ng , ns, 
			R, R0, ax, ay, rough, metallicWeight, e,
			Rss,
			Rsheen,
			Rcc, gloss,
			Rt, 1.f, e,
			RLamTrans,
			matchingComps,
			current_prd.light,
			onb );

		current_prd.light.index = lightIndex;
		current_prd.light.lumPdf *= lightPdf;
		current_prd.light.lightIntersect.hitPoint = hitpoint - current_prd.light.dir * current_prd.light.dist;

		//shadow-ray test
		PerRayData_pathtrace_shadow shadow_prd;
		shadow_prd.inShadow = false;
		const float3 L = normalize(-current_prd.light.dir);
		Ray shadow_ray = make_Ray( hitpoint, L, SHADOW_RAY_TYPE, scene_epsilon, current_prd.light.dist - scene_epsilon );
		rtTrace(top_object, shadow_ray, shadow_prd);
		if(shadow_prd.inShadow)
		{
			current_prd.light.lum = make_float3(0.0f);
		}
	}

	//sample chosen bxdf
	if(wo.z == 0.f)
	{
		TraceFaild(current_prd, hitpoint, ns, uv);
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
		TraceFaild(current_prd, hitpoint,ns,uv);
		return;
	}
	wiWorld = normalize(LocaltoWorld(wi, onb));

	//hit type
	if (comp == 7)
	{
		current_prd.hitType = SPECULAR;
	}
	else
	{
		current_prd.hitType = DIFFUSE;
	}

	//offset path reconnection along wi
	if (current_prd.type == OFFSET_PATH)
	{	
		//test
		//if (current_prd.nextMainHitType == HIT_NONE)
		//{
		//	TraceFaild(current_prd);
		//	return;
		//}

		if (current_prd.connectionState == RAY_CONNECTED)
		{
			wiWorld = current_prd.mainBsdf.wi;
			wi = normalize(WorldtoLocal(wiWorld, onb));
			current_prd.jacobian = 1.0;
		}
		else if (current_prd.connectionState == RAY_RECENTLY_CONNECTED)
		{
			wiWorld = current_prd.mainBsdf.wi;
			wi = normalize(WorldtoLocal(wiWorld, onb));
			current_prd.jacobian = 1.0;
			current_prd.connectionState = RAY_CONNECTED;
		}
		else
		{	
			//main path and offset path's current and next hit-points are on the non-specular surface
			//use hit-points contruct the next path
			if (current_prd.hitType != SPECULAR
				&& current_prd.mainHitType != SPECULAR
				&& current_prd.nextMainHitType != SPECULAR)
			{
				//check if next hit is env
				if (current_prd.nextMainHitType == ENV)
				{	
					float3 mainWi = normalize(WorldtoLocal(current_prd.mainBsdf.wi, onb));
					EnvironmentConnection(mainWi, wi, current_prd.jacobian);
					wiWorld = normalize(LocaltoWorld(wi, onb));

					//env visible test
					PerRayData_pathtrace_shadow visibleRay;
					visibleRay.inShadow = false;
					const float3 L = wiWorld;
					Ray visible_ray = make_Ray(hitpoint, L, SHADOW_RAY_TYPE, scene_epsilon, 99999.9 - scene_epsilon);
					rtTrace(top_object, visible_ray, visibleRay);
					if (visibleRay.inShadow)
					{
						current_prd.isAlive = false;
						current_prd.done = true;
					}
				}
				//else if (current_prd.nextMainHitType == HIT_NONE // test-meiyong
				//	|| current_prd.mainHitType == HIT_NONE
				//	|| current_prd.hitType == HIT_NONE)
				//{
				//	shiftConnection(current_prd.mainIntersect.hitPoint, current_prd.nextMainIntersect.hitPoint,
				//		current_prd.nextMainIntersect.normal, hitpoint, wiWorld, current_prd.jacobian, scene_epsilon);
				//	wi = normalize(WorldtoLocal(wiWorld, onb));
				//	//env visible test
				//	PerRayData_pathtrace_shadow visibleRay;
				//	visibleRay.inShadow = false;
				//	const float3 L = wiWorld;
				//	Ray visible_ray = make_Ray(hitpoint, L, SHADOW_RAY_TYPE, scene_epsilon, 99999.9 - scene_epsilon);
				//	rtTrace(top_object, visible_ray, visibleRay);
				//	if (visibleRay.inShadow)
				//	{
				//		current_prd.isAlive = false;
				//		current_prd.done = true;
				//	}
				//	current_prd.jacobian = 0;
				//}
				//else if (current_prd.hitType == ENV 
				//	|| current_prd.mainHitType == ENV)
				//{
				//	shiftConnection(current_prd.mainIntersect.hitPoint, current_prd.nextMainIntersect.hitPoint,
				//		current_prd.nextMainIntersect.normal, hitpoint, wiWorld, current_prd.jacobian, scene_epsilon);
				//	wi = normalize(WorldtoLocal(wiWorld, onb));
				//	//env visible test
				//	PerRayData_pathtrace_shadow visibleRay;
				//	visibleRay.inShadow = false;
				//	const float3 L = wiWorld;
				//	Ray visible_ray = make_Ray(hitpoint, L, SHADOW_RAY_TYPE, scene_epsilon, 99999.9 - scene_epsilon);
				//	rtTrace(top_object, visible_ray, visibleRay);
				//	if (visibleRay.inShadow)
				//	{
				//		current_prd.isAlive = false;
				//		current_prd.done = true;
				//	}
				//	current_prd.jacobian = 0;
				//}
				else
				{
					shiftConnection(current_prd.mainIntersect.hitPoint, current_prd.nextMainIntersect.hitPoint,
						current_prd.nextMainIntersect.normal, hitpoint, wiWorld, current_prd.jacobian,scene_epsilon);
					wi = normalize(WorldtoLocal(wiWorld, onb));

					//env visible test
					PerRayData_pathtrace_shadow visibleRay;
					visibleRay.inShadow = false;
					const float3 L = wiWorld;
					float dist = length(hitpoint - current_prd.nextMainIntersect.hitPoint);
					Ray visible_ray = make_Ray(hitpoint, L, SHADOW_RAY_TYPE, scene_epsilon, dist - scene_epsilon);
					rtTrace(top_object, visible_ray, visibleRay);
					if (visibleRay.inShadow)
					{
						current_prd.isAlive = false;
						current_prd.done = true;
					}

					//test
					//float3 mainDir = current_prd.mainIntersect.hitPoint - current_prd.nextMainIntersect.hitPoint;
					//float mainDistSqr = length(mainDir);
					//mainDistSqr *= mainDistSqr;
					//mainDir = normalize(mainDir);
					//float mainCosine = fabs(dot(mainDir, current_prd.nextMainIntersect.normal));

					//float3 shiftDir = hitpoint - current_prd.nextMainIntersect.hitPoint;
					//float shiftDistSqr = length(shiftDir);
					//shiftDistSqr *= shiftDistSqr;
					//shiftDir = normalize(shiftDir);
					//float shiftCosine = fabs(dot(shiftDir, current_prd.nextMainIntersect.normal));

					//if ((mainDistSqr) == 0.f)
					//{
					//	current_prd.jacobian = 1;
					//}
				}
				current_prd.connectionState = RAY_RECENTLY_CONNECTED;
			}
			// main path and offset path hit specular/transmittance surface
			//use current hitpoint's half-vector  contruct the next path
			else
			{	
				float3 mainWo = normalize(WorldtoLocal(current_prd.mainBsdf.wo, onb));
				float3 mainWi = normalize(WorldtoLocal(current_prd.mainBsdf.wi, onb));
				
				float shiftEta = e / 1.f;
				HalfVectorReconnect(mainWo, mainWi, wo, wi,
					current_prd.mainBsdf.eta, shiftEta, current_prd.jacobian, scene_epsilon);
				wiWorld = normalize(LocaltoWorld(wi, onb));
			}
		}
	}

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
	
	if(pdf == 0.f || (f.x == 0.f && f.y == 0.f && f.z == 0.f))
	{	
		TraceFaild(current_prd, hitpoint, ns, uv);
	}
	else
	{
		current_prd.ray.origin = hitpoint;
		current_prd.ray.direction = wiWorld;
		current_prd.countEmitted = false;

		current_prd.intersect.hitPoint = hitpoint;
		current_prd.intersect.normal = ns;
		current_prd.intersect.uv = uv;

		current_prd.bsdfResult.wo = woWorld;
		current_prd.bsdfResult.wi = wiWorld;
		current_prd.bsdfResult.compIndex = comp;
		current_prd.bsdfResult.f = f;
		current_prd.bsdfResult.eta = e / 1.f;
		current_prd.bsdfResult.pdf = pdf;
		current_prd.bsdfResult.weight = f * abs(dot(wiWorld, ns));

		current_prd.emitterRadiance = make_float3(0);
		current_prd.emitterPdf = 0;
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



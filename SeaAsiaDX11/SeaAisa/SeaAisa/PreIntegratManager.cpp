#include "PreIntegratManager.h"

const char* const SAMPLE_NAME = "optixPathTracer";

bool PreIntergratManager::Setup(BasicManager & basicMng, LowLevelRendermanager & renderMng, int mipmapIndex)
{	

	DxScene &scene = basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId];
	DxCamera & cam = scene.cameraList[scene.currentCameraId];

	//setup context
	context = optix::Context::create();

	//setglobal parameter
	context->setRayTypeCount(2);
	context->setEntryPointCount(1);
	context->setStackSize(1800);
	context->setMaxTraceDepth(1);

	//create global varialbe
	context["mipmapIndex"]->setUint(mipmapIndex);

	context["scene_epsilon"]->setFloat(0.0001);
	context["rr_begin_depth"]->setUint(4);
	context["sqrt_num_samples"]->setUint(sqrtNumSamples);
	context["bad_color"]->setFloat(1000000.0f, 0.0f, 1000000.0f); // Super magenta to make sure it doesn't get averaged out in the progressive rendering.
	context["loop_depth"]->setUint(12);
	context["bg_color"]->setFloat(optix::make_float3(0.0f, 0.0f, 0.0f));

	//load skybox
	SkyBox& sky = renderMng.skybox;
	TextureManager& texMng = basicMng.textureManager;
	ID3D11Texture2D** skyTex = &(texMng.dxTexure2D[sky.texIndex]);

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	(*skyTex)->GetDesc(&texDesc);

	envWidth = texDesc.Width;
	envHeight = texDesc.Height;
	envMipmap = texDesc.MipLevels;

	int div = (int)powf(2, mipmapIndex);
	width = envWidth / div;
	height = envHeight / div;

	optix::Buffer buffer;
	buffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT, RT_FORMAT_FLOAT4, width, height);
	context["output_buffer"]->set(buffer);

	ptx = getPtxString(SAMPLE_NAME, "optixPreIntergrat.cu", 0);
	//ptx = getPtxString(SAMPLE_NAME, "optixGradientPathTracerV2.cu", 0);
	context->setRayGenerationProgram(0, context->createProgramFromPTXString(ptx, "pathtrace_camera"));
	context->setExceptionProgram(0, context->createProgramFromPTXString(ptx, "exception"));
	context->setMissProgram(0, context->createProgramFromPTXString(ptx, "miss"));

	return true;
}

void PreIntergratManager::Bake(BasicManager & basicMng, LowLevelRendermanager & renderMng)
{	
	for (int i = 1; i < 6; i++)
	{
		Setup(basicMng, renderMng, i);
		CreateLight(basicMng, renderMng);
		CreateMaterial(basicMng, renderMng);
		CreateTexture(basicMng, renderMng);
		CreatGeometry(basicMng, renderMng);
		updateCamera(basicMng);
		context->launch(0, width, height);
		UpdateTexture(basicMng, renderMng, context["output_buffer"]->getBuffer()->get(), i);
		DestroyContext();
	}
}

void PreIntergratManager::UpdateTexture(
	BasicManager & basicMng, LowLevelRendermanager & renderMng, 
	RTbuffer buffer, int mipmapIndex)
{
	//load skybox
	SkyBox& sky = renderMng.skybox;
	TextureManager& texMng = basicMng.textureManager;
	ID3D11Texture2D** skyTex = &(texMng.dxTexure2D[sky.texIndex]);

	int width, height;
	RTsize buffer_width, buffer_height;

	void* imageData;
	rtBufferMap(buffer, &imageData);

	rtBufferGetSize2D(buffer, &buffer_width, &buffer_height);
	width = static_cast<int>(buffer_width);
	height = static_cast<int>(buffer_height);

	UINT rowPitch = (width) * sizeof(uint32_t) * 4;

	//std::vector<unsigned char> pix(width * height * 3);
	//FIBITMAP * bitMap = FreeImage_Allocate(width, height, 96);
	//FIBITMAP * bitMap = FreeImage_AllocateT(FIT_RGBF, width, height, 96, 32, 32, 32);
	uint32_t* bits = new uint32_t[width * height * 4];

	RTformat buffer_format;
	rtBufferGetFormat(buffer, &buffer_format);

	switch (buffer_format) {

	case RT_FORMAT_FLOAT4:
		// This buffer is upside down

		for (int j = height - 1; j >= 0; --j)
		{
			float* dst = ((float*)bits) + (4 * width*(height - 1 - j));
			float* src = ((float*)imageData) + (4 * width*j);
			for (int i = 0; i < width; i++)
			{
				for (int elem = 0; elem < 3; ++elem) {
					float P;
					P = *src++;
					*dst++ = P;
				}

				// skip alpha
				dst++;
				src++;
			}
		}
		break;

	default:
		fprintf(stderr, "Unrecognized buffer data type or format.\n");
		exit(2);
		break;
	}

	//dxDev.context->UpdateSubresource(dxTexure2D[endTextureId], 0, NULL, textures[endTextureId], rowPitch, 0);
	basicMng.dxDevice.context->UpdateSubresource(*skyTex, mipmapIndex, NULL, bits, rowPitch, 0);

	delete bits;
}

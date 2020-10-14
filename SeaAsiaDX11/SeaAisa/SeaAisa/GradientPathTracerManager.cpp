#include "GradientPathTracerManager.h"

const char* const SAMPLE_NAME = "optixPathTracer";

bool GradientPathTracer::Setup(BasicManager & basicMng, LowLevelRendermanager & renderMng)
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

	unsigned int isComputeGradients = 0;
	if (denoiseType == GraAndELoss || denoiseType == GraAndL1Loss)	isComputeGradients = 1;


	//create global varialbe
	context["scene_epsilon"]->setFloat(sceneEpsilon);
	context["rr_begin_depth"]->setUint(rrBeginDepth);
	context["sqrt_num_samples"]->setUint(sqrtNumSamples);
	context["bad_color"]->setFloat(1000000.0f, 0.0f, 1000000.0f); // Super magenta to make sure it doesn't get averaged out in the progressive rendering.
	context["bg_color"]->setFloat(optix::make_float3(0.0f, 0.0f, 0.0f));
	context["loop_depth"]->setUint(loopCount);
	context["bool_compute_gradients"]->setUint(isComputeGradients);

	width = camWidth;
	height = camHeight;

	optix::Buffer buffer;
	buffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT, RT_FORMAT_FLOAT4, width, height);
	context["output_buffer"]->set(buffer);
	optix::Buffer gradientDx;
	gradientDx = context->createBuffer(RT_BUFFER_INPUT_OUTPUT, RT_FORMAT_FLOAT4, width, height);
	context["gradient_dx_buffer"]->set(gradientDx);
	optix::Buffer gradientDy;
	gradientDy = context->createBuffer(RT_BUFFER_INPUT_OUTPUT, RT_FORMAT_FLOAT4, width, height);
	context["gradient_dy_buffer"]->set(gradientDy);
	optix::Buffer gradientColor;
	gradientColor = context->createBuffer(RT_BUFFER_INPUT_OUTPUT, RT_FORMAT_FLOAT4, width, height);
	context["gradient_output_buffer"]->set(gradientColor);
	optix::Buffer albedoColor;
	albedoColor = context->createBuffer(RT_BUFFER_INPUT_OUTPUT, RT_FORMAT_FLOAT4, width, height);
	context["albedo_output_buffer"]->set(albedoColor);
	optix::Buffer normalColor;
	normalColor = context->createBuffer(RT_BUFFER_INPUT_OUTPUT, RT_FORMAT_FLOAT4, width, height);
	context["normal_output_buffer"]->set(normalColor);
	optix::Buffer depthColor;
	depthColor = context->createBuffer(RT_BUFFER_INPUT_OUTPUT, RT_FORMAT_FLOAT4, width, height);
	context["depth_output_buffer"]->set(depthColor);

	optix::Buffer tonemappedBuffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT, RT_FORMAT_FLOAT4, width, height);
	context["tonemapped_buffer"]->set(tonemappedBuffer);

	ptx = getPtxString(SAMPLE_NAME, "optixGradientPathTracerV2.cu", 0);
	context->setRayGenerationProgram(0, context->createProgramFromPTXString(ptx, "pathtrace_camera"));
	context->setExceptionProgram(0, context->createProgramFromPTXString(ptx, "exception"));
	context->setMissProgram(0, context->createProgramFromPTXString(ptx, "miss"));

	return true;

}

void GradientPathTracer::Bake(BasicManager & basicMng, LowLevelRendermanager & renderMng)
{
	Setup(basicMng, renderMng);
	CreateLight(basicMng, renderMng);
	CreateMaterial(basicMng, renderMng);
	CreateTexture(basicMng, renderMng);
	CreatGeometry(basicMng, renderMng);
	updateCamera(basicMng);
	context->launch(0, width, height);
	//SetupPostprocessing();
	//commandListWithoutDenoiser->execute();
	displayBufferEXR(".\\Scene\\Default\\Default-primal.exr", context["output_buffer"]->getBuffer()->get(), true, EXR);
	displayBufferEXR(".\\Scene\\Default\\Default-dx.exr", context["gradient_dx_buffer"]->getBuffer()->get(), true, EXR);
	displayBufferEXR(".\\Scene\\Default\\Default-dy.exr", context["gradient_dy_buffer"]->getBuffer()->get(), true, EXR);
	displayBufferEXR(".\\Scene\\Default\\Default-albedo.exr", context["albedo_output_buffer"]->getBuffer()->get(), true, EXR);
	displayBufferEXR(".\\Scene\\Default\\Default-normal.exr", context["normal_output_buffer"]->getBuffer()->get(), true, EXR);
	displayBufferEXR(".\\Scene\\Default\\Default-depth.exr", context["depth_output_buffer"]->getBuffer()->get(), true, EXR);
	displayBufferEXR(".\\Scene\\Default\\Default-leftPrimal.exr", context["gradient_output_buffer"]->getBuffer()->get(), true, EXR);
	DestroyContext();
	
	bool isNgpt = false;
	if (denoiseType == GraAndELoss)	isNgpt = system("runNgpt.bat");
	if (denoiseType == ELoss)	isNgpt = system("runNgpt101.bat");
	if (denoiseType == GraAndL1Loss)	isNgpt = system("runNgpt102.bat");
	if (denoiseType == L1Loss)	isNgpt = system("runNgpt103.bat");
}

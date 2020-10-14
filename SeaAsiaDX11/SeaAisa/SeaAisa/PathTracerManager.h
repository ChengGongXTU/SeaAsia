#pragma once
#include"SeeAisa.h"
#include"basicmanager.h"
#include"lowlevelrendermanager.h"
#include"PathTracerLight.h"

enum ImageSaveType
{
	PPM,
	EXR,
};

struct PtxSourceCache
{
	std::map<std::string, std::string *> map;
	
	~PtxSourceCache()
	{
		for (std::map<std::string, std::string *>::const_iterator it = map.begin(); it != map.end(); ++it)
			delete it->second;
	}
	
};

class PathTracerManager
{
public:
	//context
	optix::Context context;
	optix::Buffer buffer;
	const char *ptx;

	int rr_begin_depth;
	int sqrt_num_samples;
	uint32_t       width;
	uint32_t       height;


	//light
	optix::Buffer directionalLightBuffer;
	optix::Buffer pointLightBuffer;
	optix::Buffer spotLightBuffer;

	//material
	const char *ptx2;
	optix::Material diffuse;
	optix::Program diffuse_ch;
	optix::Program diffuse_ah;

	//geometry
	optix::Program pgram_bounding_box;
	optix::Program pgram_intersection;

	//std::vector<optix::GeometryInstance> gis;
	optix::Group shadow_group;
	optix::Group geometry_group;

	//ptx
	PtxSourceCache g_ptxSourceCache;

	//pp
	// Post-processing
	optix::CommandList commandListWithDenoiser;
	optix::CommandList commandListWithoutDenoiser;
	optix::PostprocessingStage tonemapStage;
	optix::PostprocessingStage denoiserStage;
	optix::Buffer denoisedBuffer;
	optix::Buffer emptyBuffer;
	optix::Buffer trainingDataBuffer;
	bool postprocessing_needs_init = true;


	bool Setup(BasicManager &basicMng, LowLevelRendermanager &renderMng);
	const char* getPtxString(const char* sample, const char* filename, const char** log);
	void CreateLight(BasicManager & basicMng, LowLevelRendermanager & renderMng);
	void CreateMaterial(BasicManager & basicMng, LowLevelRendermanager & renderMng);
	void CreateTexture(BasicManager & basicMng, LowLevelRendermanager & renderMng);
	void CreatGeometry(BasicManager & basicMng, LowLevelRendermanager & renderMng);
	void updateCamera(BasicManager& basicMng);
	void SetupPostprocessing();
	void DestroyContext();
	void displayBufferPPM(const char* filename, RTbuffer buffer, bool disable_srgb_conversion, ImageSaveType);
	void displayBufferEXR(const char* filename, RTbuffer buffer, bool disable_srgb_conversion, ImageSaveType type);
	void SavePPM(const unsigned char *Pix, const char *fname, int wid, int hgt, int chan);
	void SaveEXR(const unsigned char *Pix, const char *fname, int wid, int hgt, int chan);

	void Bake(BasicManager &basicMng, LowLevelRendermanager &renderMng);
};
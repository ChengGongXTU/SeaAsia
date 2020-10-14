#pragma once
#pragma once
#include <optixu/optixu_math_namespace.h>   

using namespace optix;

struct Variance
{
	int n;
	float3 mean;
	float3 M2;

	Variance()
	{
		n = 0;
		mean = make_float3(0);
		M2 = make_float3(0);
	}

	__host__ __device__ __inline__ void addSample(float3 sample) {
		++n;
		float3 delta = sample - mean;
		mean += delta / n;
		M2 += delta * (sample - mean);
	}

	__host__ __device__ __inline__ float3 getSampleVariance() const {
		if (n < 2) {
			//Log(EWarn, "Sample variance with too few samples.");
			return make_float3(-1);
		}
		return M2 / (n - 1);
	}

	__host__ __device__ __inline__ float3 getSampleVarianceOfMean() const {
		if (n < 2) {
			//Log(EWarn, "Sample variance with too few samples.");
			return make_float3(-1);
		}
		return M2 / (n*(n - 1));
	}
};

enum PathType {
	MAIN_PATH,
	OFFSET_PATH
};

enum RayConnection {
	RAY_NOT_CONNECTED,      ///< Not yet connected - shifting in progress.
	RAY_RECENTLY_CONNECTED, ///< Connected, but different incoming direction so needs a BSDF evaluation.
	RAY_CONNECTED           ///< Connected, allows using BSDF values from the base path.
};

struct GradientRay
{
	float3 origin;
	float3 direction;
	float t;
};

enum HittingType
{
	EMITTER,
	ENV,
	DIFFUSE,
	SPECULAR,
	HIT_NONE
};

struct GradientBSDFResult
{
	//bssf sample info
	float3 wo;
	float3 wi;
	float eta;
	float pdf;
	float3 weight;
	float3 f;
	int compIndex;
	bool isSpecular;
};

struct GradientIntersection
{
	//geometry info
	float3 hitPoint;
	float3 normal;
	float2 uv;
};

struct DirectSampleLight
{
	float3 lum;
	float lumPdf;
	float3 dir;
	float dist;
	unsigned int index;
	float2 sample;
	GradientBSDFResult bsdfResult;
	GradientIntersection lightIntersect;
};

struct Feature
{
	float3 albedo;
	float3 normal;
	float3 depth;
};

struct GradientPath
{	
	PathType type;

	//path info 
	GradientRay ray;
	int depth;
	int countEmitted;
	int done;

	//light transport atten
	float3 throughput;
	float3 diffuseThroughput;
	float3 noJacobianThroughput;
	float pdf;
	float eta;

	//final result
	float3 radiance;
	float3 diffuseRadiance;
	float3 gradient;

	//random setting
	unsigned int seed;

	//intersection info
	GradientIntersection intersect;

	//light sample info
	DirectSampleLight light;

	//emitter
	float3 emitterRadiance;
	float emitterPdf;

	//bsdf info
	GradientBSDFResult bsdfResult;

	//shift info
	bool isAlive;
	bool isCameraHitRough;
	RayConnection connectionState;

	//jacobian info
	float jacobian;
	float jacobianDet;
	float jacobianSum;
	float jacobianMax;
	float jacobianMin;
	float jacobianInverseSum;
	float jacobianSquareSum;
	float jacobianInverseSquareSum;
	unsigned int jacobianCount;

	//current main path info
	DirectSampleLight mainDirectLight;
	GradientIntersection mainIntersect;
	GradientBSDFResult mainBsdf;

	//next main path info
	GradientIntersection nextMainIntersect;
	GradientBSDFResult nextMainBsdf;
	bool nextDone;
	bool nextIsAlive;

	//offset alphas
	float alphas;

	//offset reconnect info
	HittingType hitType;
	HittingType mainHitType;
	HittingType nextMainHitType;

	//other feature info
	Feature feature;
};

static
__host__ __device__ __inline__ void InitializeRay(
	float3 origin, float3 direction, GradientPath& prd,
	unsigned int seed, bool isMainPath
)
{	
	prd.type = isMainPath ? MAIN_PATH : OFFSET_PATH;

	prd.ray.origin = origin;
	prd.ray.direction = direction;
	prd.ray.t = 99999.9;
	prd.depth = 0;
	prd.countEmitted = 0;
	prd.done = 0;

	prd.throughput = make_float3(1);
	prd.diffuseThroughput = make_float3(1);
	prd.noJacobianThroughput = make_float3(1);
	prd.pdf = 1.0;
	prd.eta = 1.0;

	prd.radiance = make_float3(0);
	prd.diffuseRadiance = make_float3(0);
	prd.gradient = make_float3(0);

	prd.intersect.hitPoint = make_float3(0);
	prd.intersect.normal = make_float3(0);
	prd.intersect.uv = make_float2(0);

	prd.bsdfResult.wo = make_float3(0);
	prd.bsdfResult.wi = make_float3(0);
	prd.bsdfResult.compIndex = 0;
	prd.bsdfResult.f = make_float3(0);
	prd.bsdfResult.eta = 1.0;
	prd.bsdfResult.pdf = 1.0;
	prd.bsdfResult.weight = make_float3(0);
	prd.bsdfResult.isSpecular = false;

	prd.seed = seed;

	prd.isAlive = true;
	prd.isCameraHitRough = false;
	prd.connectionState = RAY_NOT_CONNECTED;

	prd.light.lum = make_float3(0);
	prd.light.dir = make_float3(0);
	prd.light.lumPdf = 0;
	prd.light.index = 0;
	prd.light.sample = make_float2(0.0);

	prd.emitterRadiance = make_float3(0);
	prd.emitterPdf = 0;

	prd.jacobian = 1;
	prd.jacobianDet = 1.0;
	prd.jacobianSum = 0.0;
	prd.jacobianMax = 0.0;
	prd.jacobianMin = 10000000.0;
	prd.jacobianInverseSum = 0.0;
	prd.jacobianSquareSum = 0.0;
	prd.jacobianInverseSquareSum = 0.0;
	prd.jacobianCount = 0;
	prd.jacobianDet = 1.0;

	//main path info
	prd.mainIntersect.hitPoint = make_float3(0);
	prd.mainIntersect.normal = make_float3(0);
	prd.mainIntersect.uv = make_float2(0);

	prd.mainBsdf.wo = make_float3(0);
	prd.mainBsdf.wi = make_float3(0);
	prd.mainBsdf.compIndex = 0;
	prd.mainBsdf.f = make_float3(0);
	prd.mainBsdf.eta = 1.0;
	prd.mainBsdf.pdf = 1.0;
	prd.mainBsdf.weight = make_float3(0);
	prd.mainBsdf.isSpecular = false;

	prd.mainDirectLight.lum = make_float3(0);
	prd.mainDirectLight.dir = make_float3(0);
	prd.mainDirectLight.lumPdf = 0;
	prd.mainDirectLight.index = 0;
	prd.mainDirectLight.sample = make_float2(0.0);

	prd.nextDone = false;
	prd.nextIsAlive = true;

	prd.alphas = 1.0;

	prd.hitType = HIT_NONE;
	prd.nextMainHitType = HIT_NONE;
	prd.mainHitType = HIT_NONE;

	prd.feature.albedo = make_float3(0);
	prd.feature.normal = make_float3(0);
	prd.feature.depth = make_float3(0);
}

static
__host__ __device__ __inline__ void AddRadiance(
	float3 &radiance, float3 contribute, float weight)
{
	radiance += (contribute * weight);
}

static
__host__ __device__ __inline__ void AddGradient(
	float3 &gradient, float3 contribute, float weight)
{
	gradient += contribute * weight;
}

static
__host__ __device__ __inline__ void TraceFaild(GradientPath & path, 
	float3 hitPoint, float3 normal, float2 uv)
{	
	path.ray.origin = hitPoint;
	
	path.intersect.hitPoint = hitPoint;
	path.intersect.normal = normal;
	path.intersect.uv = uv;

	path.isAlive = false;
	path.countEmitted = false;
	path.done = true;

	path.emitterRadiance = make_float3(0);
	path.emitterPdf = 0;

	path.bsdfResult.f = make_float3(0);
	path.bsdfResult.weight = make_float3(0);
	path.bsdfResult.pdf = 1;
	path.bsdfResult.eta = 1;

	path.light.lum = make_float3(0);
	path.light.lumPdf = 0;

	path.hitType = HIT_NONE;
}

static
__host__ __device__ __inline__ void HalfVectorReconnect(
	float3 mainWo, float3 mainWi, float3 shiftWo, float3& shiftWi, 
	float mainEta, float shiftEta, float& jacobian, float sceneEpsilon)
{
	if (CosTheta(mainWi) * CosTheta(mainWo) >= 0.0)
	{
		float3 wh = normalize(mainWo + mainWi);
		shiftWi = 2.0 * dot(shiftWo, wh) * wh - shiftWo;
		shiftWi = normalize(shiftWi);

		float jacobian = dot(shiftWi, wh) / dot(mainWi, wh);
		jacobian = fabs(jacobian);
	}
	else
	{
		float3 mainWh;
		if (CosTheta(mainWo) < 0.0)
		{
			mainWh = -(mainWo * mainEta + mainWi);
		}
		else
		{
			mainWh = -(mainWo + mainWi * mainEta);
		}
		float3 wh = normalize(mainWh);

		shiftWi = Refract(shiftWo, wh, shiftEta);

		float3 shiftWh;
		if (CosTheta(shiftWo) < 0)
		{
			shiftWh = -(shiftWo * shiftEta + shiftWi);
		}
		else
		{
			shiftWh = -(shiftWo + shiftWi * shiftEta);
		}

		float mainWhLenSqr = length(mainWh);
		float shiftWhLenSqr = length(shiftWh);
		float sDivMLen = shiftWhLenSqr / (sceneEpsilon + mainWhLenSqr);

		float mDot = fabs(dot(mainWi, wh));
		float sDot = fabs(dot(shiftWi, wh));
		float mDivSDot = mDot / (sceneEpsilon + sDot);

		jacobian = sDivMLen * mDivSDot;
	}
}

static 
__host__ __device__ __inline__ void EnvironmentConnection(
	float3 mainWi, float3& shiftWi, float& jacobian)
{
	shiftWi = mainWi;
	jacobian = 1;
}

static
__host__ __device__ __inline__ void shiftConnection(
	float3 mainPoint, float3 nextMainPoint, float3 nextMainNormal,
	float3 shiftPoint, float3& shiftWi, float& jacobian, float sceneEpsilon)
{
	float3 mainDir = mainPoint - nextMainPoint;
	float mainDistSqr = length(mainDir);
	mainDistSqr *= mainDistSqr;
	mainDir = normalize(mainDir);

	float3 shiftDir = shiftPoint - nextMainPoint;
	float shiftDistSqr = length(shiftDir);
	shiftDistSqr *= shiftDistSqr;
	shiftDir = normalize(shiftDir);

	shiftWi = -shiftDir;

	float mainCosine = dot(mainDir, nextMainNormal);
	float shiftCosine = dot(shiftDir, nextMainNormal);

	if (fabs(shiftCosine * mainDistSqr) < 0.0001)
	{
		jacobian = 0;
		return;
	}

	jacobian = fabs(shiftCosine * mainDistSqr) / 
		(sceneEpsilon + fabs(mainCosine * shiftDistSqr));
}

static
__host__ __device__ __inline__ float3 RayDifferential(
	unsigned int sampleCount, unsigned int sqrtSampleCount,
	uint2 pixelIndex, unsigned int &seed,
	float2 invScreen, float2 jitterScale, 
	float3 U, float3 V, float3 W)
{
	unsigned int x = sampleCount % sqrtSampleCount;
	unsigned int y = sampleCount % sqrtSampleCount;
	//float2 jitter = make_float2(x - rnd(seed), y - rnd(seed));
	//test
	float2 jitter = make_float2(rnd(seed),rnd(seed)) * sqrtSampleCount;
	
	float2 pixel = (make_float2(pixelIndex)) * invScreen - 1.f;
	float2 d = pixel + jitter * jitterScale;
	return normalize(d.x*U + d.y*V + W);
}

static
__host__ __device__ __inline__ Matrix4x4 CameraTransform(
	float3 pos, float3 right, float3 up, float3 dir)
{	
	Matrix4x4 cameraToWorld = Matrix4x4::fromBasis(
		right, up, -dir, pos
	);
	
	return cameraToWorld;
}
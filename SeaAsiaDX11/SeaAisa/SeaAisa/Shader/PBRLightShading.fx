
// Textures
Texture2D RT0 : register(t0);
Texture2D RT1 : register(t1);
Texture2D RT2 : register(t2);

SamplerState SamLinear0 : register(s0);
SamplerState SamLinear1 : register(s1);
SamplerState SamLinear2 : register(s2);


// Constants
struct DirLight
{
	float4 LightDirection;
	float4 LightColor;
	float4 Up;
	float4 Right;
	float LightIntensity;

};

StructuredBuffer<DirLight> DirLightArray : register(t3);

// Constants
struct PointLight
{
	float4 LightPos;
	float4 LightColor;
	float LightIntensity;
	float LightRange;

};

StructuredBuffer<PointLight> PointLightArray : register(t4);

// Constants
struct SpotLight
{
	float4 LightPos;
	float4 LightColor;
	float4 LightDirection;
	float LightIntensity;
	float LightRange;
	float SpotlightOuterAngles;
	float SpotlightInnerAngles;


};

StructuredBuffer<SpotLight> SpotLightArray : register(t5);

//ibl
Texture2D iblTexMap : register(t6);
SamplerState iblSamLinear : register(s3);

//shadowmapRT
Texture2D shadowmapRT : register(t7);
SamplerState SamLinearShadow : register(s7);

cbuffer LightType : register(b0)
{	
	//0 dir
	//1 point
	//2 spot
	int type;
};

cbuffer CameraParams : register(b1)
{
	float4 CameraPos;
};

cbuffer CubeMipmapLevel : register(b2)
{
	int maxMipmapLevel;
};

// Input/Output structures
struct VS_INPUT
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

PS_INPUT VS(VS_INPUT v)
{
	PS_INPUT o;
	o.pos = float4(v.pos,1.0);
	o.uv = v.tex;
	return o;
}

float f_JsutCause2(in float r, in float r_max)
{	
	float f = 1 - pow(r / r_max, 2);
	f = max(f, 0.0);
	return pow(f, 2);
}

float3 LightDirAndColor(inout float3 dir, in float3 worldPos)
{
	if (type == 0)
	{
		dir = normalize(DirLightArray[0].LightDirection.xyz);
		return DirLightArray[0].LightColor.xyz * DirLightArray[0].LightIntensity;

	}

	if (type == 1)
	{
		dir = worldPos - PointLightArray[0].LightPos.xyz;
		float len = length(dir);
		dir = normalize(dir);
		float f_dist = f_JsutCause2(len, PointLightArray[0].LightRange);
		return PointLightArray[0].LightColor.xyz * PointLightArray[0].LightIntensity * f_dist;

	}

	if (type == 2)
	{
		dir = worldPos - SpotLightArray[0].LightPos.xyz;
		float len = length(dir);
		dir = normalize(dir);
		
		float f_dist = f_JsutCause2(len, SpotLightArray[0].LightRange);
		float cos_s = dot(float4(dir, 0.0), float4(SpotLightArray[0].LightDirection.xyz,0.0));
		float cos_u = cos(SpotLightArray[0].SpotlightOuterAngles * 3.1415 / 180.0);
		float cos_p = cos(SpotLightArray[0].SpotlightInnerAngles * 3.1415 / 180.0);

		float t = (cos_s - cos_u) / (cos_p - cos_u);
		float f_dir_smoothstep = t * t*(3 - 2 * t);

		return SpotLightArray[0].LightColor.xyz * SpotLightArray[0].LightIntensity * f_dist * f_dir_smoothstep;
	}

	return float3(0, 0, 0);

}

half4 PS(PS_INPUT i) : SV_TARGET
{	
	//sample rt info
	float4 rt0 = RT0.Sample(SamLinear0, i.uv);
	float4 rt1 = RT1.Sample(SamLinear1, i.uv);
	float4 rt2 = RT2.Sample(SamLinear2, i.uv);

	float metallic = rt0.w;
	float roughness = max(rt1.w, 0.05);
	float specAO = 1.0;
	float3 wPos = rt0.xyz;
	float3 wNormal = normalize(2 * rt1.xyz - float3(1,1,1));
	float3 albedo = rt2.xyz;
	float matMask = rt2.w;
	wNormal *= matMask;

	//view
	float3 viewDir = normalize(CameraPos.xyz - wPos)* matMask;
	
	//Directional light info
	float3 lightDir = float3(0, 0, 1);
	float3 lightCol = clamp(LightDirAndColor(lightDir, wPos) * matMask, 0.0, 10000);
	lightDir = -lightDir;
	float dot_nl = dot(lightDir, wNormal);
	float dot_nl1 = max(dot_nl, 0.0);
	float dot_nv = dot(viewDir, wNormal);
	float dot_nv1 = max(dot_nv, 0.0);
	float3 halfDir = normalize(viewDir + lightDir) * matMask;
	float dot_nh = dot(wNormal, halfDir);
	float dot_nh1 = max(dot_nh, 0.0);
	float dot_lh = dot(lightDir, halfDir);
	float dot_lh1 = max(dot_lh, 0.0001);

	//disney diffuse 2012:
	float3 diffuseCol = lerp(0.022, albedo, 1 - metallic);
	float FD90 = 0.5 + 2 * roughness * dot_lh * dot_lh;
	float Fl = (FD90 - 1) * pow(1 - dot_nl, 5);
	float Fd = (FD90 - 1) * pow(1 - dot_nv, 5);
	float3 diffuse = (diffuseCol / 3.1415) * (1 + Fl) * (1 + Fd);
	
	/*
	//GTR D: metal and anisotropic r = 2 , non_metal and isotropic r = 1
	float a = roughness * roughness;
	float a2 = a * a;

	float r_metal = 2;
	float c_metal = (r_metal - 1) * (a2 - 1) / (1 - pow(a2, 1 - r_metal));
	float D_metal = c_metal / pow(3.1415 * (1 + dot_nh * dot_nh * (a2 - 1)), r_metal);

	float r_nonmetal = 1;
	float c_nonmetal = (a2 - 1) / log(a2);
	float D_nonmetal = c_nonmetal / (3.1415 * (1 + dot_nh * dot_nh * (a2 - 1)));

	float D = lerp(D_nonmetal, D_metal, metallic);

	//F schlick:
	float3 F0 = lerp(0.02, albedo, metallic);
	float3 F = F0 + (1 - F0) * pow(1 - dot_lh, 5);

	//G2: GGX by Walter 
	float ag = pow(0.5 + roughness * 0.5, 2);
	float ag2 = ag * ag;
	float Gl = 2 * dot_nl / (dot_nl1 + sqrt(ag2 + (1 - ag2) * pow(dot_nl, 2)));
	float Gv = 2 * dot_nv / (dot_nv1 + sqrt(ag2 + (1 - ag2) * pow(dot_nv, 2)));
	float G2 = Gl * Gv;
	*/

	//unreal pbr specular :
	//F schlick:
	float3 F0 = lerp(0.02, albedo, metallic);
	float3 F = F0 + (1 - F0) * pow(1 - dot_lh, 5);
	
	//GGX DNF:
	float a = roughness * roughness;
	float a2 = a * a;
	float D = a2 / (3.1415 * pow(dot_nh * dot_nh * (a2 - 1) + 1, 2));

	//GGX Geometry
	float k = pow(roughness + 1, 2) / 8;
	float Gv = dot_nv1 / (dot_nv1 * (1 - k) + k);
	float Gl = dot_nl1 / (dot_nl1 * (1 - k) + k);
	float G2 = Gv * Gl;

	float3 specular = D * G2 * F / (4 * dot_nl1 * dot_nv1);

	//f(l,v)
	diffuse = max(diffuse, 0.0);
	specular = max(specular, 0.0);
	float3 f_lv = diffuse + specular * specAO;
	//direct illumination
	float3 L_direct = f_lv * dot_nl1 * lightCol;
	L_direct = min(L_direct, 10000);
	
	//shadowRT
	float4 shadow = shadowmapRT.Sample(SamLinearShadow, i.uv);
	L_direct = L_direct * shadow.x;

	//IBL£º OP2 approx publish in siggraph 2013
	half3 L_indirect = half3(0, 0, 0);

	if (type == 4)
	{
		half4 c0 = half4( -1.0, -0.0275, -0.572, 0.022);
		half4 c1 = half4( 1.0, 0.0425, 1.04, -0.04);
		half4 iblRoughness = roughness * c0 + c1;
		half a004 = min(iblRoughness.x * iblRoughness.x, exp2(-9.28 * dot_nv1)) * iblRoughness.x + iblRoughness.y;
		half2 AB = half2(-1.04, 1.04) * a004 + iblRoughness.zw;
		half3 envBRDF = F0 * AB.x + AB.y;
		float3 R = normalize(-viewDir - 2 * dot(-viewDir, wNormal)*wNormal);

		//iblUV
		float PI = 3.141592653589;
		float inv_PI = 1.0 / PI;
		float theta = atan2(R.x, R.z);
		float phi = PI * 0.5 - acos(R.y);
		float Ru = (theta + PI) * (0.5 * inv_PI);
		float Rv = 0.5 * (1.0 + sin(phi));
		float2 Ruv = float2(Ru, Rv);

		half4 iblCol = iblTexMap.SampleLevel(iblSamLinear, Ruv, roughness * 8);
		half3 spec_Indirect = iblCol.xyz * envBRDF * matMask;
		L_indirect += spec_Indirect;

		half4 iblHemisphereCol = iblTexMap.SampleLevel(iblSamLinear, Ruv, 8);
		half3 diffuse_Indirect = diffuseCol * iblHemisphereCol.xyz;
		L_indirect += diffuse_Indirect;
	}

	half3 finalCol = L_direct + L_indirect;

	return half4(finalCol.xyz, 1.0);


}

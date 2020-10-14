// Constants
cbuffer wolrdTransform : register(b0)
{
	matrix mWorld;
	//matrix mInverseWorld;
};

cbuffer viewTransform : register(b1)
{
	matrix mView;
	matrix mView2;
};


cbuffer projectionTransform : register(b2)
{
	matrix mProjection;
};

cbuffer wolrdTransform : register(b3)
{
	matrix mWorldforNormal;
	//matrix mInverseWorld;
};

cbuffer lightMatrix : register(b4)
{
	matrix lightWorld;
	matrix lightView;
	matrix lightProjection;
}

// Textures/Samplers
Texture2D Albedo : register(t0);
SamplerState albedoSamLinear : register(s0);
Texture2D NormalMap : register(t1);
SamplerState normalSamLinear : register(s1);
Texture2D MRA : register(t2);
SamplerState mraSamLinear : register(s2);
Texture2D shadowmap : register(t4);
SamplerState shadowmapSamLinear : register(s4);

struct MaterialParameter 
{
	float4 _ColorFactor;
	float4 _AmbientClor;
	float _MetallicFactor;
	float _RoughnessFactor;
	float _IBLFactor;
	float offsetX;
	float offsetY;
	float scaleX;
	float scaleY;
};

StructuredBuffer<MaterialParameter> currentMaterialParameter : register(t3);


// Input/Output structures
struct VS_INPUT
{
	float4 color : COLOR;
	float3 vertex : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	float3 tangent : TANGENT;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD0;
	float4 t2w0 : TEXCOORD1;
	float4 t2w1 : TEXCOORD2;
	float4 t2w2 : TEXCOORD3;
	float4 lightViewPos :  TEXCOORD4;
};


struct PSOutput
{
	float4 RT0 : SV_Target0;
	float4 RT1 : SV_Target1;
	float4 RT2 : SV_Target2;
	float4 RT3 : SV_Target3;
};

PS_INPUT VS(VS_INPUT v)
{
	PS_INPUT o = (PS_INPUT)0;
	o.color = v.color;
	float4 wPos = mul(float4(v.vertex, 1.f), mWorld);
	o.pos = mul(wPos, mView);
	o.pos = mul(o.pos, mProjection);
	//float3 wNormal = normalize(mul(transpose((float3x3)mWorld),v.normal));
	float3 wNormal = mul((float3x3)mWorldforNormal,v.normal);
	float3 wTangent = normalize(mul(v.tangent, (float3x3)mWorld));
	float3 wBiNormal = normalize(cross(wNormal, wTangent));
	o.t2w0 = float4(wTangent.x, wBiNormal.x, wNormal.x, wPos.x);
	o.t2w1 = float4(wTangent.y, wBiNormal.y, wNormal.y, wPos.y);
	o.t2w2 = float4(wTangent.z, wBiNormal.z, wNormal.z, wPos.z);
	o.uv = v.tex;

	//shadow
	o.lightViewPos = mul(wPos, lightView);
	o.lightViewPos = mul(o.lightViewPos, lightProjection);

	return o;
}

PSOutput PS(PS_INPUT i)
{
	PSOutput output = (PSOutput)0;

	//uv
	float2 uv = i.uv *
		float2(currentMaterialParameter[0].scaleX, currentMaterialParameter[0].scaleY) +
		float2(currentMaterialParameter[0].offsetX, currentMaterialParameter[0].offsetY);
	//uv = float2 (uv.x - (int)uv.x, uv.y - (int)uv.y);

	//rt0
	output.RT0.xyz = float3(i.t2w0.w, i.t2w1.w, i.t2w2.w);
	float4 mra = MRA.Sample(mraSamLinear, uv);
	output.RT0.w = mra.x * currentMaterialParameter[0]._MetallicFactor;
	  
	//rt1
	float4 tNormal = 2 * NormalMap.Sample(normalSamLinear, uv) - 1;
	tNormal.z = sqrt(1 - saturate(dot(tNormal.xy, tNormal.xy)));
	float4 wNormal = float4(0, 0, 0, 1);
	wNormal.x = dot(i.t2w0.xyz, tNormal.xyz);
	wNormal.y = dot(i.t2w1.xyz, tNormal.xyz);
	wNormal.z = dot(i.t2w2.xyz, tNormal.xyz);
	wNormal.xyz = normalize(wNormal.xyz);
	output.RT1.xyz = (wNormal.xyz + float3(1,1,1))*0.5;
	output.RT1.w =  mra.y * currentMaterialParameter[0]._RoughnessFactor;

	//rt2
	float4 albedo = Albedo.Sample(albedoSamLinear, uv)* currentMaterialParameter[0]._ColorFactor;
	output.RT2.xyz = albedo.xyz;
	output.RT2.w = 1.0;

	//rt3
	float bias;
	float shadow;
	float2 projectTexCoord;
	float shadowDepth;
	float posDepth;

	// Set the bias value for fixing the floating point precision issues.
	bias = 0.001f;
	shadow = 0.0;

	// Calculate the projected texture coordinates.
	projectTexCoord.x = i.lightViewPos.x / i.lightViewPos.w / 2.0f + 0.5f;
	projectTexCoord.y = -i.lightViewPos.y / i.lightViewPos.w / 2.0f + 0.5f;
	
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) 
		&& (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		shadowDepth = shadowmap.Sample(shadowmapSamLinear, projectTexCoord).x;

		// Calculate the depth of the light.
		posDepth = i.lightViewPos.z / i.lightViewPos.w;

		// Subtract the bias from the lightDepthValue.
		posDepth = posDepth - bias;

		if (posDepth < shadowDepth)
		{
			shadow = 1.0;
		}
	}

	output.RT3.x = shadow;
	output.RT3.y = i.pos.z / i.pos.w;

	return output;

}

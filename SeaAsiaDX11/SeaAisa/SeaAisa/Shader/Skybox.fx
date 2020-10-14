cbuffer cbPerObject: register(b0)
{
	matrix WVP;
};

TextureCube skyboxCubemap : register(t0);
SamplerState skyboxCubemapSampleState: register(s0);

Texture2D skyboxTex : register(t1);
SamplerState skyboxTexSampleState: register(s1);

struct VS_INPUT
{
	float3 vertex : POSITION;
	//float2 uv : TEXCOORD;
	//float3 normal : NORMAL;
};

struct PS_INPUT    //output structure for skymap vertex shader
{
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD;
};


PS_INPUT VS(VS_INPUT v)
{
	PS_INPUT o = (PS_INPUT)0;

	//Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
	o.pos =  mul(float4(v.vertex, 1.0f), WVP).xyww;
	o.texcoord = v.vertex;

	return o;
}

half4 PS(PS_INPUT i) : SV_TARGET
{	
	float PI = 3.141592653589;
	float inv_PI = 1.0 / PI;
	float theta = atan2(i.texcoord.x, i.texcoord.z);
	float phi = PI * 0.5 - acos(i.texcoord.y);
	float u = (theta + PI) * (0.5 * inv_PI);
	float v = 0.5 * (1.0 + sin(phi));
	float2 uv = float2(u,v);
	float4 color1 = skyboxTex.Sample(skyboxTexSampleState, uv);
	float4 color2 = skyboxCubemap.Sample(skyboxCubemapSampleState, i.texcoord);
	return color1;
}


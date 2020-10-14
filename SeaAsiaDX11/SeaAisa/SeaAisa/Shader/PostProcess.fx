
// Textures
Texture2D RT0 : register(t0);
SamplerState SamLinear0 : register(s0);


cbuffer tonemapping : register(b0)
{
	float adapted_lumn;
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

float3 LinearTosRGB(float3 linRGB)
{
	linRGB = max(linRGB, half3(0.h, 0.h, 0.h));
	return max(1.055h * pow(linRGB, 0.416666667h) - 0.055h, 0.h);
}

half4 PS(PS_INPUT i) : SV_TARGET
{
	//sample rt info
	float3 color = RT0.Sample(SamLinear0, i.uv).xyz;

	//tone mapping
	float A = 2.51;
	float B = 0.03;
	float C = 2.43;
	float D = 0.59;
	float E = 0.14;
	color *= adapted_lumn;
	color = (color * (A * color + B)) / (color * (C * color + D) + E);

	//linear to sRGB
	color = LinearTosRGB(color);

	return half4(color,1.0);


}

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer: register(b0)
{
	matrix worldMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer2: register(b1)
{
	float4 lightPosition;	//w = padding
};


//////////////
// TEXTURES //
//////////////
Texture2D depthMapTexture : register(t0);

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeClamp : register(s0);

//////////////
// TYPEDEFS //
//////////////
struct VS_INPUT
{
	float4 color : COLOR;
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	float3 tangent : TANGENT;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	//float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;
	float4 worldPosition;
	float4 inutpPosition;

	// Change the position vector to be 4 units for proper matrix calculations.
	inutpPosition.xyz = input.position.xyz;
	inutpPosition.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(inutpPosition, worldMatrix);
	output.position = mul(output.position, lightViewMatrix);
	output.position = mul(output.position, lightProjectionMatrix);

	//// Calculate the normal vector against the world matrix only.
	//output.normal = mul(input.normal, (float3x3)worldMatrix);

	//// Normalize the normal vector.
	//output.normal = normalize(output.normal);

	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Filename: shadow.ps
////////////////////////////////////////////////////////////////////////////////

float4 PS(PS_INPUT input) : SV_TARGET
{
	float lightDepthValue = input.position.z / input.position.w;
	return float4(lightDepthValue, lightDepthValue, lightDepthValue, 1);
}
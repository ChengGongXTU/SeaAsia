//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer viewTransform : register( b0 )
{
    matrix View;
    matrix View2;
};

cbuffer projectionTransform : register( b1 )
{
    matrix Projection;
};

cbuffer viewTransform : register(b5)
{
    matrix PSView;
    matrix PSView2;
    
};

cbuffer DirLight : register( b2 )
{
    float4 Dir;
    float4 DirLightColor;
};

//cbuffer moveObj : register(b3)
//{
//    matrix World;
//};


cbuffer MaterialParameter:register(b4)
{
    float4 Ka;
    float4 Kd;
    float4 Ks;
    float4 Ke;
    float alpha;
    int illum;
    int Ns;
    float Ni;
};



//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
    matrix view : MATRIX;
    
};

static matrix viewTran = View;
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    viewTran = View;
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(float4(input.Pos,1.f), View);
    output.Pos = mul(output.Pos, Projection);
    output.Normal =normalize(mul(float4(input.Normal,0.f),View));
    output.Tex = input.Tex;
    output.view = View;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    float4 lightdir = -normalize(mul(Dir, input.view));

    float cosA = dot(lightdir, input.Normal);
    if (cosA < 0.f)
        cosA = 0.f;
    if (cosA > 1.f)
        cosA = 1.f;

    float4 eyeVec = normalize(float4(0, 0, 0, 1) - input.Pos);
    float4 H = normalize(lightdir + eyeVec);
    float cosB = dot(H, input.Normal);
    if (cosB < 0)
        cosB = 0;
    if (cosB > 1.f)
        cosB = 1.f;


    if (input.Tex.x == 0.f && input.Tex.y == .0f)
    {
        return Ka + Kd * cosA * DirLightColor + Ks * pow(cosB, Ns) * DirLightColor;
    }
    else
    {
        return Ka + txDiffuse.Sample(samLinear, input.Tex) * cosA * DirLightColor + Ks * pow(cosB, Ns) * DirLightColor;
        

    }
}

struct MATERIAL
{
	float4					m_cAmbient;
	float4					m_cDiffuse;
	float4					m_cSpecular; //a = power
	float4					m_cEmissive;
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	float3		gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	MATERIAL	gMaterial : packoffset(c4);
	uint		gnTexturesMask : packoffset(c8);
};

cbuffer cbFrameworkInfo : register(b4)
{
    float gfCurrentTime : packoffset(c0.x);
    float gfElapsedTime : packoffset(c0.y);
};

#include "Light.hlsl"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define _WITH_VERTEX_LIGHTING

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

Texture2D Default_Texture : register(t0);

Texture2D gtxtStandardTextures[7] : register(t1);
TextureCube gtxtSkyCubeTexture : register(t8);


SamplerState gssWrap : register(s0);
SamplerState gssClamp : register(s1);
SamplerState gssMirror : register(s2);

struct VS_STANDARD_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

struct VS_STANDARD_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGENT;
	float2 uv : TEXCOORD;
};

VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;

	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.tangentW = (float3)mul(float4(input.tangent, 1.0f), gmtxGameObject);
	output.bitangentW = (float3)mul(float4(input.bitangent, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSStandard(VS_STANDARD_OUTPUT input) : SV_TARGET
{
	float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (gnTexturesMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxtStandardTextures[0].Sample(gssWrap, input.uv);
	if (gnTexturesMask & MATERIAL_SPECULAR_MAP) cSpecularColor = gtxtStandardTextures[1].Sample(gssWrap, input.uv);
	if (gnTexturesMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxtStandardTextures[2].Sample(gssWrap, input.uv);
	if (gnTexturesMask & MATERIAL_METALLIC_MAP) cMetallicColor = gtxtStandardTextures[3].Sample(gssWrap, input.uv);
	if (gnTexturesMask & MATERIAL_EMISSION_MAP) cEmissionColor = gtxtStandardTextures[4].Sample(gssWrap, input.uv);


	float4 cIllumination = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 cColor = cAlbedoColor + cSpecularColor + cEmissionColor;
	if (gnTexturesMask & MATERIAL_NORMAL_MAP)
	{
		float3 normalW = input.normalW;
		float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
		float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] → [-1, 1]
		normalW = normalize(mul(vNormal, TBN));
		cIllumination = Lighting(input.positionW, normalW);
		cColor = lerp(cColor, cIllumination, 0.5f);
	}

	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_SKYBOX_CUBEMAP_INPUT
{
	float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3	positionL : POSITION;
	float4	position : SV_POSITION;
};

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
	VS_SKYBOX_CUBEMAP_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.positionL = input.position;

	return(output);
}



float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyCubeTexture.Sample(gssClamp, input.positionL);

	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D<float4> gtxtTerrainTexture[2] : register(t9);

Texture2D<float> gtxtTerrainAlphaTexture : register(t11);

struct VS_TERRAIN_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
    VS_TERRAIN_OUTPUT output;

    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.uv0 = input.uv0;
    output.uv1 = input.uv1;

    return (output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
    float4 cBaseTexColor = gtxtTerrainTexture[0].Sample(gssWrap, input.uv0);
    float4 cDetailTexColor = gtxtTerrainTexture[1].Sample(gssMirror, input.uv1);
    float fAlpha = gtxtTerrainAlphaTexture.Sample(gssWrap, input.uv0);
    float4 cColor = saturate(lerp(cBaseTexColor, cDetailTexColor, fAlpha));

    float4 cIllumination = float4(1.0f, 1.0f, 1.0f, 1.0f);

    cIllumination = Lighting(input.positionW, input.normalW);
    cColor = lerp(cColor, cIllumination, 0.5f);
   
    return (cColor);
}

//================================================================

cbuffer cbScreenInfo : register(b5)
{
    matrix Screen_Transform: packoffset(c0);
};

struct VS_SCREEN_TEXTURED_INPUT
{
    float3 position : SCREEN_POSITION;
    float2 uv : TEXCOORD;
};

struct VS_SCREEN_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_SCREEN_TEXTURED_OUTPUT VSTextureToScreen(VS_SCREEN_TEXTURED_INPUT input)
{
    VS_SCREEN_TEXTURED_OUTPUT output;

    output.position = mul(float4(input.position, 1.0f), Screen_Transform);
    output.uv = input.uv;
	
    return (output);
}

float4 PSTextureToScreen(VS_SCREEN_TEXTURED_OUTPUT input) : SV_TARGET
{
    float waveFrequency = 5.0f; // 파도 주파수
    float waveAmplitude = 0.05f; // 파도 강도

    // 텍스처 샘플링
    float4 cColor = Default_Texture.Sample(gssWrap, input.uv);

    if (cColor.w == 0.0f)
        discard;

    if (cColor.r + cColor.g + cColor.b <= 0.1f)
    {
        cColor.a = 0.8f;
        return (cColor);
    }

	// 파도 효과
    cColor.r += sin(gfCurrentTime * waveFrequency + input.uv.y * 10.0f) * waveAmplitude; 
    cColor.g += sin(gfCurrentTime * waveFrequency + input.uv.y * 10.0f + 1.0f) * waveAmplitude; 
    cColor.b += sin(gfCurrentTime * waveFrequency + input.uv.y * 10.0f + 2.0f) * waveAmplitude; 

	return (cColor);
}

//=============================================================

Texture2D gtxt_Box_Texture[2] : register(t12);

struct VS_FLYING_BOX_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VS_FLYING_BOX_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float3 bitangentW : BITANGENT;
    float2 uv : TEXCOORD;
};

VS_FLYING_BOX_OUTPUT VS_FLYING_BOX(VS_FLYING_BOX_INPUT input)
{
    VS_FLYING_BOX_OUTPUT output;
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.tangentW = (float3) mul(float4(input.tangent, 1.0f), gmtxGameObject);
    output.bitangentW = (float3) mul(float4(input.bitangent, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
    
    return (output);
}

float4 PS_FLYING_BOX(VS_FLYING_BOX_OUTPUT input) : SV_TARGET
{	
    float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

    cAlbedoColor = gtxt_Box_Texture[0].Sample(gssWrap, input.uv);
    cNormalColor = gtxt_Box_Texture[1].Sample(gssWrap, input.uv);


    float4 cIllumination = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 cColor = cAlbedoColor;

    float3 normalW = input.normalW;
    float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
    float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] → [-1, 1]
    normalW = normalize(mul(vNormal, TBN));
    cIllumination = Lighting(input.positionW, normalW);
    cColor = lerp(cColor, cIllumination, 0.5f);
    
    return (cColor);
}


struct VS_INSTANCING_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
};
struct VS_INSTANCING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float2 uv : TEXCOORD0;
};

// 회전 행렬을 정의하는 함수
float4x4 GetRotationMatrix(uint instanceID)
{
    float4x4 rotationMatrix;

    // 기본 행렬 (단위 행렬)
    rotationMatrix = float4x4(1.0f, 0.0f, 0.0f, 0.0f,
                               0.0f, 1.0f, 0.0f, 0.0f,
                               0.0f, 0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);
    if (instanceID == 1)
    {
        // X축 기준으로 45도 회전
        rotationMatrix = float4x4(
            1, 0, 0, 0,
            0, cos(45 * 3.14159265359 / 180), -sin(45 * 3.14159265359 / 180), 0,
            0, sin(45 * 3.14159265359 / 180), cos(45 * 3.14159265359 / 180), 0,
            0, 0, 0, 1
        );
    }
    else if (instanceID == 2)
    {
        // Z축 기준으로 45도 회전
        rotationMatrix = float4x4(
            cos(45 * 3.14159265359 / 180), -sin(45 * 3.14159265359 / 180), 0, 0,
            sin(45 * 3.14159265359 / 180), cos(45 * 3.14159265359 / 180), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
    }

    return rotationMatrix;
}

VS_INSTANCING_OUTPUT VS_Instancing_Asteroid(VS_INSTANCING_INPUT input, uint nInstanceID : SV_InstanceID)
{
    VS_INSTANCING_OUTPUT output;

    float4x4 rotationMatrix = GetRotationMatrix(nInstanceID);
    float4x4 finalTransform = mul(rotationMatrix, gmtxGameObject);

    output.positionW = (float3) mul(float4(input.position, 1.0f), finalTransform);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return output;
}

float4 PS_Instancing_Asteroid(VS_INSTANCING_OUTPUT input) : SV_TARGET
{
    float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
 
    cAlbedoColor = gtxt_Box_Texture[0].Sample(gssWrap, input.uv);
    cNormalColor = gtxt_Box_Texture[1].Sample(gssWrap, input.uv);


    float4 cIllumination = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 cColor = cAlbedoColor;

    float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] → [-1, 1]
    cIllumination = Lighting(input.positionW, vNormal);
    cColor = lerp(cColor, cIllumination, 0.5f);
    
    return (cColor);
}


cbuffer cbOOBBInfo : register(b7)
{
    matrix oobb_worldmatrix : packoffset(c0);
    float4 oobb_line_color : packoffset(c4);
};

struct VS_OOBB_INPUT
{
    float3 position : POSITION;
};

struct VS_OOBB_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OOBB_OUTPUT VS_OOBB(VS_OOBB_INPUT input)
{
    VS_OOBB_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), oobb_worldmatrix), gmtxView), gmtxProjection);
    output.color = oobb_line_color;

    return (output);
}

float4 PS_OOBB(VS_OOBB_OUTPUT input) : SV_TARGET
{
    float4 cColor = input.color;
    return (cColor);
}


struct VS_TEXTURED_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSBillboard(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

float4 PSBillboard(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
    float4 cColor = Default_Texture.SampleLevel(gssWrap, input.uv, 0);


    return (cColor);
}
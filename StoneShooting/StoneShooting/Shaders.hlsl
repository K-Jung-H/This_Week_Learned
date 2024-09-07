//�÷��̾� ��ü�� �����͸� ���� ��� ����
cbuffer cbPlayerInfo : register(b0)
{
    matrix gmtxPlayerWorld : packoffset(c0);
};

//ī�޶� ��ü�� �����͸� ���� ��� ����
cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};
 
//���� ��ü�� �����͸� ���� ��� ����
cbuffer cbGameObjectInfo : register(b2)
{
    matrix gmtxGameObject : packoffset(c0);
};

cbuffer Outline : register(b5)
{
    float4 line_color : packoffset(c0);
    float OutlineThickness : packoffset(c1);
    float padding[3] : packoffset(c2);
};


//========================================================================
// �÷��̾ �׸��� ���̴� 

#include "Light.hlsl"

//���� ���̴��� �Է��� ���� ����ü�� �����Ѵ�. 
struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

//���� ���̴��� ���(�ȼ� ���̴��� �Է�)�� ���� ����ü�� �����Ѵ�. 
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


VS_OUTPUT VSPlayer(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
    output.color = input.color;
    return (output);
}

float4 PSPlayer(VS_OUTPUT input) : SV_TARGET
{
    return (input.color);
}

//========================================================================


// ��ü�� �׸� ��, ���� ������ ���
#define _WITH_VERTEX_LIGHTING

//���� ���̴��� �Է� ���� ����
struct VS_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

//���� ���̴��� ��� ���� ����
struct VS_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    
#ifdef _WITH_VERTEX_LIGHTING
    float4 color : COLOR;
#else
    float3 normalW : NORMAL;
#endif
};

//���� ���̴� �Լ�
VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
    VS_LIGHTING_OUTPUT output;
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    float3 normalW = mul(input.normal, (float3x3) gmtxGameObject);
#ifdef _WITH_VERTEX_LIGHTING
    output.color = Lighting(output.positionW, normalize(normalW));
#else
    output.normalW = normalW;
#endif
    return (output);
}

//�ȼ� ���̴� �Լ�
float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
    return (input.color);
#else
    float3 normalW = normalize(input.normalW);
    float4 color = Lighting(input.positionW, normalW);
    return(color);
#endif
}
//========================================================================

// ������ ���� ���̴�
VS_LIGHTING_OUTPUT VSOutline(VS_LIGHTING_INPUT input)
{
    VS_LIGHTING_OUTPUT output;
    
    // ��� ���͸� ���� �������� Ȯ��
    float4 expandedPosition = float4(input.position + input.normal * OutlineThickness, 1.0f);
    
    // Ȯ��� ��ġ�� ��ȯ
    output.position = mul(mul(mul(expandedPosition, gmtxGameObject), gmtxView), gmtxProjection);
    output.color = line_color;
    return output;
}

// �ȼ� ���̴�
float4 PSOutline(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
    // ������ ���� ����
    return input.color;
}


//========================================================================

Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

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

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);

    return (cColor);
}

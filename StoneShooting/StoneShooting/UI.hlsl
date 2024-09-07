cbuffer ScaleBuffer : register(b0)
{
    float scale; // 1~100 ������ ��
    int fixType; // 1~4�� ������ �����Ǵ� ���� ����
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
    matrix gmtxGameObject : packoffset(c0);
};


// ���� ���̴��� �Է��� ���� ����ü�� ����
struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

// ���� ���̴��� ���(�ȼ� ���̴��� �Է�)�� ���� ����ü�� ����
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


VS_OUTPUT BAR_UI_VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // scale ���� ������ 0.0���� 1.0���� ��ȯ
    float scaledValue = scale / 600.0f;

    float3 scaledPosition = input.position;

    // fixType�� ���� �����Ǵ� �� ����
    if (fixType == 1)  // ��� ����
    {
        if (scaledPosition.y < 0.0f)
        {
            scaledPosition.y = scaledPosition.y - 2 * (scaledPosition.y * (1.0f - scaledValue));
        }
    }
    else if (fixType == 2)  // �ϴ� ����
    {
        if (scaledPosition.y > 0.0f)
        {
            scaledPosition.y = scaledPosition.y - 2 * (scaledPosition.y * (1.0f - scaledValue));
        }
    }
    else if (fixType == 3)  // ���� ����
    {
        if (scaledPosition.x > 0.0f)
        {
            scaledPosition.x = scaledPosition.x - 2 * (scaledPosition.x * (1.0f - scaledValue));
        }
    }
    else if (fixType == 4)  // ���� ����
    {
        if (scaledPosition.x < 0.0f)
        {
            scaledPosition.x = scaledPosition.x - 2 * (scaledPosition.x * (1.0f - scaledValue));
        }
    }
    else // fixType == 0
    {
        // None
    }

    // Ŭ�� �������� ��ȯ
    output.position = mul(mul(float4(scaledPosition, 1.0f), gmtxGameObject), gmtxProjection);
    output.color = input.color;
    return (output);
}

float4 BAR_UI_PS(VS_OUTPUT input) : SV_TARGET
{
    return (input.color);
}
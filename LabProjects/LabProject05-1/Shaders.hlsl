#define FRAME_BUFFER_WIDTH 800.0f
#define FRAME_BUFFER_HEIGHT 600.0f


//정점 쉐이더를 정의한다. 렌더 타겟 사각형 전체를 삼각형 2개로 표현한다. 
float4 VSMain(uint nVertexID : SV_VertexID) : SV_POSITION
{
    float4 output = (float4) 0;
    //프리미티브(삼각형)를 구성하는 정점의 인덱스(SV_VertexID)에 따라 정점을 반환한다.
    //정점의 위치 좌표는 변환이 된 좌표(SV_POSITION)이다. 즉, 투영좌표계의 좌표이다.
    
    if (nVertexID == 0)
        output = float4(-1.0f, +1.0f, 0.0f, 1.0f);
    else if (nVertexID == 1)
        output = float4(+1.0f, +1.0f, 0.0f, 1.0f);
    else if (nVertexID == 2)
        output = float4(+1.0f, -1.0f, 0.0f, 1.0f);
    
    else if (nVertexID == 3)
        output = float4(-1.0f, +1.0f, 0.0f, 1.0f);
    else if (nVertexID == 4)
        output = float4(+1.0f, -1.0f, 0.0f, 1.0f);
    else if (nVertexID == 5)
        output = float4(-1.0f, -1.0f, 0.0f, 1.0f);
    return (output);
}


//픽셀 쉐이더를 다음과 같이 정의하여 실행해 보면 다음과 같은 결과를 볼 수 있다. 
//float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
//{
//    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
//    cColor.r = input.x / FRAME_BUFFER_WIDTH;
//    cColor.g = input.y / FRAME_BUFFER_HEIGHT;
//    cColor.b = 1.0f - input.x / FRAME_BUFFER_WIDTH;
//    return (cColor);
//}

float Rectangle(float2 f2NDC, float fLeft, float fRight, float fTop, float fBottom)
{
    float2 f2Shape = float2(step(fLeft, f2NDC.x), step(f2NDC.x, fRight));
    f2Shape *= float2(step(fTop, f2NDC.y), step(f2NDC.y, fBottom));
    return (f2Shape.x * f2Shape.y);
}

float RegularPolygon(float2 f2NDC, float fSides, float fRadius)
{
    float fAngle = atan(f2NDC.y / f2NDC.x);
    float fSlices = (2.0f * 3.14159f) / fSides;
    float fShape = step(cos(floor((fAngle / fSlices) + 0.5f) * fSlices - fAngle) *
length(f2NDC), fRadius);
    return (fShape);
}
float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
{
    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float2 f2NDC = float2(input.x / FRAME_BUFFER_WIDTH, input.y / FRAME_BUFFER_HEIGHT) -
0.5f; // (0, 1) : (-0.5, 0.5)
    f2NDC.x *= (FRAME_BUFFER_WIDTH / FRAME_BUFFER_HEIGHT);
    cColor.b = RegularPolygon(f2NDC - float2(-0.3f, -0.1f), 8.0f, 0.2f); //4, 6, 8, ...
    cColor.r = RegularPolygon(f2NDC - float2(+0.3f, +0.2f), 4.0f, 0.15f);
    return (cColor);
}
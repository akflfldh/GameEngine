
cbuffer BloomConstantData : register(b0)
{
    uint gWidth;
    uint gHeight;
    float gThreshold;
};

Texture2D<float4> gInputTexture : register(t0);
RWTexture2D<float4> gOutputTexture : register(u0);
SamplerState gLinearSampler : register(s0);

float4 ExtractBrightColor(float4 color)
{
    float brightness = max(color.r, max(color.g, color.b));
    float contribution = max(brightness - gThreshold, 0.0f) / max(brightness, 0.0001f);
    return color * contribution;
}

[numthreads(256, 1, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (dispatchThreadID.x >= gWidth || dispatchThreadID.y >= gHeight)
        return;

    uint inputWidth;
    uint inputHeight;
    gInputTexture.GetDimensions(inputWidth, inputHeight);

    float2 outputSize = float2(gWidth, gHeight);
    float2 uv = (float2(dispatchThreadID.xy) + 0.5f) / outputSize;
    float2 texelSize = 1.0f / float2(gWidth, gHeight);



float weights[11] =
{
    0.008812f,
    0.027144f,
    0.065114f,
    0.121649f,
    0.176998f,
    0.200565f,
    0.176998f,
    0.121649f,
    0.065114f,
    0.027144f,
    0.008812f
};

float2 texOffset[11] =
{
    {-texelSize.x * 5.0f, 0.0f},
    {-texelSize.x * 4.0f, 0.0f},
    {-texelSize.x * 3.0f, 0.0f},
    {-texelSize.x * 2.0f, 0.0f},
    {-texelSize.x * 1.0f, 0.0f},
    { 0.0f,              0.0f},
    { texelSize.x * 1.0f, 0.0f},
    { texelSize.x * 2.0f, 0.0f},
    { texelSize.x * 3.0f, 0.0f},
    { texelSize.x * 4.0f, 0.0f},
    { texelSize.x * 5.0f, 0.0f}
};

    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for(int i=0; i<11; ++i)
    {
        color  +=  ExtractBrightColor(gInputTexture.SampleLevel(gLinearSampler, uv + texOffset[i], 0.0f)) *weights[i] ;


    }


    // float4 color = ExtractBrightColor(gInputTexture.SampleLevel(gLinearSampler, uv, 0.0f)) * 0.5f;
    // color += ExtractBrightColor(gInputTexture.SampleLevel(gLinearSampler, uv - float2(texelSize.x, 0.0f), 0.0f)) *
    //          0.25f;
    // color += ExtractBrightColor(gInputTexture.SampleLevel(gLinearSampler, uv + float2(texelSize.x, 0.0f), 0.0f)) *
    //          0.25f;
    gOutputTexture[dispatchThreadID.xy] = color;
}


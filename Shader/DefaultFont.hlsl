cbuffer PassBuffer : register(b0)
{
    float4x4 gViewProj;
};

struct VertexIn
{
    float2 mPos : POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
    float mPxRange : COMMON;
    float mCommonTwo : COMMON1;
    float mCommonThree : COMMON2;
};

struct VertexOut
{
    float4 mPos : SV_POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
    float mPxRange : COMMON;
};

Texture2D _TexMap : register(t0);
SamplerState _LinearSampler : register(s0);

VertexOut VSMain(VertexIn vin)
{
    VertexOut vout;
    vout.mPos = mul(float4(vin.mPos, 0.0f, 1.0f), gViewProj);
    vout.mTex = vin.mTex;
    vout.mColor = vin.mColor;
    vout.mPxRange = vin.mPxRange;
    return vout;
}

float middle(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

float4 PSMain(VertexOut pin) : SV_Target
{
    float3 msd = _TexMap.Sample(_LinearSampler, pin.mTex).rgb;

    // sd >= 0.5f 글자 안, s< 0.5f 글자 밖  , 0.5f는 다 글자경계
    float sd = middle(msd.r, msd.g, msd.b);

    // 스크린기준 불투명 값  ,  (범위  -0.5 ~ 0.5 로 이동후 PxRange 곱)
    float screenDist = (sd - 0.5f) * pin.mPxRange;

    float opacity = clamp(screenDist + 0.5f, 0.0f, 1.0f);

    float4 color = float4(pin.mColor.xyz, opacity);
    return color;
}
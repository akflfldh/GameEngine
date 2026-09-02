cbuffer PassBuffer:register(b0)
{
    float4x4 gViewProj;
};

struct VertexIn
{
    float2 mPos : POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
    float mCommonOne : COMMON;
    float mCommonTwo : COMMON1;
    float mCommonThree: COMMON2;
};

struct VertexOut
{
    float4 mPos : SV_POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
};

Texture2D _TexMap : register(t0);
SamplerState _LinearSampler : register(s0);



VertexOut VSMain(VertexIn vin)
{
    VertexOut vout;
    vout.mPos = mul(float4(vin.mPos, 0.0f, 1.0f) ,gViewProj); 
    vout.mTex = vin.mTex;
    vout.mColor= vin.mColor;
    return vout;
}


float4 PSMain(VertexOut pin) : SV_Target
{
    float4 color = _TexMap.Sample(_LinearSampler, pin.mTex);


    return color * pin.mColor;
}
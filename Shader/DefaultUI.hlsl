struct VertexIn
{
    float2 mPos : POSITION;
    float2 mTex : TEX;
    float4 mColor : COLOR;
};

struct VertexOut
{
    float4 mPos : SV_POSITION;
    float2 mTex : TEX;
    float4 mColor : COLOR;
};

Texture2D _TexMap : register(t0);
SamplerState _LinearSampler : register(s0);



VertexOut VSMain(VertexIn vin)
{
    VertexOut vout;
    vout.mPos = float4(vin.mPos, 0.0f, 1.0f); // 이미 ClipSpace 좌표임
    vout.mTex = vin.mTex;
    vout.mColor= vin.mColor;
    return vout;
}


float4 PSMain(VertexOut pin) : SV_Target
{
    float4 color = _TexMap.Sample(_LinearSampler, pin.mTex);

    return color;
}
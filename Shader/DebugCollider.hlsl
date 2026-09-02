

cbuffer PassBuffer:register(b0)
{
    float4x4 gViewProj;
};


cbuffer ObjectBuffer :register(b1)
{
    float4x4 gWorld;
    float4 gColor;
};

struct VertexIn
{
    float3 mPosL : POSITION;
    float2 mTex : TEX;  
    float3 mNormal : NORMAL;
};


struct VertexOut
{
    float4 mPosH :SV_POSITION;
};


VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    float4 posW = mul(gWorld,float4(vin.mPosL,1.0F));
    vout.mPosH =mul(posW,gViewProj);

    return vout;
}

float4 PS(VertexOut pin):SV_Target
{
    return gColor;
}
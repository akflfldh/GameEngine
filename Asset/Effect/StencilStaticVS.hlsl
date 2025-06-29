cbuffer ObjectConstant : register(b0)
{
    float4x4 gWorld;
};

cbuffer PassConstant : register(b1)
{
    float4x4 gViewProj;
};


struct VertexIn
{
    float3 mPosL : POSITION;
};


struct VertexOut
{
    float4 mPosH : SV_POSITION;
};


VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    
    float4 posW = mul(float4(vin.mPosL.xyz,1.0f),gWorld);
    vout.mPosH = mul(posW , gViewProj);
    
    return vout;
}


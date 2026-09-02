

cbuffer PassBuffer:register(b0)
{
    float4x4 gViewProj;
};


cbuffer ObjectBuffer :register(b1)
{
    float4x4 gWorld;
    float4 gOutlineColor;
};


struct VertexIn
{
    float3 mPosL : POSITION;
    float2 mTex : TEX;  
    float3 mNormal : NORMAL;   
    float4 mTagent :TANGENT;
};


struct VertexOut
{
    float4 mPosH :SV_POSITION;
};


VertexOut VS_Stencil(VertexIn vin)
{
     VertexOut vout;
    float3 scaledPosL= vin.mPosL * 1.0f;
    float4 posW = mul(gWorld,float4(scaledPosL,1.0F));
    vout.mPosH =mul(posW,gViewProj);

    return vout;
}


VertexOut VS_DrawOutline(VertexIn vin)
{
    VertexOut vout;
    float3 scaledPosL= vin.mPosL * 1.03f;
    float4 posW = mul(gWorld,float4(scaledPosL,1.0F));
    vout.mPosH =mul(posW,gViewProj);

   // vout.mNormal = mul(vin.mNormal, (float3x3)gWorldInvTrans );
    return vout;
}

float4 PS(VertexOut pin):SV_Target
{
    return gOutlineColor;
}
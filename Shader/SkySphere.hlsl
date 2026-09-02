cbuffer PassBuffer:register(b0)
{
    float4x4 gViewProj;
    float3 gEye;
    int gLightNums;
    float4 gAmbientLight; 
       float4x4 gWorld;
};



Texture2D _TexMap :register(t1);

SamplerState _LinearSampler :register(s0);

struct VertexIn
{
    float3 mPosL : POSITION;
    float2 mTex : TEX;  
    float3 mNormal : NORMAL;
    float4 mTangent : TANGENT;
};


struct VertexOut
{
    float4 mPosH :SV_POSITION;
    float3 mPosW :POSITION;
    float2 mTex : TEX;
};



VertexOut VS(VertexIn vin) 
{
    VertexOut vout;

    float4 posW = mul(gWorld , float4(vin.mPosL,1.0f));
    vout.mPosH = mul(posW,gViewProj);
    vout.mPosW =posW;
    vout.mTex =vin.mTex;

return vout;


}


float4 PS(VertexOut pin) :SV_Target
{

    float4 color =_TexMap.Sample(_LinearSampler,pin.mTex.xy);
 
    return color;



}
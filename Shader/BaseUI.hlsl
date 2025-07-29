

cbuffer PassBuffer:register(b0)
{
    float4x4 gViewProj;
};


Texture2D _TexMap :register(t0);

SamplerState _LinearSampler:register(s0);


struct VertexIn
{
    float3 mPosL : POSITION;
    float2 mTex : TEX;  
    float4 mColor : COLOR;
};


struct VertexOut
{
    float4 mPosH :SV_POSITION;
    float2 mTex : TEX;
    float4 mColor : COLOR;
};


VertexOut VS(VertexIn vin)
{

    VertexOut vout;


    vout.mPosH = mul(float4(vin.mPosL,1.0f),gViewProj);
    vout.mTex =vin.mTex;
    vout.mColor= vin.mColor;


    return vout;
}




float4 PS(VertexOut pin) :SV_Target
{
    


    return pin.mColor;

}
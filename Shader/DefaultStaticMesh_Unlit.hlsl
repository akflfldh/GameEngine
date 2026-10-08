

cbuffer MaterialBuffer :register(b2)
{
    float3 gDiffuseFactor;
    float gMetallic;
    float3 gAmbient;
    float gRoughness;
    float3 gEmissiveColor;
    float gEmissiveIntensity; 
    float2 gUVTiling;
    float2 gUVRotationPivot;
    float gUVRotation;
}

Texture2D _TexMap :register(t1);

SamplerState _LinearSampler :register(s0);

struct VertexOut
{
    float4 mPosH :SV_POSITION;
    float3 mPosW :POSITION;
    float2 mTex : TEX;
    float3 mNormal : NORMAL;
    float4 mTangent :TANGENT;
};



float4 PS(VertexOut pin):SV_Target
{
   
    float4 color  =  _TexMap.Sample(_LinearSampler,pin.mTex);
    color *=float4(gDiffuseFactor,1.0f);
    return color;

}



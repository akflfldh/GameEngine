

struct VertexIn
{
    float3 mPos:POSITION ;
    float2 mTex :TEX ;
    float4 mColor :COLOR; 
};


struct VertexOut{

    float4 mPos:SV_POSITION;
    float4 mColor:COLOR;
};


VertexOut VSMain(VertexIn vin)
{
    VertexOut vout;

    vout.mPos = float4(vin.mPos.xyz,1.0f);
    vout.mColor = vin.mColor;

    return vout;
}

float4 PSMain(VertexOut pin ):SV_Target
{
    return pin.mColor;
}




cbuffer PassBuffer:register(b0)
{
    float4x4 gViewProj; 
};



struct VertexIn
{

float4 mPosW :POSITION;
float4 mColor:COLOR;

};


struct VertexOut
{
float4 mPosH :SV_POSITION;
float4 mColor :COLOR;

};



VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.mPosH = mul(vin.mPosW,gViewProj);
    vout.mColor = vin.mColor;
    return vout;

}


float4 PS(VertexOut pin ):SV_Target{

    return pin.mColor;

}


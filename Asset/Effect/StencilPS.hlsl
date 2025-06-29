

struct VertexOut
{
    float4 mPosH :SV_POSITION;
};


float4 PS(VertexOut pin):SV_Target
{
    float4 color =float4(1.0f,1.0f,1.0f,1.0f);
    return color;
}


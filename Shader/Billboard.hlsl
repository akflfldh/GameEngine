


cbuffer PassBuffer :register(b0)
{
    float4x4 gViewProj;
    float3 gEye;
}

struct ObjectData
{
    float4x4 mWorld;
    float2 mSize;
};


Texture2D _TexMap : register(t1);
StructuredBuffer<ObjectData> objectSB : register(t5);

SamplerState _LinearSampler :register(s0);


struct VertexOut{
    float3 mPosW :POSITION;
    float2 mSize :SIZE;
};

struct GeoOut
{
    float4 mPosH :SV_POSITION;
    float3 mPosW : POSITION;
    float2 mTex :TEX;
    float3 mNormal :NORMAL;
};


VertexOut VS(uint id :SV_VertexID)
{
    ObjectData obData = objectSB[id];
    VertexOut vout;

    vout.mPosW = mul(obData.mWorld, float4(0,0,0,1)).xyz;
    vout.mSize = obData.mSize;

    return vout;
}

float3 GetSafeWorldUp(float3 look)
{
    float3 up = float3(0,1,0);
   if( abs(dot(look,up)) >=0.98f)
   {
        return float3(0,0,1);
   }
    return up;
}

[maxvertexcount(4)]
void GS(point VertexOut gin[1] ,
  inout TriangleStream<GeoOut> triStream)
{
    float3 look  = gEye - gin[0].mPosW;
    look = normalize(look);
    float3 worldUp = GetSafeWorldUp(look);
    float3 right = normalize(cross(worldUp,look));
    float3 up = normalize(cross(look,right));

    float halfWidth = gin[0].mSize.x *0.5f;
    float halfHeight = gin[0].mSize.y * 0.5f;

    float3 v[4];
    v[0]  = gin[0].mPosW - halfWidth * right  + halfHeight * up;
    v[1] = gin[0].mPosW + halfWidth * right +halfHeight *up;
    v[2] =gin[0].mPosW - halfWidth * right -halfHeight * up;
    v[3] =gin[0].mPosW + halfWidth * right - halfHeight * up;
    
    
    float2 tex[4]={
        float2(0.0f,0.0f),
        float2(1.0f,0.0f),
        float2(0.0f,1.0f),
        float2(1.0f,1.0f)
    };
    
    GeoOut gout;
    [unroll]
    for(int i=0; i<4; ++i)
    {   
        gout.mPosH = mul(float4(v[i],1.0f),gViewProj);
        gout.mPosW = v[i].xyz;
        gout.mTex = tex[i];
        gout.mNormal = look;
        triStream.Append(gout);
    }
}


float4 PS(GeoOut pin) :SV_Target
{

    float4 color = _TexMap.Sample(_LinearSampler,pin.mTex);

    clip(color.w - 0.1f);

    return color;

}














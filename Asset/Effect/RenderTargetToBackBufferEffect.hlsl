





float gPosX : register(b0);
float gPosY :register(b1);
float gWidth :register(b2);
float gHeight :register(b3);



Texture2D gTexture : register(t0);

SamplerState gSamMinMagMipLinear : register(s0);


struct VertexIn
{
    float3 mPosH :POSITION;
    float2 mTex : TEX;
};



struct VertexOut
{
    float4 mPosH:SV_POSITION;
    float2 mTex: TEX;
};



VertexOut VS(VertexIn vin , uint vertexID : SV_VertexID)
{

     VertexOut vout;

  
        //012 , 213 삼각형 버텍스순서
    // float2 pos[4] ={
    //     float2(gPosX- gWidth/2,gPosY + gHeight/2),
    //     float2(gPosX- gWidth/2, gPosY - gHeight/2),
    //     float2(gPosX + gWidth/2, gPosY + gHeight/2),
    //     float2(gPosX + gWidth/2,gPosY - gHeight/2)
    // };


        float2 pos[4] ={
        float2(-1.0f,1.0f),
        float2(-1.0f, -1.0f),
        float2(1.0f, 1.0f),
        float2(1.0f,-1.0f)
    };

    float2 tex[4]={
        float2(0,0),
        float2(0,1),
        float2(1,0),
        float2(1,1)
    };


   // float2 posH = pos[vertexID] * 2 - 1.0f;

    vout.mPosH = float4(pos[vertexID].xy,1.0f,1.0f);
   vout.mTex = tex[vertexID];    

    return vout;
}




float4 PS(VertexOut pin):SV_Target
{

   float4 color =  gTexture.Sample(gSamMinMagMipLinear,pin.mTex);

    if(color.w <0.001f)
    {
        discard;
    }


return color;

}
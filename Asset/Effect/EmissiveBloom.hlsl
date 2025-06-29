
cbuffer passConstant : register(b0)
{
    float gClientWidth;
    float gClientHeight;
}

Texture2D emissiveMap : register(t0);
Texture2D copiedSceneMap:register(t1);


SamplerState gMinMagMipPointSampler : register(s0);


struct VertexIn
{
    float3 mPosH : POSITION;
    float2 mTex :TEX;
};


struct VertexOut
{

    float4 mPosH :SV_POSITION;
    float2 mTex :Tex;

};


VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.mPosH = float4(vin.mPosH.xyz,1.0f);

    vout.mTex = vin.mTex;
    
    return vout;
}


float4 PS(VertexOut pin):SV_Target
{   

    float texDeltaX = 1.0f/gClientWidth;
    float texDeltaY =1.0F/gClientHeight;

    float2 nearTexCoordinate[9];

    float minTexX = pin.mTex.x - texDeltaX;
    float minTexY = pin.mTex.y - texDeltaY;

    for(int y= 0; y<3; ++y)
    {
        float texY =minTexY + texDeltaY*y;
        for(int x= 0; x<3; ++x)
        {
            nearTexCoordinate[y*3+x].x = minTexX + texDeltaX*x;
            nearTexCoordinate[y*3+x].y = texY;
        }
    }

    float weightArray[9] ={1/16, 2/16, 1/16, 2/16, 4/16 ,2/16 ,1/16 ,2/16 ,1/16};
    
    float4 totalColor = float4(0,0,0,0); 
    for(int i= 0; i<9; ++i)
    {
        float4 emissiveValue = emissiveMap.Sample(gMinMagMipPointSampler,nearTexCoordinate[i]);
        float4 color = copiedSceneMap.Sample(gMinMagMipPointSampler,nearTexCoordinate[i]);
       totalColor+= emissiveValue*color * weightArray[i];

    }

    float4 origianlColor = copiedSceneMap.Sample(gMinMagMipPointSampler,nearTexCoordinate[4]);
    totalColor +=origianlColor;
    totalColor.w =1.0f;


    return totalColor;

}
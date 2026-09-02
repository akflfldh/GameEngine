cbuffer PassBuffer:register(b0)
{
    float4x4 gViewProj;
    float3 gEye;
    float gLightNums;
};


cbuffer ObjectBuffer :register(b1)
{
    float4x4 gWorld;
    float4x4 gWorldInvTrans;
};

cbuffer MaterialBuffer :register(b2)
{
    float gMetallic;
    float gRoughness;   
}


struct LightData
{
    float3 mStrength;
    float mFalloffStart;
    float3 mDirection;
    float mFalloffEnd;
    float3 mPosition;
    float mSpotPower;
    int mLightType;
    float3 mPad1;
};


StructuredBuffer<LightData> gLights :register(t0);


Texture2D _TexMap :register(t1);
Texture2D NormalMap :register(t2);

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
    float3 mNormal : NORMAL;
    float4 mTangent :TANGENT;
};


float3 ComputeLighting(LightData light,float3 albedo ,  float3 posW, float3 normal,float3 toEye)
{
//평행광이라고만 가정

//람베르트 코사인법칙, 분산광만 고려 

    float3 toLight = float3(0,0,0);


    if(light.mLightType == 0)
    {
       toLight  =-light.mDirection;
    }else if(light.mLightType ==1)
    {
        float3 d = light.mPosition - posW; 
        toLight = normalize(d);
        
        light.mStrength *= saturate((light.mFalloffEnd - length(d) )/(light.mFalloffEnd -light.mFalloffStart));
    }else if(light.mLightType ==2)
    {
        float3 d = light.mPosition - posW;
        toLight = normalize(d);

        //각도에따른 빛의세기 
        float k =  pow(max(dot(light.mDirection , toLight),0.0f),light.mSpotPower);

        //거리에 따른 빛의세기 
        float s = saturate((light.mFalloffEnd - d )/(light.mFalloffEnd -light.mFalloffStart));

        light.mStrength *=(k*s);
    }



   float lambertCos =  max(dot(normal,toLight) , 0);

    float3 halfwayVector = normalize((toEye+toLight));

    float3 RF0 = lerp(float3(0.04,0.04,0.04),albedo,gMetallic);
    float3 RF = RF0 + (1-RF0)* pow( 1.0f- dot(halfwayVector,toEye),5);

    float m  = (1.0f- gRoughness) *255.0f;

    //표면거칠기
    
    float3 sr =  ((m+8)/8) *  pow( max(dot(normal , halfwayVector),0.0f),m);


    float3 kd = (1.0f-RF) *(1.0f -gMetallic);

    float3 diffuse = kd *  albedo;
    float3 specular =sr * RF;

    float3  ret =  lambertCos * light.mStrength * (diffuse + specular);

    return ret;


}



VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    float4 posW = mul(gWorld,float4(vin.mPosL,1.0F));
    vout.mPosW = posW.xyz;
    vout.mPosH =mul(posW,gViewProj);

    vout.mTex = vin.mTex;
    vout.mNormal = mul(gWorldInvTrans, float4(vin.mNormal,0.0f));
    vout.mTangent = mul(gWorldInvTrans,float4(vin.mTangent.xyz,0.0f));
    vout.mTangent.w = vin.mTangent.w;

    
    return vout;

}



float4 PS(VertexOut pin):SV_Target
{
    float3 tangentNormal =  NormalMap.Sample(_LinearSampler,pin.mTex).xyz;
    tangentNormal = tangentNormal * 2.0f -1.0f;

    float3 T = normalize(pin.mTangent);
    float3 N = normalize(pin.mNormal);
    float3 B = normalize( cross(N,T)) * pin.mTangent.w;

    float3x3 TBN =float3x3(T,B,N);

    float3 normalWorld=normalize(mul(tangentNormal,TBN));

    float3 finalColor =float3(0,0,0);
    float3 toEye = normalize(gEye - pin.mPosW);
    float4 color  =  _TexMap.Sample(_LinearSampler,pin.mTex);

    for(int i = 0; i < gLightNums; ++i)
    {
        // 라이트의 Position과 Direction은 이미 월드 공간이므로 변환 없이 쾌적하게 계산!
        finalColor += ComputeLighting(gLights[i], color.xyz, pin.mPosW, normalWorld, toEye); 
    }


 
    return float4(finalColor,1,0f);


}



cbuffer PassBuffer:register(b0)
{
    float4x4 gViewProj;
    float3 gEye;
    int gLightNums;
    float4 gAmbientLight; 

    float4x4 mLightViewProj;
    float2 mShadowMapInvSize;
    uint mShadowEnabled;
    float mPadding;
};


cbuffer ObjectBuffer :register(b1)
{
    float4x4 gWorld;
    float4x4 gWorldInvTrans;

    uint gPaletteOffset = 0;
    uint gPaletteCount = 0;

};

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
Texture2D gShadowMap :register(t3);

#if defined(ENABLE_SKINNING)
StructuredBuffer<float4x4> gSkinPalette : register(t5);
#endif

SamplerState _LinearSampler :register(s0);
SamplerComparisonState _ShadowSampler : register(s2);


struct VertexIn
{
    float3 mPosL : POSITION;
    float2 mTex : TEX;  
    float3 mNormal : NORMAL;
    float4 mTangent : TANGENT;
#if defined(ENABLE_SKINNING)
    uint4 mJointIndices0 : BONEINDEX0;
    uint4 mJointIndices1 : BONEINDEX1;
    float4 mJointWeights0 : BONEWEIGHT0;
    float4 mJointWeights1 : BONEWEIGHT1;
#endif
};


struct VertexOut
{
    float4 mPosH :SV_POSITION;
    float3 mPosW :POSITION;
    float2 mTex : TEX;
    float3 mNormal : NORMAL;
    float4 mTangent :TANGENT;
    float4 mShadowPosH : SAHDOW;
};






#define PI 3.14159265F




float CalcShadowFactor(float4 shadowPosH )
{

    if (mShadowEnabled == 0)
      return 1.0f;


    shadowPosH.xyz/=shadowPosH.w;
    
    float depth = shadowPosH.z;

    uint width, height, numMips;
    gShadowMap.GetDimensions(0, width, height, numMips);

    float dx= 1.0f/(float)width;
 


   float2 shadowUV =  shadowPosH.xy * float2(0.5, -0.5f) + 0.5f;




    const float2 offset[9]= {
        float2(-dx,-dx) ,float2(0,dx),float2(dx,dx),
        float2(-dx,0), float2(0,0),float2(dx,0),
        float2(-dx,-dx),float2(0,-dx), float2(dx,-dx)
    };


    if (any(shadowUV < 0.0f) || any(shadowUV > 1.0f) ||
      shadowPosH.z < 0.0f || shadowPosH.z > 1.0f)
  {
      return 1.0f;
  }

  float factor =0.0f;
  for(int i=0; i<9; ++i)
  {
    float2 uv = shadowUV+offset[i];
    factor +=  gShadowMap.SampleCmpLevelZero(_ShadowSampler,uv,depth);
  }
    return factor/9.0f;

}




//BRDF Lambert 

float3 DiffuseLambert(float3 albedo)
{

    return albedo/ PI;

}

//BRDF Cook-Torrance:

float3 GetFresnel(float3 fresnel0 , float3 normal, float3 light)
{
    return fresnel0 + (1.0f- fresnel0) * pow((1.0f-max(0.0f, dot(normal,light))),5);
}



//alpha = r * r ; 
float NDFGGX(float3 normal, float3 halfVector , float alpha)
{
    //GGX 

    // dot (n ,m)은 사전에 걸러진다고 가정 
    float NdotM = dot(normal,halfVector);

    float alpha2 = alpha * alpha; 

    float denom =1 +  NdotM * NdotM * (alpha2-1.0f);

    return  alpha2 / (PI *denom * denom);
}



//높이 상관 마스킹 그림자 함수
float SmithHeightCorrelated(float3 light ,  float3 normal,  float3 toEye ,float alpha)
{

    float NdotV = dot(normal,toEye);
    float NdotL =dot(normal,light);



    float inv_av=  (alpha * sqrt(  max(1.0f- (NdotV * NdotV), 0.0f))) / NdotV; 
    float inv_al=  (alpha * sqrt(max(1.0f- (NdotL * NdotL) , 0.0f)))/ NdotL; 


    float lamdaV = (-1  + sqrt(1+ (inv_av * inv_av)) )/ 2.0f;  
    float lamdaL = (-1  + sqrt(1+(inv_al*inv_al)) )/ 2.0f;  


    return 1.0f/(1+lamdaV+ lamdaL);


}


float3 GetFresnel0(float3 defaultFresnel0, float3 baseColor, float metalic)
{
   return  lerp(defaultFresnel0, baseColor, metalic);

}


float3 SpecularBRDF(float3 light, float3 normal, float3 toEye,float roughness ,float3 fresnel)
{

    /*
    
        F(h,l) * G2(l,v,h) *D(h) / (4 * |dot (n ,l)| *|dot(n,v)| )
    */

    
    
    float alpha = max( roughness * roughness , 0.01f);    //roghness가 0이면 Nan문제가생김.
    float3 halfVector=  normalize(light+toEye);

  //  float3 F =  GetFresnel(fresnel0,halfVector, light);

    float G =  SmithHeightCorrelated(light,normal,toEye,alpha);

    float D = NDFGGX(normal,halfVector,alpha);

    return fresnel * G * D / (4 * dot(normal,light) * dot(normal,toEye));

}


float3 ComputeBRDF(float3 albedo, float3 light, float3 normal, float3 toEye,float roughness ,float metalic) 
{
 
    float NdotL = dot(normal, light);
    float NdotV = dot(normal, toEye);

    // 유효한 상반구에서만 BRDF 계산
    if (NdotL <= 0.0f || NdotV <= 0.0f)
        return float3(0.0f, 0.0f, 0.0f);

    float3 halfVector=  normalize(light+toEye);
 

    float3 fresnel0 = GetFresnel0(float3(0.04f,0.04f,0.04f), albedo,metalic);
       float3 fresnel = GetFresnel(fresnel0, halfVector, light);



    float3 specular =  SpecularBRDF(light, normal, toEye,roughness,fresnel);
    float3 diffuse =(1.0f- fresnel0) * (1.0f- metalic) * DiffuseLambert(albedo); 
                        //들어가는 비율,  흡수되지않고 나오는 비금속 성분 비율  
    return specular+ diffuse;



}




float3 ComputeLight(LightData light,float3 albedo ,  float3 posW, float3 normal,float3 toEye)
{


  

    float3 toLight  =-light.mDirection;
 float lambertCos =  max(dot(normal,toLight) , 0);
  return  lambertCos * light.mStrength *  ComputeBRDF(albedo, toLight, normal, toEye,gRoughness,gMetallic);
    


    // float lambertCos =  max(dot(normal,toLight) , 0);

    // float3 halfwayVector = normalize((toEye+toLight));

    // float3 RF0 = lerp(float3(0.04,0.04,0.04),albedo,gMetallic);
    // float3 RF = RF0 + (1-RF0)* pow( 1.0f- dot(halfwayVector,toEye),5);

    // float m  =max( (1.0f- gRoughness) *255.0f, 1.0f) ;

    // //표면거칠기
    
    // float3 sr =  ((m+8)/8) *  pow( max(dot(normal , halfwayVector),0.0f),m);


    // float3 kd = (1.0f-RF) *(1.0f -gMetallic);

    // float3 diffuse = kd *  albedo;
    // float3 specular =sr * RF;

    // float3  ret =  lambertCos * light.mStrength * (diffuse + specular);

    // return ret;


}


float3 ComputeLighting(LightData light,float3 albedo ,  float3 posW, float3 normal,float3 toEye,float shadowFactor)
{
//평행광이라고만 가정

//람베르트 코사인법칙, 분산광만 고려 

    float3 toLight = float3(0,0,0);


    if(light.mLightType == 0)
    {
     
       return     shadowFactor *  ComputeLight(light,albedo,posW,normal,toEye);

    }else if(light.mLightType ==1)
    {
         float3 d = light.mPosition - posW; 
        float dist = length(d);
        toLight = normalize(d);
        
        light.mDirection = -toLight;
       // light.mStrength *= saturate((light.mFalloffEnd - dist )/(light.mFalloffEnd -light.mFalloffStart));
        float window =  max((1.0f -   pow ((dist/ light.mFalloffEnd),4)),0.0f);
        float win2 = window * window;

        light.mStrength  *= win2;
        light.mStrength *= ((1.0f* 1.0f)/ (dist * dist  + 1.0f * 1.0f));
           return  ComputeLight(light,albedo,posW,normal,toEye);


    }else if(light.mLightType ==2)
    {
        float3 d = light.mPosition - posW;
        toLight = normalize(d);

        //각도에따른 빛의세기 
        float k =  pow(max(dot(light.mDirection , -toLight),0.0f),light.mSpotPower);

        //거리에 따른 빛의세기 
        float s = saturate((light.mFalloffEnd - length(d) )/(light.mFalloffEnd -light.mFalloffStart));

        light.mStrength *=(k*s);
        light.mDirection = -toLight;
        return  ComputeLight(light,albedo,posW,normal,toEye);

        }



//    float lambertCos =  max(dot(normal,toLight) , 0);

//     float3 halfwayVector = normalize((toEye+toLight));

//     float3 RF0 = lerp(float3(0.04,0.04,0.04),albedo,gMetallic);
//     float3 RF = RF0 + (1-RF0)* pow( 1.0f- dot(halfwayVector,toEye),5);

//     float m  =max( (1.0f- gRoughness) *255.0f, 1.0f) ;

//     //표면거칠기
    
//     float3 sr =  ((m+8)/8) *  pow( max(dot(normal , halfwayVector),0.0f),m);


//     float3 kd = (1.0f-RF) *(1.0f -gMetallic);

//     float3 diffuse = kd *  albedo;
//     float3 specular =sr * RF;

//     float3  ret =  lambertCos * light.mStrength * (diffuse + specular);

    return float3(0,0,0);


}


//rotation먼저하고 scale 
float2 TransformUV(float2 uv, float rotation, float2 rotationPivot, float2 scale)
{

    float s;
    float c;
    sincos(radians(rotation),s,c);
    

    float2x2 R =
    {
         c, -s,
         s,  c
    };



    return   scale * ( mul(R, (uv-rotationPivot)) + rotationPivot); 

}

float2x2 GetUVTransformInvMatrix(float rotation , float2 scale)
{

       float s;
    float c;
    sincos(radians(rotation),s,c);
    

    float2x2 R =
    {
         c, -s,
         s,  c
    };
    
    float2x2 S=
    {
        scale.x ,0,
        0,scale.y
    };
    
     float2x2 mat = mul(S,R);
   float det = determinant(mat);
   float invDet = 1.0f / det;


  return float2x2(
         mat._m11, -mat._m01,
        -mat._m10,  mat._m00
    ) * invDet;

}

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    float4 positionLocal = float4(vin.mPosL, 1.0f);
    float3 normalLocal = vin.mNormal;
    float3 tangentLocal = vin.mTangent.xyz;

#if defined(ENABLE_SKINNING)
    // Palette는 mesh bind object space에서 현재 skeleton object space로 변환한다.
    // Component world는 아래에서 한 번만 적용하며, V1은 positive uniform joint scale만 지원한다.
    float4x4 skinMatrix =
        gSkinPalette[vin.mJointIndices0.x+gPaletteOffset] * vin.mJointWeights0.x +
        gSkinPalette[vin.mJointIndices0.y+gPaletteOffset] * vin.mJointWeights0.y +
        gSkinPalette[vin.mJointIndices0.z+gPaletteOffset] * vin.mJointWeights0.z +
        gSkinPalette[vin.mJointIndices0.w+gPaletteOffset] * vin.mJointWeights0.w +
        gSkinPalette[vin.mJointIndices1.x+gPaletteOffset] * vin.mJointWeights1.x +
        gSkinPalette[vin.mJointIndices1.y+gPaletteOffset] * vin.mJointWeights1.y +
        gSkinPalette[vin.mJointIndices1.z+gPaletteOffset] * vin.mJointWeights1.z +
        gSkinPalette[vin.mJointIndices1.w+gPaletteOffset] * vin.mJointWeights1.w;

    positionLocal = mul(skinMatrix, positionLocal);
    normalLocal = mul((float3x3)skinMatrix, normalLocal);
    tangentLocal = mul((float3x3)skinMatrix, tangentLocal);
#endif

    float4 posW = mul(gWorld, positionLocal);
    vout.mPosW = posW.xyz;
    vout.mPosH =mul(posW,gViewProj);
   vout.mShadowPosH = mul(posW,mLightViewProj);

    vout.mTex = TransformUV(vin.mTex,gUVRotation,gUVRotationPivot,gUVTiling);  // vin.mTex * gUVTiling;
    


    float3 N  =normalize( mul(gWorldInvTrans, float4(normalLocal,0.0f))).xyz;
    float3 T  = mul(gWorld,float4(tangentLocal,0.0f));

    T=normalize(T -N * dot(N,T));    
    float3 B = normalize( cross(N,T)) *vin.mTangent.w;
    


    
    float2x2 uvTransformInv =  GetUVTransformInvMatrix(gUVRotation,gUVTiling);

    
   float3 newT =
    T * uvTransformInv[0][0] +
    B * uvTransformInv[1][0];

    vout.mNormal =N;
    vout.mTangent.xyz= newT;
    vout.mTangent.w = vin.mTangent.w;
    
    
    return vout;

}



float4 PS(VertexOut pin):SV_Target
{
    float3 tangentNormal =  NormalMap.Sample(_LinearSampler,pin.mTex).xyz;
    tangentNormal = tangentNormal * 2.0f -1.0f;

    float3 T = normalize(pin.mTangent.xyz);
    float3 N = normalize(pin.mNormal);
    float3 B = normalize( cross(N,T)) * pin.mTangent.w;

    float3x3 TBN =float3x3(T,B,N);

    float3 normalWorld=normalize(mul(tangentNormal,TBN));

    float3 finalColor =float3(0,0,0);
    float3 toEye = normalize(gEye - pin.mPosW);
    float4 color  =  _TexMap.Sample(_LinearSampler,pin.mTex);
    
    color *=float4(gDiffuseFactor,1.0f);


    float shadowFactor= CalcShadowFactor(pin.mShadowPosH);

    for(int i = 0; i < gLightNums; ++i)
    {
        // 라이트의 Position과 Direction은 이미 월드 공간이므로 변환 없이 쾌적하게 계산!
        finalColor += ComputeLighting(gLights[i], color.xyz, pin.mPosW, normalWorld, toEye,shadowFactor); 
    }

    float3 ambient= gAmbientLight.xyz * gAmbient * color;
    
  
    finalColor += gEmissiveColor;
    finalColor*=gEmissiveIntensity;
  
  finalColor += ambient; 
     
    
    return float4(finalColor,1.0f);


}






    

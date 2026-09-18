

static const char DefaultStaticMeshHLSL[] = R"(

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
};

cbuffer MaterialBuffer :register(b2)
{
    float3 gDiffuseFactor;
    float gMetallic;
    float3 gAmbient;
    float gRoughness;
    float3 gEmissiveColor;
    float gEmissiveIntensity;
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

SamplerState _LinearSampler :register(s0);
SamplerComparisonState _ShadowSampler : register(s2);


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

    
    float alpha = roughness * roughness;
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
    vout.mShadowPosH = mul(posW,mLightViewProj);
    
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






    )";

static const char DefaultStaticMesh_Unlit_HLSL[] = R"(

cbuffer MaterialBuffer :register(b2)
{
    float3 gDiffuseFactor;
    float gMetallic;
    float3 gAmbient;
    float gRoughness;
    float3 gEmissiveColor;
    float gEmissiveIntensity; 
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


)";

static const char OutlineStaticMeshHLSL[] = R"(
cbuffer PassBuffer:register(b0)
{
    float4x4 gViewProj;
};


cbuffer ObjectBuffer :register(b1)
{
    float4x4 gWorld;
    float4 gOutlineColor;
};


struct VertexIn
{
    float3 mPosL : POSITION;
    float2 mTex : TEX;  
    float3 mNormal : NORMAL;   
    float4 mTagent :TANGENT;
};


struct VertexOut
{
    float4 mPosH :SV_POSITION;
};


VertexOut VS_Stencil(VertexIn vin)
{
     VertexOut vout;
    float3 scaledPosL= vin.mPosL * 1.0f;
    float4 posW = mul(gWorld,float4(scaledPosL,1.0F));
    vout.mPosH =mul(posW,gViewProj);

    return vout;
}


VertexOut VS_DrawOutline(VertexIn vin)
{
    VertexOut vout;
    float3 scaledPosL= vin.mPosL * 1.03f;
    float4 posW = mul(gWorld,float4(scaledPosL,1.0F));
    vout.mPosH =mul(posW,gViewProj);

   // vout.mNormal = mul(vin.mNormal, (float3x3)gWorldInvTrans );
    return vout;
}

float4 PS(VertexOut pin):SV_Target
{
    return gOutlineColor;
}
)";

static const char GrayScaleHLSL[] = R"(

cbuffer PassBuffer : register(b0)
{
    float gGrayScale;
};


struct VertexIn
{
    float2 mPos : POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
    float mPxRange : COMMON;
    float mCommonTwo : COMMON1;
    float mCommonThree : COMMON2;
};


struct VertexOut
{
    float4 mPos : SV_POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
};

Texture2D _TexMap : register(t1);
SamplerState _LinearSampler : register(s0);

VertexOut VSMain(uint vertexID : SV_VertexID)
{
    VertexOut vout;

    //O(00)  ->  (0,0)
    //1(01)  ->  (1,0)
    //2(10)  ->  (0,1)
    //3 (11) ->  (1,1)
 
    vout.mTex = float2(vertexID & 1 , vertexID >> 1);
    
    vout.mPos = float4(vout.mTex.x * 2.0f - 1.0f ,  (vout.mTex.y *2.0f - 1.0f)*-1.0f ,0.0f,1.0f );


    return vout;
}


float4 PSMain(VertexOut pin) : SV_Target
{
    float4 color = _TexMap.Sample(_LinearSampler,pin.mTex);
    return color * gGrayScale;
})";

static const char DefaultUIHLSL[] = R"(

cbuffer PassBuffer:register(b0)
{
    float4x4 gViewProj;
    float gScreenWidth;
    float gScreenHeight;

};

struct VertexIn
{
    float2 mPos : POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
    float mBorderU : COMMON;
    float mBorderV : COMMON1;
    float mUseBorder: COMMON2;
    float mCommonFour: COMMON3;
    float4 mBorderColor : COMMON4;

};

struct VertexOut
{
    float4 mPos : SV_POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
    float mBorderU : COMMON;
    float mBorderV : COMMON1;
    float mUseBorder: COMMON2;
    float mCommonFour: COMMON3;
    float4 mBorderColor : COMMON4;
};

Texture2D _TexMap : register(t1);
SamplerState _LinearSampler : register(s0);
SamplerState _MIN_MAG_MIP_POINTE : register(s1);



VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.mPos = mul(float4(vin.mPos, 0.0f, 1.0f) ,gViewProj); 

    vout.mTex = vin.mTex;
    vout.mColor= vin.mColor;
    vout.mBorderU =vin.mBorderU;
    vout.mBorderV =vin.mBorderV;
    vout.mUseBorder = vin.mUseBorder;
    vout.mCommonFour = vin.mCommonFour;
    vout.mBorderColor = vin.mBorderColor;


    return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
    float4 color = _TexMap.Sample(_LinearSampler, pin.mTex);
    
     
   bool bBorder =   pin.mUseBorder > 0.1f && ( pin.mTex.x <= pin.mBorderU  || pin.mTex.x >= (1.0f-pin.mBorderU) || pin.mTex.y <= pin.mBorderV  || pin.mTex.y >= (1.0f- pin.mBorderV ));



  if(bBorder)
       return pin.mBorderColor;

 clip(color.a < 0.1f  ? -1 : 1);
 

    
    

   return color * pin.mColor;
}

)";

static const char DefaultFontHLSL[] = R"(cbuffer PassBuffer : register(b0)
{
    float4x4 gViewProj;
};

struct VertexIn
{
    float2 mPos : POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
    float mPxRange : COMMON;
    float mCommonTwo : COMMON1;
    float mCommonThree : COMMON2;
};

struct VertexOut
{
    float4 mPos : SV_POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
    float mPxRange : COMMON;
};

Texture2D _TexMap : register(t1);
SamplerState _LinearSampler : register(s0);

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.mPos = mul(float4(vin.mPos, 0.0f, 1.0f), gViewProj);
    vout.mTex = vin.mTex;
    vout.mColor = vin.mColor;
    vout.mPxRange = vin.mPxRange;
    return vout;
}

float middle(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

float4 PS(VertexOut pin) : SV_Target
{
    float3 msd = _TexMap.Sample(_LinearSampler, pin.mTex).rgb;

    // sd >= 0.5f 글자 안, s< 0.5f 글자 밖  , 0.5f는 다 글자경계
    float sd = middle(msd.r, msd.g, msd.b);

    // 스크린기준 불투명 값  ,  (범위  -0.5 ~ 0.5 로 이동후 PxRange 곱)
    float screenDist = (sd - 0.5f) * pin.mPxRange;

    float opacity = clamp(screenDist + 0.5f, 0.0f, 1.0f);

    float4 color = float4(pin.mColor.xyz, opacity);
    return color;
}
)";

static const char DebugGridHLSL[] = R"(

cbuffer PassBuffer :register(b0)
{
float4x4 gViewProj;
float3 gEye;
}


struct VertexIn
{
    float2 mPos : POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
    float mPxRange : COMMON;
    float mCommonTwo : COMMON1;
    float mCommonThree : COMMON2;
};

struct VertexOut
{
    float4 mPosH :SV_POSITION;
    float3 mPosW: POSITION;
};

VertexOut VS(uint vertexID : SV_VertexID)
{
    float scale = 10000.0f;


    float2 gridCorners[4] = {
        float2(-scale,  scale), // 0번 정점 (좌상단)
        float2( scale,  scale), // 1번 정점 (우상단)
        float2(-scale, -scale), // 2번 정점 (좌하단)
        float2( scale, -scale)  // 3번 정점 (우하단)
    };

    float3 worldPos = float3(
        gEye.x + gridCorners[vertexID].x,
        0.0f, // 격자는 바닥이니까 Y는 0
        gEye.z + gridCorners[vertexID].y
    );


VertexOut vout;
vout.mPosW =worldPos;
vout.mPosH =mul(float4(worldPos,1.0f),gViewProj);


return vout;

}


float DrawGrid(float2 worldPos, float gridSize)
{
    //현재픽셀의 월드좌표를 그리드 단위의좌표로 변환 
    float2 coord = worldPos/gridSize;


    //픽셀사이의 월드좌표(그리드단위)변화율을 계산 
    float2 derivative  = fwidth(coord);

    // 정수값을 중심으로 -0.5 0.5 사이의값들에대해서  정수는 0 , 양 끝은 최대 0.5
    //를 가지는 중심에서 벌어진 정도를 나타내는  v 모양으로 변환하는 작업
    //마지막으로 픽셀단위로 변환 (0을 기준으로 떨어진 픽셀 거리 정보를 얻는다 )
    float2 grid = abs(frac(coord-0.5f)-0.5f) / derivative ;
    
    //x축,y축선중 좀 더 진한것(정수에 가까운것선택)
    float Line = min(grid.x, grid.y);

//1픽셀 이내의 값들은 부드럽게 블러차리 , 1픽셀을 넘어가는 경우에는 완전 투명 
    return 1.0f-min(Line,1.0f);
}


float4 PS(VertexOut pin) :SV_Target{

float thinLine = DrawGrid(pin.mPosW.xz , 1.0f);

float thickLine= DrawGrid(pin.mPosW.xz, 10.0F);

float dist = length(pin.mPosW - gEye);

float fade = 1.0f- saturate(dist/100.0f); //100 이하 

float3 gridColor = float3(0.5f,0.5f,0.5f);

//굵은라인은 더 찐하게 
float alpha = max(thinLine * 0.3f, thickLine * 0.8f) * fade;

return float4(gridColor,alpha);

}




)";

static const char BillboardHLSL[] = R"(



cbuffer PassBuffer :register(b0)
{
    float4x4 gViewProj;
    float3 gEye;
}

struct ObjectData
{
    float4x4 mWorld;
    float2 mSize;
    float mPadding1;
    float mPadding2;
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

)";

static const char DebugLineHLSL[] = R"(



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

)";

static char SkySphereHLSL[] = R"(

cbuffer PassBuffer:register(b0)
{
    float4x4 gViewProj;
    float3 gEye;
    int gLightNums;
    float4 gAmbientLight; 
    float4x4 gWorld;
};




Texture2D _TexMap :register(t1);

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
};



VertexOut VS(VertexIn vin) 
{
    VertexOut vout;

    float4 posW = mul(gWorld , float4(vin.mPosL,1.0f));
    vout.mPosH = mul(posW,gViewProj);
    vout.mPosW =posW;
    vout.mTex =vin.mTex;

return vout;


}


float4 PS(VertexOut pin) :SV_Target
{

    float4 color =_TexMap.Sample(_LinearSampler,pin.mTex.xy);
 
    return color;



})";

static const char ShadowHLSL[] = R"(
cbuffer PassBuffer:register(b0)
{
    float4x4 gLightViewProj;
};

cbuffer ObjectBuffer : register(b1)
{
    float4x4 gWorld;
    float4x4 gWorldInvTrans;
};

struct VertexIn
{
    float3 mPosL : POSITION;
};

struct VertexOut
{
    float4 mPosH : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    float4 posW = mul(gWorld, float4(vin.mPosL, 1.0f));
    vout.mPosH = mul(posW,gLightViewProj);

    return vout;
}
)";

static const char ToneMappingHLSL[] = R"(


cbuffer PassBuffer:register(b0)
{
    float gExposure;
};

struct VertexIn
{
    float2 mPos : POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
    float mPxRange : COMMON;
    float mCommonTwo : COMMON1;
    float mCommonThree : COMMON2;
};


struct VertexOut
{
    float4 mPos : SV_POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
};

Texture2D _TexMap : register(t1);
Texture2D _TexMapTwo : register(t2);
SamplerState _LinearSampler : register(s0);

VertexOut VSMain(uint vertexID : SV_VertexID)
{
    VertexOut vout;

    //O(00)  ->  (0,0)
    //1(01)  ->  (1,0)
    //2(10)  ->  (0,1)
    //3 (11) ->  (1,1)
 
    vout.mTex = float2(vertexID & 1 , vertexID >> 1);
    
    vout.mPos = float4(vout.mTex.x * 2.0f - 1.0f ,  (vout.mTex.y *2.0f - 1.0f)*-1.0f ,0.0f,1.0f );


    return vout;
}


float4 PSMain(VertexOut pin) : SV_Target
{
    float3 color = _TexMap.Sample(_LinearSampler,pin.mTex).rgb;
    float3 color2 =_TexMapTwo.Sample(_LinearSampler,pin.mTex).rgb;

    color+=color2 * 0.3f;

    color *= gExposure;
    color =  color /(1.0f + color);


    return float4(color, 1.0f);


};

)";

static const char BloomHorizontalHLSL[] = R"(
cbuffer BloomConstantData : register(b0)
{
    uint gWidth;
    uint gHeight;
    float gThreshold;
};

Texture2D<float4> gInputTexture : register(t0);
RWTexture2D<float4> gOutputTexture : register(u0);
SamplerState gLinearSampler : register(s0);

float4 ExtractBrightColor(float4 color)
{
    float brightness = max(color.r, max(color.g, color.b));
    float contribution = max(brightness - gThreshold, 0.0f) / max(brightness, 0.0001f);
    return color * contribution;
}

[numthreads(256, 1, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (dispatchThreadID.x >= gWidth || dispatchThreadID.y >= gHeight)
        return;

    uint inputWidth;
    uint inputHeight;
    gInputTexture.GetDimensions(inputWidth, inputHeight);

    float2 outputSize = float2(gWidth, gHeight);
    float2 uv = (float2(dispatchThreadID.xy) + 0.5f) / outputSize;
    float2 texelSize = 1.0f / float2(gWidth, gHeight);



float weights[11] =
{
    0.008812f,
    0.027144f,
    0.065114f,
    0.121649f,
    0.176998f,
    0.200565f,
    0.176998f,
    0.121649f,
    0.065114f,
    0.027144f,
    0.008812f
};

float2 texOffset[11] =
{
    {-texelSize.x * 5.0f, 0.0f},
    {-texelSize.x * 4.0f, 0.0f},
    {-texelSize.x * 3.0f, 0.0f},
    {-texelSize.x * 2.0f, 0.0f},
    {-texelSize.x * 1.0f, 0.0f},
    { 0.0f,              0.0f},
    { texelSize.x * 1.0f, 0.0f},
    { texelSize.x * 2.0f, 0.0f},
    { texelSize.x * 3.0f, 0.0f},
    { texelSize.x * 4.0f, 0.0f},
    { texelSize.x * 5.0f, 0.0f}
};

    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for(int i=0; i<11; ++i)
    {
        color  +=  ExtractBrightColor(gInputTexture.SampleLevel(gLinearSampler, uv + texOffset[i], 0.0f)) *weights[i] ;


    }


    // float4 color = ExtractBrightColor(gInputTexture.SampleLevel(gLinearSampler, uv, 0.0f)) * 0.5f;
    // color += ExtractBrightColor(gInputTexture.SampleLevel(gLinearSampler, uv - float2(texelSize.x, 0.0f), 0.0f)) *
    //          0.25f;
    // color += ExtractBrightColor(gInputTexture.SampleLevel(gLinearSampler, uv + float2(texelSize.x, 0.0f), 0.0f)) *
    //          0.25f;
    gOutputTexture[dispatchThreadID.xy] = color;
}

)";

static const char BloomVerticalHLSL[] = R"(
cbuffer BloomConstantData : register(b0)
{
    uint gWidth;
    uint gHeight;
    float gThreshold;
};

Texture2D<float4> gInputTexture : register(t0);
RWTexture2D<float4> gOutputTexture : register(u0);
SamplerState gLinearSampler : register(s0);

[numthreads(1, 256, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (dispatchThreadID.x >= gWidth || dispatchThreadID.y >= gHeight)
        return;

    uint inputWidth;
    uint inputHeight;
    gInputTexture.GetDimensions(inputWidth, inputHeight);

    float2 outputSize = float2(gWidth, gHeight);
    float2 uv = (float2(dispatchThreadID.xy) + 0.5f) / outputSize;
    float2 texelSize = 1.0f / float2(gWidth, gHeight);

    float weights[11] =
    {
        0.008812f,
        0.027144f,
        0.065114f,
        0.121649f,
        0.176998f,
        0.200565f,
        0.176998f,
        0.121649f,
        0.065114f,
        0.027144f,
        0.008812f
    };

    float2 texOffset[11] =
    {
        {0.0f, -texelSize.y * 5.0f},
        {0.0f, -texelSize.y * 4.0f},
        {0.0f, -texelSize.y * 3.0f},
        {0.0f, -texelSize.y * 2.0f},
        {0.0f, -texelSize.y * 1.0f},
        {0.0f,  0.0f},
        {0.0f,  texelSize.y * 1.0f},
        {0.0f,  texelSize.y * 2.0f},
        {0.0f,  texelSize.y * 3.0f},
        {0.0f,  texelSize.y * 4.0f},
        {0.0f,  texelSize.y * 5.0f}
    };

    float4 color =
        float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 11; ++i)
    {
        color += gInputTexture.SampleLevel(
            gLinearSampler,
            uv + texOffset[i],
            0.0f)
            * weights[i];
    }

    gOutputTexture[dispatchThreadID.xy] = color;
}
)";

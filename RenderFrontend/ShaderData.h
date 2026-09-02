

static const char DefaultStaticMeshHLSL[] = R"(

cbuffer PassBuffer:register(b0)
{
    float4x4 gViewProj;
    float3 gEye;
    int gLightNums;
    float4 gAmbientLight; 
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

    float m  =max( (1.0f- gRoughness) *255.0f, 1.0f) ;

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

    float3 T = normalize(pin.mTangent.xyz);
    float3 N = normalize(pin.mNormal);
    float3 B = normalize( cross(N,T)) * pin.mTangent.w;

    float3x3 TBN =float3x3(T,B,N);

    float3 normalWorld=normalize(mul(tangentNormal,TBN));

    float3 finalColor =float3(0,0,0);
    float3 toEye = normalize(gEye - pin.mPosW);
    float4 color  =  _TexMap.Sample(_LinearSampler,pin.mTex);
    
    color *=float4(gDiffuseFactor,1.0f);

    for(int i = 0; i < gLightNums; ++i)
    {
        // 라이트의 Position과 Direction은 이미 월드 공간이므로 변환 없이 쾌적하게 계산!
        finalColor += ComputeLighting(gLights[i], color.xyz, pin.mPosW, normalWorld, toEye); 
    }

    float3 ambient= gAmbientLight.xyz * gAmbient * color;
    
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
    float mCommonOne : COMMON;
    float mCommonTwo : COMMON1;
    float mCommonThree: COMMON2;
};

struct VertexOut
{
    float4 mPos : SV_POSITION;
    float2 mTex : TEXCOORD;
    float4 mColor : COLOR;
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
    return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
    float4 color = _TexMap.Sample(_LinearSampler, pin.mTex);


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


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


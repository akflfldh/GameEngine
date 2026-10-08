


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


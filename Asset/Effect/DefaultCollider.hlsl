
cbuffer ObjectConstant:register(b0)
{
	float4x4 gWorld;
}


cbuffer PassConstant:register(b1)
{
	float4x4 gViewProj;

}


Texture2D gDiffuseMap :register(t0);

SamplerState gSamMinMagMipLinear :register(s0);

//
//cbuffer MaterialConstant:register(b2)
//{
//	float3 gFresnelR0;
//	float gShiness;
//	float3 gSpecular;
//	float3 gAmbient;
//
//}



struct VertexIn
{
	float3 mPosL:POSITION;
	float2 mTex:TEX;
};


struct VertexOut
{
	float4 mPosH:SV_POSITION;
	float2 mTex:TEX;
};


VertexOut VS(VertexIn vin)
{

	VertexOut vout;

	float4 posW = mul(float4(vin.mPosL, 1.0f), gWorld);
	vout.mPosH = mul(posW, gViewProj);
	vout.mTex = vin.mTex;
	return vout;
}



float4 PS(VertexOut pin) :SV_Target
{
	//float4 color = float4(0.0f,0.0f,0.0f,1.0f);

	float4 color= gDiffuseMap.Sample(gSamMinMagMipLinear, pin.mTex);

	clip(color.w < 0.1f ? -1 : 1);
	return color;
}
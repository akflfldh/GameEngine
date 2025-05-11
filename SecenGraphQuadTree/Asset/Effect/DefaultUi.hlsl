
cbuffer ObjectConstant:register(b0)
{
	float4x4 gWorld;
	float3 gColorIntensity;
	float4x4 gTextureTrans0;
}

cbuffer PassConstant:register(b1)
{
	float4x4 gViewProj;
};


Texture2D gDiffuseMap :register(t0);

SamplerState gSamMinMagMipLinear :register(s0);



struct VertexIn
{
	float3 mPosL:POSITION;
	float3 mNormal:NORMAL;
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

	vout.mTex = mul(float4(vin.mTex,0.0f,1.0f),gTextureTrans0);
	return vout;
}



float4 PS(VertexOut pin) :SV_Target
{
	float4 color = gDiffuseMap.Sample(gSamMinMagMipLinear,pin.mTex);

	clip(color.w < 0.1f ? -1 : 1 );

	color = float4(gColorIntensity,1.0f)*color;
	
	color = pow(color,1/2.2f);

	return color;
}
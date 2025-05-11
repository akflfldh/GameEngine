
cbuffer ObjectConstant:register(b0)
{
	float4x4 gWorld;
	float3 gColorIntensity;
	float3 gColor;
}


cbuffer PassConstant:register(b1)
{
	float4x4 gViewProj;
};


Texture2D gDiffuseMap :register(t0);

SamplerState gSamMinMagMipLinearBorder :register(s0);



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

	float4 color = gDiffuseMap.Sample(gSamMinMagMipLinearBorder,pin.mTex);
	
	clip(color.w == 0.0f ? -1 : 1);

	color =color*float4(gColorIntensity,1.0f) * float4(gColor,1.0f);



	color= pow(color,(1/2.2));
	//color.w=1.0f;

	

	
	return color;


}


cbuffer ObjectConstant :register(b0)
{
	float4x4 gWorld;
	float3 gColor;
}


cbuffer PassConstant:register(b1)
{
	float4x4 gViewProj;
}




struct VertexIn
{
	float3 mPosL:POSITION;
};



struct VertexOut
{
	float4 mPosH:SV_POSITION;
};



VertexOut VS(VertexIn vin)
{

	VertexOut vout;
	float4 posW = mul(float4(vin.mPosL, 1.0f), gWorld);
	
	vout.mPosH = mul(posW, gViewProj);

	return vout;
}



float4 PS(VertexOut pin) :SV_Target
{

	float4 color =	float4(gColor,1.0f);

	return color;

}







struct VertexOut
{
	float4 mPosH:SV_POSITION;
	float3 mPosW:POSITION;
	float3 mNormal:NORMAL;
	float2 mTex:TEX;
};


float4 PS(VertexOut pin) :SV_Target
{
	



    
    return color;

}
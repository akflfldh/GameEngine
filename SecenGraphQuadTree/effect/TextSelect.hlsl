


//object�� �ʿ����.

cbuffer PassConstantBuffer : register(b0)
{

	float4x4 gViewProj;
};


struct ObjectData
{
	float4x4 gWorld;
};

//�� ����ü���� �̸����ְ� �װ��� �����ϰ� ���������۸� �� �����ϰ�

StructuredBuffer<ObjectData> gObjectStructuredBuffer:register(t0);

Texture2D gDiffuseMap : register(t1);

SamplerState gSamMinMagMipLinear : register(s0);


struct VertexIn
{
	float3 mPosL:POSITION;
	float2 mTex:TEX;
};

struct VertexOut
{

	float4 mPosH :SV_POSITION;
	float2 mTex:TEX;

};


VertexOut VS(float3 posL:POSITION, float2 tex : TEX, uint instanceID: SV_InstanceID )
{
	VertexOut vout;

	float4x4 worldMatrix = gObjectStructuredBuffer[instanceID].gWorld;
	
	float4 posW=mul(float4(posL, 1.0f), worldMatrix);
	vout.mPosH = mul(posW, gViewProj);

	vout.mTex = tex;

	return vout;
}


float4 PS(VertexOut pin) :SV_Target
{

	float4 color = gDiffuseMap.Sample(gSamMinMagMipLinear,pin.mTex);
	//������ �����Ͷ����� ����
	return color;

}








#include"DefaultLight.hlsl"

cbuffer ObjectConstant:register(b0)
{
	float4x4 gWorld;
	float4x4 gWorldInvTrans;
	float4x4 gTextureTrans0;
	float3 gEye;

	float3 gColor;
}


cbuffer PassConstant:register(b1)
{
	float4x4 gViewProj;
};

// cbuffer MaterialConstant:register(b2)
// {
	
// 	// float3 gFresnelR0;
// 	// float gShiness;
// 	// float3 gSpecular;
// 	// float3 gAmbient;
	
// }

// cbuffer LightConstant:register(b3)
// {
// 	Light gDirectionalLight[10];
// 	Light gSpotLight[10];
// 	Light gPointLight[10];

// 	int gDirectionalLightCount;
// 	int gSpotLightCount;
// 	int gPointLightCount;


// }


//light



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
	float3 mPosW:POSITION;
	float3 mNormal:NORMAL;
	float2 mTex:TEX;
};


VertexOut VS(VertexIn vin)
{

	VertexOut vout;

	float4 posW = mul(float4(vin.mPosL, 1.0f), gWorld);
	vout.mPosW = posW.xyz;
	vout.mPosH = mul(posW, gViewProj);


	vout.mNormal = normalize(mul(float4(vin.mNormal, 0.0f), gWorldInvTrans).xyz);

	vout.mTex = mul(float4(vin.mTex,0.0f,1.0f),gTextureTrans0);
	
	return vout;

}



float4 PS(VertexOut pin) :SV_Target
{
	//����ó��
	// float3 lightVector = -1.0f * gDirectionalLight[0].gDirection;
	// float3 normal = normalize(pin.mNormal);
	// float3 lightintensity = gDirectionalLight[0].gLight;


	 float4 color =gDiffuseMap.Sample(gSamMinMagMipLinear,pin.mTex);
	clip(color.x == 0.0f ? -1 : 1);

    color=float4(gColor,1.0f);
	// float4 ambientTerm = float4(gAmbient, 0.0f) * float4(lightintensity,0.0f);
	// //color = max(dot(lightVector, normal), 0) * color * float4(lightintensity, 1.0f)+ ambientTerm;
	// //float4 color = float4(1.0f, 0.0f, 1.0f,1.0f);

	// float3 eyeVector = gEye - pin.mPosW;
	// color = CaculateLight(eyeVector, lightintensity, lightVector, normal, color, gFresnelR0, gShiness);
	return color;
}
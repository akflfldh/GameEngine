
#include"DefaultLight.hlsl"

cbuffer ObjectConstant:register(b0)
{
	float4x4 gWorld;
	float4x4 gWorldInvTrans;
	float3 gEye;
	float gDiffuseMapGammaFlag;
#ifdef SKINNING
	float4x4 gBoneAnimationMatrix[120];
	float4x4 gBoneAnimationInverseTransposeMatrix[120];
#endif 

}


cbuffer PassConstant:register(b1)
{
	float4x4 gViewProj;
};

cbuffer MaterialConstant:register(b2)
{
	float3 gFresnelR0;
	float gShiness;
	float3 gSpecular;
	float3 gAmbient;

}

cbuffer LightConstant:register(b3)
{
	Light gDirectionalLight[10];
	Light gSpotLight[10];
	Light gPointLight[10];

	int gDirectionalLightCount;
	int gSpotLightCount;
	int gPointLightCount;
	

}


//light



Texture2D gDiffuseMap :register(t0);

SamplerState gSamMinMagMipLinear :register(s0);



struct VertexIn
{
	float3 mPosL:POSITION;
	float3 mNormal:NORMAL;
	float2 mTex:TEX;

#ifdef SKINNING
	float4 mWeightOne:BONEWEIGHTONE;
	float4 mWeightTwo:BONEWEIGHTTWO;
	int4 mBoneIndexOne:BONEINDEXONE;
	int4 mBoneIndexTwo:BONEINDEXTWO;
#endif 
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




	float3 posL = vin.mPosL;
	float3 normalL = vin.mNormal;

#ifdef SKINNING
	float totalWeight = 0.0f;

	totalWeight += vin.mWeightOne.x;
	totalWeight += vin.mWeightOne.y;
	totalWeight += vin.mWeightOne.z;
	totalWeight += vin.mWeightOne.w;
	totalWeight += vin.mWeightTwo.x;
	totalWeight += vin.mWeightTwo.y;
	totalWeight += vin.mWeightTwo.z;
	totalWeight += vin.mWeightTwo.w;


	

	int boneIndexArray[8];
	float boneWeightArray[8];
	boneIndexArray[0] = vin.mBoneIndexOne.x;
	boneIndexArray[1] = vin.mBoneIndexOne.y;
	boneIndexArray[2] = vin.mBoneIndexOne.z;
	boneIndexArray[3] = vin.mBoneIndexOne.w;
	boneIndexArray[4] = vin.mBoneIndexTwo.x;
	boneIndexArray[5] = vin.mBoneIndexTwo.y;
	boneIndexArray[6] = vin.mBoneIndexTwo.z;
	boneIndexArray[7] = vin.mBoneIndexTwo.w;

	boneWeightArray[0] = vin.mWeightOne.x;
	boneWeightArray[1] = vin.mWeightOne.y;
	boneWeightArray[2] = vin.mWeightOne.z;
	boneWeightArray[3] = vin.mWeightOne.w;
	boneWeightArray[4] = vin.mWeightTwo.x;
	boneWeightArray[5] = vin.mWeightTwo.y;
	boneWeightArray[6] = vin.mWeightTwo.z;
	boneWeightArray[7] = vin.mWeightTwo.w;


	posL = float3(0, 0, 0);
	normalL = float3(0, 0, 0);

	for (int i = 0; i < 8; ++i)
	{
		posL += boneWeightArray[i] * mul(float4(vin.mPosL, 1.0f), gBoneAnimationMatrix[boneIndexArray[i]]).xyz;
		normalL += boneWeightArray[i] * mul(float4(vin.mNormal, 0.0f), gBoneAnimationInverseTransposeMatrix[boneIndexArray[i]]).xyz;

	}
	posL /= totalWeight;
	normalL /= totalWeight;
#endif 





	float4 posW = mul(float4(posL, 1.0f),gWorld);
	vout.mPosW = posW.xyz;
	vout.mPosH = mul(posW,gViewProj);


	vout.mNormal =	normalize(mul(float4(normalL, 0.0f), gWorldInvTrans).xyz);
	//vout.mNormal =float4(normalL,0.0f);//	normalize(mul(float4(normalL, 0.0f), gWorldInvTrans).xyz);

	vout.mTex = vin.mTex;
	return vout;
}



float4 PS(VertexOut pin) :SV_Target
{
	//����ó��
	 // 조명 처리
    float3 lightVector = normalize(-1.0f * gDirectionalLight[0].gDirection);
    float3 normal = normalize(pin.mNormal);
    float3 lightIntensity = gDirectionalLight[0].gLight;

    float4 color = gDiffuseMap.Sample(gSamMinMagMipLinear, pin.mTex);
    // float4 ambientTerm = float4(gAmbient, 0.0f) * float4(lightIntensity, 0.0f);

    float3 eyeVector = normalize(gEye - pin.mPosW); // 정규화 추가
    color = CaculateLight(eyeVector, lightIntensity, lightVector, normal, color.rgb, gFresnelR0, gShiness,gSpecular);


     color =  pow(color ,1 / 2.2f); // 감마 보정 수정
    color.a = 1.0f;
   
    return color;

}
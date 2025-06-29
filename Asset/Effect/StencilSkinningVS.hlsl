
cbuffer ObjectConstant:register(b0)
{
	float4x4 gWorld;
    float4x4 gBoneAnimationMatrix[120];
	float4x4 gBoneAnimationInverseTransposeMatrix[120];
};

cbuffer PassConstant:register(b1)
{
    float4x4 gViewProj;
};


struct VertexIn
{
    float3 mPosL:POSITION;
    float4 mWeightOne:BONEWEIGHTONE;
	float4 mWeightTwo:BONEWEIGHTTWO;
	int4 mBoneIndexOne:BONEINDEXONE;
	int4 mBoneIndexTwo:BONEINDEXTWO;
};


struct VertexOut
{
    float4 mPosH :POSITION;
};


VertexOut VS(VertexIn vin)
{
    VertexOut vout;

	float3 posL = vin.mPosL;
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

	}
	posL /= totalWeight;

	float4 posW = mul(float4(posL, 1.0f),gWorld);
	vout.mPosW = posW.xyz;
	vout.mPosH = mul(posW,gViewProj);
    
    return vout;


}
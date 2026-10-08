
cbuffer PassBuffer:register(b0)
{
    float4x4 gLightViewProj;
};

cbuffer ObjectBuffer : register(b1)
{
    float4x4 gWorld;
    float4x4 gWorldInvTrans;

    uint gPaletteOffset = 0;
    uint gPaletteCount = 0;
};

#if defined(ENABLE_SKINNING)
StructuredBuffer<float4x4> gSkinPalette : register(t5);
#endif

struct VertexIn
{
    float3 mPosL : POSITION;
#if defined(ENABLE_SKINNING)
    uint4 mJointIndices0 : BONEINDEX0;
    uint4 mJointIndices1 : BONEINDEX1;
    float4 mJointWeights0 : BONEWEIGHT0;
    float4 mJointWeights1 : BONEWEIGHT1;
#endif
};

struct VertexOut
{
    float4 mPosH : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    float4 positionLocal = float4(vin.mPosL, 1.0f);
#if defined(ENABLE_SKINNING)
    // Main pass와 동일한 palette 식을 사용해야 shadow silhouette도 현재 pose를 따른다.
    float4x4 skinMatrix =
        gSkinPalette[vin.mJointIndices0.x+gPaletteOffset] * vin.mJointWeights0.x +
        gSkinPalette[vin.mJointIndices0.y+gPaletteOffset] * vin.mJointWeights0.y +
        gSkinPalette[vin.mJointIndices0.z+gPaletteOffset] * vin.mJointWeights0.z +
        gSkinPalette[vin.mJointIndices0.w+gPaletteOffset] * vin.mJointWeights0.w +
        gSkinPalette[vin.mJointIndices1.x+gPaletteOffset] * vin.mJointWeights1.x +
        gSkinPalette[vin.mJointIndices1.y+gPaletteOffset] * vin.mJointWeights1.y +
        gSkinPalette[vin.mJointIndices1.z+gPaletteOffset] * vin.mJointWeights1.z +
        gSkinPalette[vin.mJointIndices1.w+gPaletteOffset] * vin.mJointWeights1.w;
    positionLocal = mul(skinMatrix, positionLocal);
#endif

    float4 posW = mul(gWorld, positionLocal);
    vout.mPosH = mul(posW,gLightViewProj);

    return vout;
}


cbuffer PassBuffer:register(b0)
{
    float4x4 gViewProj;
};


cbuffer ObjectBuffer :register(b1)
{
    float4x4 gWorld;
    float4 gOutlineColor;
    uint gPaletteOffset;
    uint gPaletteCount;
    uint2 gPadding;
};

#if defined(ENABLE_SKINNING)
StructuredBuffer<float4x4> gSkinPalette : register(t5);
#endif


struct VertexIn
{
    float3 mPosL : POSITION;
    float2 mTex : TEX;  
    float3 mNormal : NORMAL;   
    float4 mTagent :TANGENT;
#if defined(ENABLE_SKINNING)
    uint4 mJointIndices0 : BONEINDEX0;
    uint4 mJointIndices1 : BONEINDEX1;
    float4 mJointWeights0 : BONEWEIGHT0;
    float4 mJointWeights1 : BONEWEIGHT1;
#endif
};


struct VertexOut
{
    float4 mPosH :SV_POSITION;
};

float4 GetOutlinePositionLocal(VertexIn vin)
{
    float4 positionLocal = float4(vin.mPosL, 1.0f);
#if defined(ENABLE_SKINNING)
    // Main pass와 동일한 palette를 먼저 적용해야 stencil과 확대 외곽선이 현재 pose를 따른다.
    float4x4 skinMatrix =
        gSkinPalette[vin.mJointIndices0.x + gPaletteOffset] * vin.mJointWeights0.x +
        gSkinPalette[vin.mJointIndices0.y + gPaletteOffset] * vin.mJointWeights0.y +
        gSkinPalette[vin.mJointIndices0.z + gPaletteOffset] * vin.mJointWeights0.z +
        gSkinPalette[vin.mJointIndices0.w + gPaletteOffset] * vin.mJointWeights0.w +
        gSkinPalette[vin.mJointIndices1.x + gPaletteOffset] * vin.mJointWeights1.x +
        gSkinPalette[vin.mJointIndices1.y + gPaletteOffset] * vin.mJointWeights1.y +
        gSkinPalette[vin.mJointIndices1.z + gPaletteOffset] * vin.mJointWeights1.z +
        gSkinPalette[vin.mJointIndices1.w + gPaletteOffset] * vin.mJointWeights1.w;
    positionLocal = mul(skinMatrix, positionLocal);
#endif
    return positionLocal;
}


VertexOut VS_Stencil(VertexIn vin)
{
     VertexOut vout;
    float4 posW = mul(gWorld, GetOutlinePositionLocal(vin));
    vout.mPosH =mul(posW,gViewProj);

    return vout;
}


VertexOut VS_DrawOutline(VertexIn vin)
{
    VertexOut vout;
    float3 scaledPosL= GetOutlinePositionLocal(vin).xyz * 1.03f;
    float4 posW = mul(gWorld,float4(scaledPosL,1.0F));
    vout.mPosH =mul(posW,gViewProj);

   // vout.mNormal = mul(vin.mNormal, (float3x3)gWorldInvTrans );
    return vout;
}

float4 PS(VertexOut pin):SV_Target
{
    return gOutlineColor;
}

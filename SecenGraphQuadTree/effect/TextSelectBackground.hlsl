
struct Data
{
	float4x4 gWorld;

};


cbuffer PassConstant:register(b0)
{
	float4x4 gViewProj;
}


Texture2D gDiffuseMap:register(t0);
StructuredBuffer<Data> gInstanceData:register(t1);


SamplerState gSamMINMagMipPoint:register(s0);




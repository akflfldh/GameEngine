#include "MaterialGenerator.h"
namespace Quad
{
	bool MaterialGenerator::CreateDefaultMaterial(RawMaterialData& oRawMaterialData)
	{
		oRawMaterialData.mName = "Default";
		oRawMaterialData.mType = MaterialType::eOpaque;
		oRawMaterialData.mEffectName = "Default.effect";
		oRawMaterialData.mDiffuseMapName = "Red.png";
		oRawMaterialData.mShiness = 10.0f;
		oRawMaterialData.mSpecular = { 0.2f,0.2f,0.2f };
		return true;
	}

}
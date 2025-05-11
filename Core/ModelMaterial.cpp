#include "Asset/Material/ModelMaterial/ModelMaterial.h"
#include"Parser/JsonParser.h"
#include"ResourceManager/EffectManager/EffectManager.h"

Quad::ModelMaterial::ModelMaterial()
	:Material()
{
}

Quad::ModelMaterial::ModelMaterial(const Material& mat)
	:Material(mat),mSourceMaterialPointer(mat.GetUniqueID())
{

}

Quad::ModelMaterial& Quad::ModelMaterial::operator=(const Material& mat)
{

	mDiffuseMapDirtyFlag = true;
	mNormalMapDirtyFlag = true;
	mEffectDirtyFlag = true;
	mSpecularDirtyFlag = true;

	 mDiffuseDirtyFlag = true;
	 mFresnelR0DirtyFlag = true;
	 mShinessDirtyFlag = true;
	 mSpecularDirtyFlag = true;
	 mAmbientDirtyFlag = true;
	 mColorDirtyFlag = true;
	 mColorIntensityDirtyFlag = true;
	 mDiffuseMapDirtyFlag = true;
	 mNormalMapDirtyFlag = true;
	 mEffectDirtyFlag = true;

	Material::operator=(mat);
	mSourceMaterialPointer = mat.GetUniqueID();
	//ResetDirtyFlag();
	// TODO: 여기에 return 문을 삽입합니다.

	return *this;

}

void Quad::ModelMaterial::SetDiffuseMap(Texture* texture)
{

	Material::SetDiffuseMap(texture);
	mDiffuseMapDirtyFlag = true;

}

void Quad::ModelMaterial::SetDiffuseMap(const TextureSmartPointer& texture)
{
	Material::SetDiffuseMap(texture);
	mDiffuseMapDirtyFlag = true;

}

void Quad::ModelMaterial::SetNormalMap(Texture* texture)
{

	Material::SetNormalMap(texture);
	mNormalMapDirtyFlag = true;
}

void Quad::ModelMaterial::SetNormalMap(const TextureSmartPointer& texture)
{
	Material::SetNormalMap(texture);
	mNormalMapDirtyFlag = true;
}

void Quad::ModelMaterial::SetEffect(Effect* effect)
{
	Material::SetEffect(effect);
	mEffectDirtyFlag = true;

}

void Quad::ModelMaterial::SetSpecular(const DirectX::XMFLOAT3& specular)
{
	Material::SetSpecular(specular);
	mSpecularDirtyFlag = true;
}

void Quad::ModelMaterial::SetAmbient(const DirectX::XMFLOAT3& ambient)
{
	Material::SetAmbient(ambient);

	mAmbientDirtyFlag = true;
}

void Quad::ModelMaterial::SetShiness(float shiness)
{

	Material::SetShiness(shiness);
	mShinessDirtyFlag = true;

}

void Quad::ModelMaterial::SetColor(const DirectX::XMFLOAT3& color)
{
	Material::SetColor(color);

	mColorDirtyFlag = true;
}

void Quad::ModelMaterial::SetColorIntensity(const DirectX::XMFLOAT3& intensity)
{
	Material::SetColorIntensity(intensity);
	mColorIntensityDirtyFlag = true;
}

void Quad::ModelMaterial::SetFresnelRO(const DirectX::XMFLOAT3& fresnelR0)
{

	Material::SetFresnelRO(fresnelR0);
	mFresnelR0DirtyFlag = true;
}

void Quad::ModelMaterial::SetEffectName(const std::string& effectName)
{
	Material::SetEffectName(effectName);
	mEffectDirtyFlag = true;
}

Quad::Texture* Quad::ModelMaterial::GetDiffuseMap() const
{

	if (mDiffuseMapDirtyFlag)
	{
		return Material::GetDiffuseMap();
	}
	else
	{
		return mSourceMaterialPointer->GetDiffuseMap();

	}
	
}

Quad::Texture* Quad::ModelMaterial::GetNormalMap() const
{

	if (mNormalMapDirtyFlag)
	{
		return Material::GetNormalMap();
	}
	else
	{
		return mSourceMaterialPointer->GetNormalMap();
	}
}

Quad::Effect* Quad::ModelMaterial::GetEffect() const
{

	if (mEffectDirtyFlag)
	{
		return Material::GetEffect();
	}
	else
	{
		return mSourceMaterialPointer->GetEffect();
	}
}

DirectX::XMFLOAT4 Quad::ModelMaterial::GetDiffuse() const
{

	if (mDiffuseDirtyFlag)
	{
		return Material::GetDiffuse();
	}
	else
	{
		return mSourceMaterialPointer->GetDiffuse();
	}

	
}

DirectX::XMFLOAT3 Quad::ModelMaterial::GetFresnelR0() const
{
	if (mFresnelR0DirtyFlag)
	{
		return Material::GetFresnelR0();
	}else
	{
		return mSourceMaterialPointer->GetFresnelR0();
	
	}

}

DirectX::XMFLOAT3 Quad::ModelMaterial::GetSpecular() const
{
	if (mSpecularDirtyFlag)
	{
		return Material::GetSpecular();
	}
	else
	{
		return mSourceMaterialPointer->GetSpecular();
	}
	
}

DirectX::XMFLOAT3 Quad::ModelMaterial::GetAmbient() const
{
	if (mAmbientDirtyFlag)
	{
		return Material::GetAmbient();
	}
	else
	{
		return mSourceMaterialPointer->GetAmbient();
	}

	
}

float Quad::ModelMaterial::GetShiness() const
{
	if (mShinessDirtyFlag)
	{
		return Material::GetShiness();
	}
	else
	{
		return mSourceMaterialPointer->GetShiness();
	}
	
}

const DirectX::XMFLOAT3& Quad::ModelMaterial::GetColor() const
{
	if (mColorDirtyFlag)
	{
		return Material::GetColor();
	}
	else
	{
		return mSourceMaterialPointer->GetColor();
	}
	// TODO: 여기에 return 문을 삽입합니다.
}

const DirectX::XMFLOAT3& Quad::ModelMaterial::GetColorIntensity() const
{
	if (mColorIntensityDirtyFlag)
	{
		return Material::GetColorIntensity();
	}
	else
	{
		return mSourceMaterialPointer->GetColorIntensity();
	}
	// TODO: 여기에 return 문을 삽입합니다.
}

const std::string& Quad::ModelMaterial::GetEffectName() const
{
	if (mEffectDirtyFlag)
	{
		return Material::GetEffectName();
	}
	else
	{
		return mSourceMaterialPointer->GetEffectName();
	}
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::ModelMaterial::SetSourceMaterial(Material* mat)
{
	mSourceMaterialPointer = mat;
	ResetDirtyFlag();
}

void Quad::ModelMaterial::SetSourceMaterial(unsigned long long id)
{
	mSourceMaterialPointer = id;
	ResetDirtyFlag();
}

void Quad::ModelMaterial::ResetDirtyFlag()
{
	mDiffuseDirtyFlag = false;
	mFresnelR0DirtyFlag = false;
	mShinessDirtyFlag = false;
	mSpecularDirtyFlag = false;
	mAmbientDirtyFlag = false;
	mColorDirtyFlag = false;
	mColorIntensityDirtyFlag = false;

	mDiffuseMapDirtyFlag = false;
	mNormalMapDirtyFlag = false;

	mEffectDirtyFlag = false;
}

void Quad::ModelMaterial::Serialize(const std::string& tag)
{

	//flag 
	JsonParser::Write("ModelMaterial_DiffuseDirtyFlag"+tag, mDiffuseDirtyFlag);
	JsonParser::Write("ModelMaterial_FresnelR0DirtyFlag" + tag, mFresnelR0DirtyFlag);
	JsonParser::Write("ModelMaterial_ShinessDirtyFlag" + tag, mShinessDirtyFlag);
	JsonParser::Write("ModelMaterial_SpecularDirtyFlag" + tag, mSpecularDirtyFlag);
	JsonParser::Write("ModelMaterial_AmbientDirtyFlag" + tag, mAmbientDirtyFlag);
	JsonParser::Write("ModelMaterial_ColorDirtyFlag" + tag, mColorDirtyFlag);
	JsonParser::Write("ModelMaterial_ColorIntensityDirtyFlag" + tag, mColorIntensityDirtyFlag);
	JsonParser::Write("ModelMaterial_DiffuseMapDirtyFlag" + tag, mDiffuseMapDirtyFlag);
	JsonParser::Write("ModelMaterial_NormalMapDirtyFlag" + tag, mNormalMapDirtyFlag);
	JsonParser::Write("ModelMaterial_EffectDirtyFlag" + tag, mEffectDirtyFlag);

	if (mDiffuseDirtyFlag)
		JsonParser::Write("ModelMaterial_Diffuse" + tag, GetDiffuse());

	if (mFresnelR0DirtyFlag)
		JsonParser::Write("ModelMaterial_FresnelR0" + tag, GetFresnelR0());

	if (mShinessDirtyFlag)
		JsonParser::Write("ModelMaterial_Shiness" + tag, GetShiness());

	if (mSpecularDirtyFlag)
		JsonParser::Write("ModelMaterial_Specular" + tag, GetSpecular());

	if (mAmbientDirtyFlag)
		JsonParser::Write("ModelMaterial_Ambient" + tag, GetAmbient());

	if (mColorDirtyFlag)
		JsonParser::Write("ModelMaterial_Color" + tag, GetColor());

	if (mColorIntensityDirtyFlag)
		JsonParser::Write("ModelMaterial_ColorIntensity" + tag, GetColorIntensity());

	if (mDiffuseMapDirtyFlag)
	{

		Texture* diffuseMap = GetDiffuseMap();
		unsigned long long id = 0;
		if (diffuseMap != nullptr)
			id = diffuseMap->GetUniqueID();

		JsonParser::Write("ModelMaterial_DiffuseMap" + tag, id);

	}
	if (mNormalMapDirtyFlag)
	{
		
		Texture* normalMap = GetDiffuseMap();
		unsigned long long id = 0;
		if (normalMap != nullptr)
			id = normalMap->GetUniqueID();
		JsonParser::Write("ModelMaterial_NormalMap" + tag, id);

	}
	if (mEffectDirtyFlag)
		JsonParser::Write("ModelMaterial_Effect" + tag, GetEffect()->GetUniqueID());






}

void Quad::ModelMaterial::DeSerialize(const std::string& tag)
{
	mDiffuseDirtyFlag =JsonParser::ReadBool("ModelMaterial_DiffuseDirtyFlag" + tag);
	mFresnelR0DirtyFlag= JsonParser::ReadBool("ModelMaterial_FresnelR0DirtyFlag" + tag);
	mShinessDirtyFlag =JsonParser::ReadBool("ModelMaterial_ShinessDirtyFlag" + tag);
	mSpecularDirtyFlag=	JsonParser::ReadBool("ModelMaterial_SpecularDirtyFlag" + tag);
	mAmbientDirtyFlag=JsonParser::ReadBool("ModelMaterial_AmbientDirtyFlag" + tag);
	mColorDirtyFlag=JsonParser::ReadBool("ModelMaterial_ColorDirtyFlag" + tag);
	mColorIntensityDirtyFlag=	JsonParser::ReadBool("ModelMaterial_ColorIntensityDirtyFlag" + tag);
	mDiffuseMapDirtyFlag=JsonParser::ReadBool("ModelMaterial_DiffuseMapDirtyFlag" + tag);
	mNormalMapDirtyFlag=JsonParser::ReadBool("ModelMaterial_NormalMapDirtyFlag" + tag);
	mEffectDirtyFlag=JsonParser::ReadBool("ModelMaterial_EffectDirtyFlag" + tag);



	 

	if (mDiffuseDirtyFlag)
	{
		DirectX::XMFLOAT4 diffuse;
		JsonParser::Read("ModelMaterial_Diffuse" + tag, diffuse);
		//SetDiffuse()
	}



	if (mFresnelR0DirtyFlag)
	{
		DirectX::XMFLOAT3 fresnelR0 ;
		JsonParser::Read("ModelMaterial_FresnelR0" + tag, fresnelR0);
		SetFresnelRO(fresnelR0);

	}
	if (mShinessDirtyFlag)
	{
		float shiness = 0.0f;
		JsonParser::Read("ModelMaterial_Shiness" + tag, shiness);
		SetShiness(shiness);
	}
	if (mSpecularDirtyFlag)
	{
		DirectX::XMFLOAT3 specular;
		JsonParser::Read("ModelMaterial_Specular" + tag, specular);
		SetSpecular(specular);
	}
	if (mAmbientDirtyFlag)
	{
		DirectX::XMFLOAT3 ambient;
		JsonParser::Read("ModelMaterial_Ambient" + tag, ambient);
		SetAmbient(ambient);
	}
	if (mColorDirtyFlag)
	{
		DirectX::XMFLOAT3 color;
		JsonParser::Read("ModelMaterial_Color" + tag, color);
		SetColor(color);
	}
	if (mColorIntensityDirtyFlag)
	{
		DirectX::XMFLOAT3 colorIntensity;
		JsonParser::Read("ModelMaterial_ColorIntensity" + tag, colorIntensity);
		SetColorIntensity(colorIntensity);
	}
	if (mDiffuseMapDirtyFlag)
	{
		unsigned long long id = 0;
		JsonParser::Read("ModelMaterial_DiffuseMap" + tag, id);
		SetDiffuseMap(id);
	}
	if (mNormalMapDirtyFlag)
	{
		unsigned long long id = 0;
		JsonParser::Read("ModelMaterial_NormalMap" + tag, id);
		SetNormalMap(id);
	}
	if (mEffectDirtyFlag)
	{
		unsigned long long id = 0;
		JsonParser::Read("ModelMaterial_Effect" + tag, id);
		SetEffect(EffectManager::GetEffect(id));
	}
}

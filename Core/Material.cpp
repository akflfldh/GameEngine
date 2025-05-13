#include "Asset/Material/Material.h"
#include"Utility/Utility.h"

#include"Parser/JsonParser.h"
#include"ResourceManager/EffectManager/EffectManager.h"
#include"ResourceSmartPointer/TextureSmartPointer.h"

namespace Quad
{
	/*Material::Material()
		:Asset("",EAssetType::eMaterial),mDiffuseMap(nullptr),mNormalMap(nullptr),mDirtyCallbackHandleTable(100,nullptr)
	{
		for (int i = 0; i < _countof(mTextureTransformMatrix); ++i)
			mTextureTransformMatrix[i] = Utility::GetIdentityMatrixF();
	}*/
	Material::Material(const std::string & name)
		:Asset(name,EAssetType::eMaterial),mDiffuseMap(nullptr),mNormalMap(nullptr),mDirtyCallbackHandleTable(100,nullptr),mEffect(nullptr)
	{

		for (int i = 0; i < _countof(mTextureTransformMatrix); ++i)
			mTextureTransformMatrix[i] = Utility::GetIdentityMatrixF();



	}

	Material::Material(const Material& mat)
		:Asset(mat) ,mType(mat.mType), mDiffuse(mat.mDiffuse),mFresnelR0(mat.mFresnelR0),
		mShiness(mat.mShiness),mAmbient(mat.mAmbient),mColor(mat.mColor),mColorIntensity(mat.mColorIntensity),
		mDiffuseMap(mat.mDiffuseMap),mNormalMap(mat.mNormalMap),mEffect(mat.mEffect),mEffectName(mat.mEffectName),mDirtyCallbackHandleTable(100,nullptr)
	{
		 
		//핸들테이블은 인스턴스마다 고유하니 이동생성과 이동대입일때만 값이 복사된다.



		for (int i = 0; i < _countof(mTextureTransformMatrix); ++i)
			mTextureTransformMatrix[i] = mat.mTextureTransformMatrix[i];
		SetDirtyFlag(true);
	}

	Material::Material(Material&& mat)
		:Asset(std::move(mat)), mType(mat.mType), mDiffuse(mat.mDiffuse), mFresnelR0(mat.mFresnelR0),
		mShiness(mat.mShiness), mAmbient(mat.mAmbient), mColor(mat.mColor), mColorIntensity(mat.mColorIntensity),
		mDiffuseMap(mat.mDiffuseMap), mNormalMap(mat.mNormalMap), mEffect(mat.mEffect), mEffectName(mat.mEffectName),mDirtyCallbackHandleTable(100,nullptr)
	{

		mDirtyCallbackHandleTable = mat.mDirtyCallbackHandleTable;




		for (int i = 0; i < _countof(mTextureTransformMatrix); ++i)
			mTextureTransformMatrix[i] = mat.mTextureTransformMatrix[i];
		SetDirtyFlag(true);

	}

	Material& Material::operator=(const Material& mat)
	{

		//Asset::operator=(mat);
		
		mType = mat.mType;
		mDiffuse = mat.mDiffuse;
		mFresnelR0 = mat.mFresnelR0;
		mShiness = mat.mShiness;
		mAmbient = mat.mAmbient;
		mColor = mat.mColor;
		mColorIntensity = mat.mColorIntensity;
		mDiffuseMap = mat.mDiffuseMap;
		mNormalMap = mat.mNormalMap;
		mEffect = mat.mEffect;
		mEffectName = mat.mEffectName;

		

		for (int i = 0; i < _countof(mTextureTransformMatrix); ++i)
			mTextureTransformMatrix[i] = mat.mTextureTransformMatrix[i];

		SetDirtyFlag(true);



		// TODO: 여기에 return 문을 삽입합니다.
		return *this;
	}

	Material& Material::operator=(Material&& mat)
	{
		// TODO: 여기에 return 문을 삽입합니다.
		Asset::operator=(std::move(mat));

		mType = mat.mType;
		mDiffuse = mat.mDiffuse;
		mFresnelR0 = mat.mFresnelR0;
		mShiness = mat.mShiness;
		mAmbient = mat.mAmbient;
		mColor = mat.mColor;
		mColorIntensity = mat.mColorIntensity;
		mDiffuseMap = mat.mDiffuseMap;
		mNormalMap = mat.mNormalMap;
		mEffect = mat.mEffect;
		mEffectName = mat.mEffectName;

		for (int i = 0; i < _countof(mTextureTransformMatrix); ++i)
			mTextureTransformMatrix[i] = mat.mTextureTransformMatrix[i];
		SetDirtyFlag(true);
		return *this;

	}

	
	void Material::SetType(MaterialType type)
	{
		mType = type;

		SetDirtyFlag(true);

	}

	void Material::SetDiffuseMap(Texture* texture)
	{
		mDiffuseMap = texture;
		if (mDiffuseMap.GetObjectID() == 0)
		{
			int a = 2;
		}
		SetDirtyFlag(true);
	}

	void Material::SetDiffuseMap(const TextureSmartPointer & texture)
	{
		mDiffuseMap = texture;
		if (mDiffuseMap.GetObjectID() == 0)
		{
			int a = 2;
		}
		SetDirtyFlag(true);
	}

	void Material::SetNormalMap(Texture* texture)
	{
		mNormalMap = texture;
		SetDirtyFlag(true);
	}

	void Material::SetNormalMap(const TextureSmartPointer& texture)
	{
		mNormalMap = texture;
		SetDirtyFlag(true);
	}

	void Material::SetEffect(Effect* effect)
	{
		mEffect = effect;
		SetDirtyFlag(true);
	}

	void Material::SetSpecular(const DirectX::XMFLOAT3& specular)
	{
		mSpecular = specular;
		SetDirtyFlag(true);
	}

	void Material::SetAmbient(const DirectX::XMFLOAT3& ambient)
	{
		mAmbient = ambient;
		SetDirtyFlag(true);
	}

	void Material::SetShiness(float shiness)
	{
		mShiness = shiness;
		SetDirtyFlag(true);
	}

	void Material::SetColor(const DirectX::XMFLOAT3& color)
	{


		mColor = color;
		SetDirtyFlag(true);
	}

	void Material::SetColorIntensity(const DirectX::XMFLOAT3& intensity)
	{

		mColorIntensity = intensity;
		SetDirtyFlag(true);
	}

	void Material::SetFresnelRO(const DirectX::XMFLOAT3& fresnelR0)
	{
		mFresnelR0 = fresnelR0;

	}

	void Material::SetDiffuse(const DirectX::XMFLOAT4& diffuse)
	{
		mDiffuse = diffuse;

	}

	void Material::SetEffectName(const std::string& effectName)
	{
		mEffectName = effectName;
		SetDirtyFlag(true);
	}


	MaterialType Material::GetType() const
	{
		return mType;
	}

	Texture* Material::GetDiffuseMap() const
	{
		return mDiffuseMap.GetPointer();
	}
	Texture* Material::GetNormalMap() const
	{
		return mNormalMap.GetPointer();
	}
	Effect* Material::GetEffect() const
	{

		return EffectManager::GetEffect(mEffectName);
		//return mEffect;
	}
	DirectX::XMFLOAT4 Material::GetDiffuse() const
	{
		return mDiffuse;
	}
	DirectX::XMFLOAT3 Material::GetFresnelR0() const
	{
		return mFresnelR0;
	}
	DirectX::XMFLOAT3 Material::GetSpecular() const
	{
		return mSpecular;
	}
	DirectX::XMFLOAT3 Material::GetAmbient() const
	{
		return mAmbient;
	}
	float Material::GetShiness() const
	{
		return mShiness;
	}
	const DirectX::XMFLOAT3& Material::GetColor() const
	{
		return mColor;
		// TODO: 여기에 return 문을 삽입합니다.
	}
	const DirectX::XMFLOAT3& Material::GetColorIntensity() const
	{
		return mColorIntensity;
	}
	const std::string& Material::GetEffectName() const
	{
		return mEffectName;
		// TODO: 여기에 return 문을 삽입합니다.
	}
	void XM_CALLCONV Material::SetTextureTransformMatrix(unsigned int index, DirectX::FXMMATRIX matrix)
	{
		DirectX::XMStoreFloat4x4(&mTextureTransformMatrix[index], matrix);
	}
	void Material::SetTextureTransformMatrix(unsigned int index, const DirectX::XMFLOAT4X4& matrix)
	{
		mTextureTransformMatrix[index] = matrix;
	}

	const DirectX::XMFLOAT4X4& Material::GetTextureTransformMatrix(int index) const
	{

		return mTextureTransformMatrix[index];
		// TODO: 여기에 return 문을 삽입합니다.
	}

	void Material::SetDirtyFlag(bool flag)
	{

		mDirtyFlag = flag;
		if (flag == true)
		{
			callDirtyCallbacks();
		}
	}

	bool Material::GetDirtyFlag() const
	{
		return mDirtyFlag;
	}

	QHANDLE Material::AddDirtyCallback(const std::function<void()> & callback)
	{
		QHANDLE handle =  mDirtyCallbackHandleTable.Register(callback);			
		mDirtyHandleVector.push_back(handle);
		return handle;
	}

	void Material::RemoveDirtyCallback(QHANDLE handle)
	{
		mDirtyHandleVector.erase(std::remove(mDirtyHandleVector.begin(), mDirtyHandleVector.end(), handle), mDirtyHandleVector.end());
		mDirtyCallbackHandleTable.Release(handle);
	
	}

	void Material::Serialize(const std::string& tag )
	{
		Asset::Serialize();
		unsigned long long id = 0;
		if (mDiffuseMap.GetPointer() != nullptr)
			id = mDiffuseMap->GetUniqueID();

		JsonParser::Write("Material_DiffuseMap", id);

		id = 0;
		if (mNormalMap.GetPointer() != nullptr)
			id = mNormalMap->GetUniqueID();

		JsonParser::Write("Material_NormalMap", id);
		id = 0;

		JsonParser::Write("Material_Diffuse", mDiffuse);
		JsonParser::Write("Material_Ambient", mAmbient);
		JsonParser::Write("Material_FresnelR0", mFresnelR0);
		JsonParser::Write("Material_Shiness", mShiness);
		JsonParser::Write("Material_Specular", mSpecular);
		JsonParser::Write("Material_Color", mColor);
		JsonParser::Write("Material_ColorIntensity", mColorIntensity);
		JsonParser::Write("Material_EffectName", mEffectName);

	}

	void Material::DeSerialize(const std::string& tag)
	{
		Asset::DeSerialize();

		unsigned long long id = 0;
		JsonParser::Read("Material_DiffuseMap", id);
		mDiffuseMap = id;

		JsonParser::Read("Material_NormalMap", id);
		mNormalMap = id;

	//	JsonParser::Read("Material_DiffuseMap", mDiffuseMap);
		//JsonParser::Read("Material_NormalMap", mNormalMap);
		JsonParser::Read("Material_Diffuse", mDiffuse);
		JsonParser::Read("Material_Ambient", mAmbient);
		JsonParser::Read("Material_FresnelR0", mFresnelR0);
		JsonParser::Read("Material_Shiness", mShiness);

		JsonParser::Read("Material_Specular", mSpecular);
		JsonParser::Read("Material_Color", mColor);
		JsonParser::Read("Material_ColorIntensity", mColorIntensity);

		JsonParser::Read("Material_EffectName", mEffectName);


	}

	bool Material::CheckType(const ContentItem* source)
	{
		bool ret = Asset::CheckType(source);

		if (!ret)
			return false;

		const Asset* asset = static_cast<const Asset*>(source);

		if (asset->GetAssetType() != EAssetType::eMaterial)
			return false;

		return true;
	}

	bool Material::InnerCopy(const ContentItem* source)
	{
		Asset::InnerCopy(source);
		
		const Material* mat = static_cast<const Material*>(source);

		mType = mat->mType;
		mDiffuse = mat->mDiffuse;
		mFresnelR0 = mat->mFresnelR0;
		mShiness = mat->mShiness;
		mAmbient = mat->mAmbient;
		mColor = mat->mColor;
		mColorIntensity = mat->mColorIntensity;
		mDiffuseMap = mat->mDiffuseMap;
		mNormalMap = mat->mNormalMap;
		mEffect = mat->mEffect;
		mEffectName = mat->mEffectName;

		for (int i = 0; i < _countof(mTextureTransformMatrix); ++i)
			mTextureTransformMatrix[i] = mat->mTextureTransformMatrix[i];

		SetDirtyFlag(true);

		return true;
	}

	void Material::callDirtyCallbacks()
	{
		const  std::vector<QHANDLE> &usingHandleVector =mDirtyCallbackHandleTable.GetUsingHandleVector();
		for (int i = 0; i < usingHandleVector.size(); ++i)
		{
			auto callback = mDirtyCallbackHandleTable.GetValue(usingHandleVector[i]);
			if((*callback)!=nullptr)
				(*callback)();
		}
	
	}


}
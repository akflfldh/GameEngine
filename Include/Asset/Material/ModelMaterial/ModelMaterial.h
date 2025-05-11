#pragma once



#include"Asset/Material/Material.h"
#include"ResourceSmartPointer/MaterialSmartPointer.h"
#include"ResourceSmartPointer/TextureSmartPointer.h"



#include"Core/CoreDllExport.h"
#include"ResourceSmartPointer/MaterialSmartPointer.h"

namespace Quad
{

	class CORE_API_LIB ModelMaterial:public Material
	{
	public:
		ModelMaterial();
		~ModelMaterial() = default;
		ModelMaterial(const Material& mat);
		
		ModelMaterial& operator=(const Material& mat);

		//virtual void SetType(MaterialType type) override;
		virtual void SetDiffuseMap(Texture* texture)override;
		virtual void SetDiffuseMap(const TextureSmartPointer &texture)override;
		virtual void SetNormalMap(Texture* texture)override;
		virtual void SetNormalMap(const TextureSmartPointer& texture)override;
		virtual void SetEffect(Effect* effect)override;
		virtual void SetSpecular(const DirectX::XMFLOAT3& specular)override;
		virtual void SetAmbient(const DirectX::XMFLOAT3& ambient)override;
		virtual void SetShiness(float shiness)override;
		virtual void SetColor(const DirectX::XMFLOAT3& color)override;
		virtual void SetColorIntensity(const DirectX::XMFLOAT3& intensity) override;
		virtual void SetFresnelRO(const DirectX::XMFLOAT3& fresnelR0)override;

		virtual void SetEffectName(const std::string& effectName)override;


		virtual Texture* GetDiffuseMap() const override;
		virtual Texture* GetNormalMap() const override;
		virtual Effect* GetEffect() const override;
		virtual DirectX::XMFLOAT4 GetDiffuse() const override;
		virtual DirectX::XMFLOAT3 GetFresnelR0() const override;
		virtual DirectX::XMFLOAT3 GetSpecular() const override;
		virtual DirectX::XMFLOAT3 GetAmbient() const override;
		virtual float GetShiness() const override;
		virtual const DirectX::XMFLOAT3& GetColor() const override;
		virtual const DirectX::XMFLOAT3& GetColorIntensity()const override;

		virtual const std::string& GetEffectName() const;






		void SetSourceMaterial(Material* mat);
		void SetSourceMaterial(unsigned long long id);
		void ResetDirtyFlag();


		virtual void Serialize(const std::string& tag) override;
		virtual void DeSerialize(const std::string& tag) override;

	private:



		bool mDiffuseDirtyFlag=false;
		bool mFresnelR0DirtyFlag=false;
		bool mShinessDirtyFlag=false;
		bool mSpecularDirtyFlag=false;
		bool mAmbientDirtyFlag=false;
		bool mColorDirtyFlag=false;
		bool mColorIntensityDirtyFlag=false;
		
		bool mDiffuseMapDirtyFlag=false;
		bool mNormalMapDirtyFlag=false;

		bool mEffectDirtyFlag=false;	


		MaterialSmartPointer mSourceMaterialPointer;


	};

}

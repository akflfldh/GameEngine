#pragma once


#include"header.h"
#include"Asset/Texture/Texture.h"
#include "Effect/Effect.h"
#include"Asset/Asset.h"
#include"Utility/HandleTable.h"
#include"ResourceSmartPointer/TextureSmartPointer.h"


#include"Core/CoreDllExport.h"


enum class MaterialType :uint32_t
{
	eOpaque=0,
	eTransparent,

};

namespace Quad
{


	class CORE_API_LIB Material:public Asset
	{
	public:

		//Material();
		Material(const std::string & name="");
		~Material() = default;

	
		Material(const Material& mat);
		Material(Material&& mat);

		Material& operator=(const Material& mat);
		Material& operator=(Material&& mat);



		virtual void SetType(MaterialType type);
		virtual void SetDiffuseMap(Texture* texture);
		virtual void SetDiffuseMap(const TextureSmartPointer  & texture);
		virtual void SetNormalMap(Texture* texture);
		virtual void SetNormalMap(const TextureSmartPointer& texture);
		virtual void SetEffect(Effect* effect);
		virtual void SetSpecular(const DirectX::XMFLOAT3& specular);
		virtual void SetAmbient(const DirectX::XMFLOAT3& ambient);
		virtual void SetShiness(float shiness);
		virtual void SetColor(const DirectX::XMFLOAT3& color);
		virtual void SetColorIntensity(const DirectX::XMFLOAT3& intensity);
		virtual void SetFresnelRO(const DirectX::XMFLOAT3& fresnelR0);
		virtual void SetDiffuse(const DirectX::XMFLOAT4& diffuse);

		virtual void SetEffectName(const std::string& effectName);

	
		MaterialType GetType()const;
		virtual Texture* GetDiffuseMap() const;
		virtual Texture* GetNormalMap() const;
		virtual Effect* GetEffect() const;
		virtual DirectX::XMFLOAT4 GetDiffuse() const;
		virtual DirectX::XMFLOAT3 GetFresnelR0() const;
		virtual DirectX::XMFLOAT3 GetSpecular() const;
		virtual DirectX::XMFLOAT3 GetAmbient() const;
		virtual float GetShiness() const;
		virtual const DirectX::XMFLOAT3& GetColor() const;
		virtual const DirectX::XMFLOAT3& GetColorIntensity()const;




		





		virtual const std::string& GetEffectName() const;

		void XM_CALLCONV SetTextureTransformMatrix(unsigned int index, DirectX::FXMMATRIX matrix);
		void SetTextureTransformMatrix(unsigned int index, const DirectX::XMFLOAT4X4& matrix);
		
		const DirectX::XMFLOAT4X4 & GetTextureTransformMatrix(int index) const;


		void SetDirtyFlag(bool flag);
		bool GetDirtyFlag()const;

		QHANDLE AddDirtyCallback(const std::function<void()> & callback);
		void RemoveDirtyCallback(QHANDLE handle);
		

		virtual void Serialize(const std::string& tag = "") override;
		virtual void DeSerialize(const std::string& tag = "") override;



		virtual bool CheckType(const ContentItem* source)override;
	protected:


		virtual bool InnerCopy(const ContentItem* source) override;


	private:
		void callDirtyCallbacks();




	private:
		MaterialType mType;


		DirectX::XMFLOAT4 mDiffuse = { 1.0f,1.0f,1.0f,1.0f };
		DirectX::XMFLOAT3 mFresnelR0 = { 0.1f,0.1f,0.1f };
		float mShiness = 0.2f;
		DirectX::XMFLOAT3 mSpecular = { 0.5f,0.5f,0.5f };
		DirectX::XMFLOAT3 mAmbient = { 0.2f,0.2f,0.2f };
		DirectX::XMFLOAT3 mColor = { 1.0f,1.0f,1.0f };
		DirectX::XMFLOAT3 mColorIntensity = { 1.0f,1.0f,1.0f };
		bool mDirtyFlag = true;
		//Texture* mDiffuseMap;		
		//Texture* mNormalMap ;
		TextureSmartPointer mDiffuseMap;
		TextureSmartPointer mNormalMap;

		//type 반/투명인가 불투명인가,

		//어떤 렌더링패스들을 사용해야하는가.
		//shader 
		Effect* mEffect;	//댕글링 포인터 문제가 있을수있다.
		std::string mEffectName="Default.effect";



		DirectX::XMFLOAT4X4 mTextureTransformMatrix[3]; //배열로 여러개존재하게할수도있을거같은데


		

		HandleTable<std::function<void()>> mDirtyCallbackHandleTable;
		std::vector<QHANDLE> mDirtyHandleVector;

	};

}
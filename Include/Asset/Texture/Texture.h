#pragma once


#include"header.h"
#include"DescriptorHeapType.h"
#include"Asset/Asset.h"

#include"Asset/Texture//TextureFormat.h"
#include"Asset/Texture/TextureDllExport.h"


namespace Quad
{



	class  Texture:public Asset
	{
	public:
		Texture(const std::string & name="");
		~Texture() = default;

		void SetTextureResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource);
		void SetViewIndex(ViewIndex viewIndex);

		Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const;
		ViewIndex GetViewIndex() const;

		

		void SetTextureFormat(ETextureFormat format);
		ETextureFormat GetTextureFormat()const;

		void SetTextureType(ETextureType type);
		ETextureType GetTextureType()const;


		virtual  void Serialize(const std::string& tag = "") override;
		virtual void DeSerialize(const std::string& tag = "") override;

		//CLASSNAME(Texture)
		const char* GetClassTypeName() const { return "Texture"; } ;

		virtual bool CheckType(const Quad::ContentItem* source) override;

	protected:

		virtual bool InnerCopy(const Quad::ContentItem* source) override;


	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
		ViewIndex mViewIndex =-1;



		ETextureFormat mTextureFormat;
		ETextureType mTextureType;







	};

}
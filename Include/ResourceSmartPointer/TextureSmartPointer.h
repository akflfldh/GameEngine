#pragma once




#include"Core/CoreDllExport.h"

#include"ResourceSmartPointer/IAssetSmartPointer.h"
#include"Asset/Texture/Texture.h"




namespace Quad
{
	class Texture;

	class CORE_API_LIB TextureSmartPointer :public IAssetSmartPointer
	{
	public:
		TextureSmartPointer();
		~TextureSmartPointer()=default;


		TextureSmartPointer(const TextureSmartPointer& pointer)=default;
		TextureSmartPointer(TextureSmartPointer&&) = default;
		TextureSmartPointer& operator=(TextureSmartPointer&&) = default;


		TextureSmartPointer(Texture* texture);


		TextureSmartPointer(unsigned long long textureID);

		TextureSmartPointer& operator=(const TextureSmartPointer& pointer);
		TextureSmartPointer& operator=(Texture* texture);
		TextureSmartPointer& operator=(unsigned long long id);

		Texture* operator->() const;

		//매개변수가 true라면 Table을 검사해서 그 id를 가지는 엔티티가 여전히 유효한지검사한다.
		Texture* GetPointer(bool checkTableFlag = false) const;

		unsigned long long GetObjectID() const;

		//table을 검사하여 이 id를 가지는 엔티티가 여전히 존재하는지 검사한다, 없으면 주소는nullptr과 id는 0으로 설정된다.
		void CheckAddress()const ;





	private:
		unsigned long long mID;
		mutable Texture* mTexture;

	};

}

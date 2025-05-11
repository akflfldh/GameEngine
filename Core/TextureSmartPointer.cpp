#include "ResourceSmartPointer/TextureSmartPointer.h"
#include"Asset/Texture/Texture.h"
#include"ResourceManager/TextureManager/TextureManager.h"

Quad::TextureSmartPointer::TextureSmartPointer()
	:mID(0),mTexture(nullptr)
{
}


Quad::TextureSmartPointer::TextureSmartPointer(Texture* texture)
	:mTexture(texture)
{
	if (mTexture == nullptr)
	{
		mID = 0;
	}else
		mID = mTexture->GetUniqueID();

}

Quad::TextureSmartPointer::TextureSmartPointer(unsigned long long textureID)
	:mID(textureID)
{
	if (mID == 0)
		mTexture = nullptr;
	else
		mTexture = TextureManager::GetTexture(textureID);


}

Quad::TextureSmartPointer& Quad::TextureSmartPointer::operator=(const TextureSmartPointer& pointer)
{
	mTexture = pointer.mTexture;
	mID = pointer.mID;

	return *this;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::TextureSmartPointer& Quad::TextureSmartPointer::operator=(Texture* texture)
{
	mTexture = texture;
	if (mTexture == nullptr)
		mID = 0;
	else
		mID = mTexture->GetUniqueID();


	return *this;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::TextureSmartPointer& Quad::TextureSmartPointer::operator=(unsigned long long id)
{
	mID = id;
	if (mID == 0)
		mTexture = nullptr;
	else
		mTexture = TextureManager::GetTexture(id);
	// manager
	

	return *this;

	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::Texture* Quad::TextureSmartPointer::operator->() const
{

	return mTexture;
}

Quad::Texture* Quad::TextureSmartPointer::GetPointer(bool checkTableFlag) const
{


	if (checkTableFlag)
	{
		//manager
		CheckAddress();
	}
	

	return mTexture;
	
}

unsigned long long Quad::TextureSmartPointer::GetObjectID() const
{

	return mID;
}

void Quad::TextureSmartPointer::CheckAddress() const
{
	//manager
	
	if (mID == 0)
		mTexture = nullptr;
	else
		mTexture = 	TextureManager::GetTexture(mID);


}

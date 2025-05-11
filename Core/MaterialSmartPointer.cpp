#include "ResourceSmartPointer/MaterialSmartPointer.h"
#include"ResourceManager/MaterialManager/MaterialManager.h"

#include"Asset/Material/Material.h"

Quad::MaterialSmartPointer::MaterialSmartPointer()
	:mID(0),mMaterial(nullptr)
{
}

Quad::MaterialSmartPointer::~MaterialSmartPointer()
{
}



Quad::MaterialSmartPointer::MaterialSmartPointer(Material* mat)
	:mMaterial(mat)
{
	if (mMaterial == nullptr)
		mID = 0;
	else
		mID = mat->GetUniqueID();

}

Quad::MaterialSmartPointer::MaterialSmartPointer(unsigned long long id)
	:mID(id)
{
	if (mID == 0)
		mMaterial = nullptr;
	else
		mMaterial = MaterialManager::GetMaterial(id);

}

Quad::MaterialSmartPointer& Quad::MaterialSmartPointer::operator=(const MaterialSmartPointer& pointer)
{

	mMaterial = pointer.mMaterial;
	mID = pointer.mID;

	return *this;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::MaterialSmartPointer& Quad::MaterialSmartPointer::operator=(Material* mat)
{
	mMaterial = mat;
	if (mat == nullptr)
		mID = 0;
	else
		mID = mat->GetUniqueID();

	return *this;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::MaterialSmartPointer& Quad::MaterialSmartPointer::operator=(unsigned long long id)
{

	mID = id;
	if (mID == 0)
		mMaterial = nullptr;
	else
		mMaterial = MaterialManager::GetMaterial(mID);

	// TODO: 여기에 return 문을 삽입합니다.
	return *this;
}

Quad::Material* Quad::MaterialSmartPointer::operator->() const
{
	return mMaterial;
}

Quad::Material& Quad::MaterialSmartPointer::operator*() const
{
	return *mMaterial;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::Material* Quad::MaterialSmartPointer::GetPointer(bool checkTableFlag)  const
{

	if (checkTableFlag)
	{
		CheckAddress();
	}


	return mMaterial;
}

unsigned long long Quad::MaterialSmartPointer::GetID() const
{
	return mID;
}

void Quad::MaterialSmartPointer::CheckAddress()  const
{
	if (mID == 0)
		mMaterial = nullptr;
	else
		mMaterial = MaterialManager::GetMaterial(mID);

}

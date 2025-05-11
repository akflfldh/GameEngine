#include "ResourceSmartPointer/MeshSmartPointer.h"

#include"Asset/Mesh/Mesh.h"
//#include"Utility/UniqueIDTable.h"

#include"ResourceManager/MeshManager.h"

Quad::MeshSmartPointer::MeshSmartPointer()
	:mID(0),mMesh(nullptr)
{
}

Quad::MeshSmartPointer::~MeshSmartPointer()
{
}

bool Quad::MeshSmartPointer::operator==(Mesh* mesh) const
{
	if (mMesh == mesh)
		return true;

	return false;
}

bool Quad::MeshSmartPointer::operator!=(Mesh* mesh) const
{
	return !(operator==(mesh));
}

Quad::MeshSmartPointer::MeshSmartPointer(Mesh* mesh)
	:mMesh(mesh)
{
	if (mMesh == nullptr)
		mID = 0;
	else
		mID = mesh->GetUniqueID();

}

Quad::MeshSmartPointer::MeshSmartPointer(unsigned long long id)
	:mID(id)
{
	if (mID == 0)
		mMesh = nullptr;
	else
		mMesh = MeshManager::GetMesh(id);
	
}

Quad::MeshSmartPointer& Quad::MeshSmartPointer::operator=(const MeshSmartPointer& pointer)
{

	mID = pointer.mID;
	mMesh = pointer.mMesh;
	return *this;


	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::MeshSmartPointer& Quad::MeshSmartPointer::operator=(Mesh* mesh)
{

	mMesh = mesh;
	if (mMesh == nullptr)
		mID = 0;
	else
		mID = mesh->GetUniqueID();

	return *this;

	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::MeshSmartPointer& Quad::MeshSmartPointer::operator=(unsigned long long id)
{
	mID = id;
	if (mID == 0)
		mMesh = nullptr;
	else
		MeshManager::GetMesh(mID);

	return *this;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::Mesh* Quad::MeshSmartPointer::operator->() const
{
	return mMesh;
}

Quad::Mesh* Quad::MeshSmartPointer::GetPointer(bool checkTableFlag) const
{

	if (checkTableFlag)
	{
		CheckAddress();
	}
	return mMesh;
}



unsigned long long Quad::MeshSmartPointer::GetID() const
{
	return mID;
}

void Quad::MeshSmartPointer::CheckAddress() const
{

	if (mID == 0)
		mMesh = nullptr;
	else
		mMesh = MeshManager::GetMesh(mID);

}


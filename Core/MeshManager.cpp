#include "ResourceManager/MeshManager.h"
#include"Asset/Mesh/StaticMesh.h"
#include"Asset/Mesh/SkinningMesh.h"
//#include"Mesh.h"

//#include"LineMesh.h"



namespace Quad
{
	//std::unordered_map<std::wstring, Mesh*> MeshManager::mMeshUnMap(MESH_MAXNUM);

	MeshManager::MeshManager() : mStaticMeshAllocator(MESH_MAXNUM),mSkinningMeshAllocator(MESH_MAXNUM)
		/*,mLineMeshAllocator(MESH_MAXNUM)*/
	{
	}

	void MeshManager::Initialize(DescriptorHeapManagerMaster* descriptorHeapManagerMaster)
	{
		mDescriptorHeapManagerMaster = descriptorHeapManagerMaster;
		mStaticMeshAllocator.Initialize();
		mSkinningMeshAllocator.Initialize();
		//mLineMeshAllocator.Initialize();




	}

	bool MeshManager::CheckDuplicate(const std::string& name)
	{
		return GetMesh(name) != nullptr ? true : false;
	}

	Mesh* MeshManager::GetMesh(const std::string& name)
	{
		auto instance = GetInstance();
		unsigned long long id = instance->mNameTable.GetID(name);
		return GetMesh(id);
	}

	Mesh* MeshManager::GetMesh(unsigned long long id)
	{
		auto instance = GetInstance();
		if (id == 0)
			return nullptr;

		return instance->mIDTable.GetElement(id);

	}

	Mesh* MeshManager::CreateMesh(const std::string& name, EMeshType meshType)
	{
		auto instance = GetInstance();

		//이름이 중복되면 - > 해시값은 당연히 중복 
		//이름이 중복되지않지만 해시값이 중복될수도있어  

		//확률은 낮으니깐 2번의 경우는 
		if (instance->GetMesh(name) == nullptr)
		{
			
			Mesh* newMesh = nullptr;
			switch (meshType)
			{
			case EMeshType::eStaticMesh:
				newMesh = (Mesh*)instance->mStaticMeshAllocator.GetInstance();
				break;
			case EMeshType::eSknningMesh:
				newMesh = (Mesh*)instance->mSkinningMeshAllocator.GetInstance();
				break;

		/*	case EMeshType::eLine:
				newInstance = mLineMeshAllocator.GetInstance();
				break;*/
			}
	

				newMesh->SetName(name);

				instance->mIDTable.Register(newMesh);
				instance->mNameTable.Register(name, newMesh->GetUniqueID());


			//if(newInstance==nullptr) throw exception; 
			return newMesh;
		}

		return nullptr;
	}

	Mesh* MeshManager::CreateMeshFromFile(const std::string& name, EMeshType meshType, unsigned long long id)
	{
		auto instance = GetInstance();

		//이름이 중복되면 - > 해시값은 당연히 중복 
		//이름이 중복되지않지만 해시값이 중복될수도있어  

		//확률은 낮으니깐 2번의 경우는 
		if (instance->GetMesh(name) == nullptr)
		{

			Mesh* newMesh = nullptr;
			switch (meshType)
			{
			case EMeshType::eStaticMesh:
				newMesh = (Mesh*)instance->mStaticMeshAllocator.GetInstance();
				break;
			case EMeshType::eSknningMesh:
				newMesh = (Mesh*)instance->mSkinningMeshAllocator.GetInstance();
				break;

				/*	case EMeshType::eLine:
						newInstance = mLineMeshAllocator.GetInstance();
						break;*/
			}


			newMesh->SetName(name);
			instance->mIDTable.RegisterFromFile(newMesh, id);
			instance->mNameTable.Register(name, id);

			return newMesh;
		}

		return nullptr;
	}



	void MeshManager::RemoveMesh(const std::string& name)
	{
		auto instance = GetInstance();
		unsigned long long id = instance->mNameTable.GetID(name);
		RemoveMesh(id);
	}

	void MeshManager::RemoveMesh(unsigned long long id)
	{
		auto instance = GetInstance();
		Mesh * mesh = instance->mIDTable.GetElement(id);

		instance->mIDTable.UnRegister(id);
		instance->mNameTable.UnRegister(mesh->GetName());

		switch(mesh->GetMeshType())
		{
		case EMeshType::eStaticMesh:
			instance->mStaticMeshAllocator.ReleaseInstance((StaticMesh*)mesh);
			break;


		case EMeshType::eSknningMesh:
			instance->mSkinningMeshAllocator.ReleaseInstance((SkinningMesh*)mesh);

			break;
		}



	}

}
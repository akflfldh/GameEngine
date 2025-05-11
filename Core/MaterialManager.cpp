#include "ResourceManager/MaterialManager/MaterialManager.h"
#include"Asset/Material/Material.h"


namespace Quad
{


	//std::unordered_map<std::wstring, Material*> MaterialManager::mMaterialUnMap(MATERIAL_MAXNUM);

	MaterialManager::MaterialManager()
		:mMaterialAllocator(MATERIAL_MAXNUM)
	{

		mMaterialAllocator.Initialize();
	}

	void MaterialManager::Initialize()
	{
	

	}



	Material* MaterialManager::GetMaterial(const std::string& name) 
	{
		
		auto instance = GetInstance();
		unsigned long long id = instance->mNameTable.GetID(name);
		return GetMaterial(id);
	}

	Material* MaterialManager::GetMaterial(unsigned long long id) 
	{
		auto instance = GetInstance();
		if (id == 0)
			return nullptr;
		else
			return instance->mIDTable.GetElement(id);
	
	}

	void MaterialManager::ReMoveMaterial(const std::string& name)
	{
		auto instance = GetInstance();
		unsigned long long id = instance->mNameTable.GetID(name);
		
		ReMoveMaterial(id);
	}

	void MaterialManager::ReMoveMaterial(unsigned long long id)
	{
		auto instance = GetInstance();
		if (id == 0)
			return;

		Material* mat = instance->mIDTable.GetElement(id);


		instance->mIDTable.UnRegister(id);
		instance->mNameTable.UnRegister(mat->GetName());


		instance->mMaterialAllocator.ReleaseInstance(mat);



	}

	Material* MaterialManager::CreateMaterial(const std::string& name)
	{
		auto instance = GetInstance();
		unsigned long long id = instance->mNameTable.GetID(name);
		if (id != 0)
			return nullptr;


		Material * newMaterial =  instance->mMaterialAllocator.GetInstance();
		newMaterial->SetName(name);

		instance->mIDTable.Register(newMaterial);
		instance->mNameTable.Register(name, newMaterial->GetUniqueID());


		return newMaterial;
		
	}

	Material* MaterialManager::CreateMaterialFromFile(const std::string& name, unsigned long long id)
	{
		auto instance = GetInstance();

		if (instance->mIDTable.GetElement(id) == nullptr)
		{

			Material* newMaterial = instance->mMaterialAllocator.GetInstance();

			newMaterial->SetName(name);

			instance->mIDTable.RegisterFromFile(newMaterial, id);
			instance->mNameTable.Register(name, id);

			return newMaterial;
		}

		return nullptr;
	}


}
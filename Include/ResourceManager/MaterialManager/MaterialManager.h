#pragma once

#include"Asset/Material/Material.h"
#include"Allocator/DestructingPoolAllocator.h"		// 
#include"Utility/Singleton.h"
#include"Utility/UniqueIDTable.h"
#include"Utility/NameTable.h"


#include"Core/CoreDllExport.h"

#define MATERIAL_MAXNUM 100000
#define EngineMode
#define EditorMode

namespace Quad
{

	class CORE_API_LIB  MaterialManager:public Singleton<MaterialManager>
	{
		//friend class ResourceStorer;
		friend class ResourceController;
#ifdef EngineMode
		friend class EngineModeDirector;
		friend class EditorAssetImportVersionModule;
#endif

#ifdef EditorMode

		friend class EditorModeDirector;
#endif

	public:
		MaterialManager();
		~MaterialManager() = default;

		void Initialize();

	//	bool  CheckDuplicate(const std::string& name);


		static	Material* GetMaterial(const std::string& name) ;
		static Material* GetMaterial(unsigned long long id);

		static void ReMoveMaterial(const std::string& name);
		static void ReMoveMaterial(unsigned long long id);


		static Material* CreateMaterial(const std::string& name);		//처음생성하면 Material에 아무것도 설정되지않을상태
		static Material* CreateMaterialFromFile(const std::string& name, unsigned long long id);


	private:
		//static std::unordered_map<std::wstring, Material*> mMaterialUnMap;

		DestructingPoolAllocator<Material> mMaterialAllocator;


		UniqueIDTable<Material> mIDTable;
		NameTable mNameTable;
		





	};

}


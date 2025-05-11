#pragma once

#include"header.h"
#include"HeapManager/DescriptorHeapManagerMaster.h"
//#include"Mesh.h"

#include"Allocator/DestructingPoolAllocator.h"
#include<functional>
#include"RawResourceType.h"
#include"Asset/Mesh/StaticMesh.h"
#include"Asset/Mesh/SkinningMesh.h"
#include"Utility/Singleton.h"

#include"Utility/UniqueIDTable.h"
#include"Utility/NameTable.h"
//#include"LineMesh.h"


#include"Core/CoreDllExport.h"

#include"ResourceStorer.h"

#define MESH_MAXNUM 100000

#define EngineMode
#define EditorMode
namespace Quad
{
	class CORE_API_LIB  MeshManager:public Singleton<MeshManager>
	{
		friend class ResourceStorer;
		friend class ResourceController;

#ifdef EngineMode
		friend class EngineModeDirector;
		friend class EditorAssetImportVersionModule;
#endif

#ifdef EditorMode

		friend class EditorModeDirector;
#endif


	public:
		MeshManager();
		~MeshManager() = default;

		void Initialize(DescriptorHeapManagerMaster* descriptorHeapManagerMaster);


		bool  CheckDuplicate(const std::string& name);
		static Mesh* GetMesh(const std::string& name);
		static Mesh* GetMesh(unsigned long long id);
		static Mesh* CreateMesh(const std::string& name, EMeshType meshType);		//처음생성하면 Mesh에 아무것도 설정되지않을상태
		static Mesh* CreateMeshFromFile(const std::string& name, EMeshType meshType,unsigned long long id );

		static void RemoveMesh(const std::string& name);
		static void RemoveMesh(unsigned long long id);
		


	private:
		DescriptorHeapManagerMaster* mDescriptorHeapManagerMaster;
		

		UniqueIDTable<Mesh>mIDTable;
		NameTable mNameTable;




		DestructingPoolAllocator<StaticMesh> mStaticMeshAllocator;
		DestructingPoolAllocator<SkinningMesh> mSkinningMeshAllocator;
		//DestructingPoolAllocator<LineMesh> mLineMeshAllocator;






	};

}
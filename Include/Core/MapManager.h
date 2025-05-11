#pragma once


#include"Map/Map.h"
#include"Allocator/DestructingPoolAllocator.h"	
#include"Utility/Singleton.h"
#include"Core/CoreDllExport.h"
#define MAP_MAXNUM 100
namespace Quad
{
	class CORE_API_LIB MapManager:public Singleton<MapManager>
	{
	public:
		MapManager();
		~MapManager() = default;

		void Initialize();

		bool  CheckDuplicate(const std::string& name);
		Quad::Map* GetMap(const std::string& name);
		Quad::Map* CreateMap(const std::string& name);		//처음생성하면 Material에 아무것도 설정되지않을상태



	private:
		std::unordered_map<std::string, Quad::Map*> mMapUnMap;
		DestructingPoolAllocator<Quad::Map> mMapAllocator;


	};

}
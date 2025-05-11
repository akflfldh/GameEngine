#include "MapManager.h"
namespace Quad
{
	MapManager::MapManager() :mMapAllocator(MAP_MAXNUM)
	{


	}

	void MapManager::Initialize()
	{
		mMapAllocator.Initialize();
	}

	bool MapManager::CheckDuplicate(const std::string& name)
	{
		return GetMap(name) != nullptr ? true : false;
	}

	Quad::Map* MapManager::GetMap(const std::string& name)
	{
		std::unordered_map<std::string, Quad::Map*>::const_iterator it = mMapUnMap.find(name);
		return it != mMapUnMap.cend() ? it->second : nullptr;

	}

	Quad::Map* MapManager::CreateMap(const std::string& name)
	{
		if (!CheckDuplicate(name))
		{
			Quad::Map* newInstance = mMapAllocator.GetInstance();

			newInstance->SetName(name);
			if (newInstance != nullptr)
				mMapUnMap.insert({ name,newInstance });
			return newInstance;
		}
		return nullptr;
	}

}

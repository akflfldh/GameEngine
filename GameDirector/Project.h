#pragma once

#include"Singleton.h"
#include"MapMetaData.h"
#include<Vector>
#include<unordered_map>


namespace Quad
{
	class Map;

	class Project:public Singleton<Project>
	{
	public:
		Project();
		~Project();


		void AddMap(const std::string& mapName,Quad::Map* map);
		Map* GetMap(const std::string& mapName);
	
	private:

		
		std::vector<MapMetaData*> m3DMapMetaDataVector;
		std::vector<MapMetaData*> mUiMapMetaDataVector;

	
		std::unordered_map<std::string, Map*> mMapTable;
	};
}


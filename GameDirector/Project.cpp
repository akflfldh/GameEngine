#include "Project.h"



Quad::Project::Project()
{

}


Quad::Project::~Project()
{


}

void Quad::Project::AddMap(const std::string& mapName, Quad::Map* map)
{

	mMapTable[mapName] = map;

}

Quad::Map* Quad::Project::GetMap(const std::string& mapName)
{
	
	std::unordered_map<std::string,Map*>:: iterator iter  = mMapTable.find(mapName);

	return  iter != mMapTable.end() ? iter->second : nullptr;
}




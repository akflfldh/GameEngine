#pragma once

#include"Predeclare.h"
#include<string>
namespace Quad
{
	class EditorMap;
	class EditorMapManager
	{
	public:

	
		static Map* CreateGamePlayMap(System * system ,const std::string & mapName , bool isRuntimeMap);

		static Map* CreateEngineSystemMap(System* system, const std::string& mapName, bool isRuntimeMap = true);

		
		static void RemoveMap(Map* map);


	private:

	};
}

#pragma once

#include"Predeclare.h"
#include"Utility/Singleton.h"

#include<string>
#include"Core/CoreDllExport.h"

namespace Quad
{
	

	class CORE_API_LIB GameMapSystem
	{
	public:

		//항상 runtime GameMap을 만드는것이다.
		//EditorSystem과 다르게


		static Map* CreateMap(System* system, const std::string& mapName, bool isUserGameSystem);//초기화까지수행



	private:



	};

}

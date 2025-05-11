#pragma once

#include"Predeclare.h"
#include<string>
namespace Quad
{
	class EditorMap;
	class EditorSystem
	{
	public:

		//유저의 게임플레이 시스템에서 사용하는지에대한 여부(false라면 에디터의 다른시스템에서 사용하기위한 맵생성)
		static Map* CreateMap(System * system ,const std::string & mapName , bool isRuntimeMap, bool isUserGameSystem );//초기화까지수행


	private:

	};
}

#pragma once

#include<string>
namespace Quad
{
	class Map;
	class System;
	class GameMapInstanceGenerator
	{
	public:
		GameMapInstanceGenerator() = delete;
		~GameMapInstanceGenerator() = default;


		static Map* CreateMap(System* system, const std::string& mapName);


	private:


	};
}

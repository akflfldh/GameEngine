#pragma once
#include<string>

#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB MapMetaData
	{
	public:

		//void SetMapPath(const std::string& mapPath);
		void SetMapName(const std::string& mapName);

		//const std::string GetMapPath() const;
		const std::string GetMapName()const;

		void Serialize();
		void DeSerialize();

	private:
	//	std::string mMapPath;
		std::string mMapName;
	

	};

}

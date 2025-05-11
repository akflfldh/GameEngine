#pragma once

//#define EngineMode
//
//#ifdef EngineMode
#include<string>
namespace Quad
{
	class EngineModeDirector
	{
	public:
		~EngineModeDirector() = default;

		static void SaveAsset();



	private:
		EngineModeDirector() = default;


		static void SaveMesh(const std::string & assetPath);

		static void SaveMaterial(const std::string & assetPath);
		static void SaveTexture(const std::string & assetPath);


	};

}


//#endif
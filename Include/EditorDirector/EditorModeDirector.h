#pragma once


#include"Predeclare.h"
#include<string>
#include<vector>

#include"Utility/Singleton.h"

namespace Quad
{


	class EditorModeDirector:public Singleton<EditorModeDirector>
	{
	public:
		EditorModeDirector() = default;
		~EditorModeDirector() = default;


		void SetGamePlayWindowChromeSystem(WindowChromeSystem* gamePlayWindowChromeSystem);

		

		static std::vector<Quad::Asset*>& LoadAsset(const std::string & assetPath);
		//EdiorMode에서는 Editor의 Asset을 Save하지않는다. 엔진의 Asset을 결정하는건 EngineMode
		//static void SendAssetToFileSystem();



		//gameWindowLayoutSystem에서 사용할 엔티티들을 생성해준다.
		static void InitGameWindowLayoutSystem();



	private:
		

		static void LoadEffect(const std::string& assetPath);
		static void LoadMesh(const std::string& assetPath);
		static void LoadMaterial(const std::string& assetPath);
		static void LoadTexture(const std::string& assetPath);


		static std::vector<Asset*> mAssetVector;



		WindowChromeSystem* mGamePlayWindowChromeSystem;

	};


}
#pragma once

#include"header.h"
#include"IProgramDirector.h"

#include"Utility/Singleton.h"

namespace Quad
{
	class GameWindow;
	class GameWindowController;
	class MapMetaData;
	class Project;
	class GameDirector :public Quad::IProgramDirector,Quad::Singleton<GameDirector>
	{
	public:
		GameDirector();

		virtual void Initialize()override;
		virtual void PreUpdate(float deltaTime)override;
		virtual void Update(float deltaTime)override;
		virtual void EndUpdate(float deltaTime)override;
		virtual void Draw()override;


	private:

		void ReadMapMetaDataFile(const std::string& mapMetaDatFilePath, std::vector<MapMetaData*>& o3DMapMetaDataVector);



	private:
		GameWindow* mGameWindow;
		GameWindowController* mGameWindowController;

		Project* mProject;
		std::string mEditorPathA;
		std::wstring mEditorPathW;
	};


}
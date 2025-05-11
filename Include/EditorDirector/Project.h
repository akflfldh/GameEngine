#pragma once
#include<vector>
#include"Predeclare.h"
#include<string>
namespace Quad
{
	class Map;


	class Project
	{
		friend class ProjectDirector;

	public:
		~Project();


		MapMetaData* GetMapMetaData(const std::string& mapName) const;

		void CreateMapMetaData(const std::string& mapPath, const std::string& mapName);


		void SaveProject();
		void LoadProject();


		void Serialize();
		void DeSerialize();


		void SetName(const std::string& name);
		const std::string& GetName()const;

		
		bool SaveCurrentMap();

		void SaveMapCommonData();

	

		void LoadMapCommonData();

	private:
		void SaveEditorCurrentMapData(const std::string& projectFolderPath, const std::string& currMapName);//현재 map과 관련된 editor 데이터 저장
		void LoadEditorCurrentMapData(const std::string& projectFolderPath, const std::string& currMapName);


	private:
		
		Project();
		std::string mName;
		static Project* CreateDefaultProject() ;



		std::vector<MapMetaData*> mSystemMapMetaDataVector;
		//std::vector<MapMetaData*> mUiSystemMapMetaDataVector;


		int mCurrent3DMapIndex;
		int mCurrentUiMapIndex;


	};

}

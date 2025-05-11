#pragma once
#include"Predeclare.h"
#include"Utility/Singleton.h"

#include"EditorDirector/ProjectMetaData.h"

#include<vector>
#include<string>

#include"FrameWindowMainSceneDirector.h"
namespace Quad
{

	class EditorMap;
	class JsonParser;

	class ProjectDirector :public Singleton<ProjectDirector>
	{
	public:
		ProjectDirector();
		~ProjectDirector();

		//프로젝트메타파일로드하기, 화면 프로젝트 선택씬 구성하기 등 초기작업수행 //프레임윈도우초기화이후 초기화수행 
		void Initialize(FrameWindow* frameWindow, std::vector<DockingWindowController*> normalWindowControllerVector, PopupWindowController* popupWindowController,FrameWindowController* frameWindowController,FrameWindowUiSystem * frameWindowUiSystem);

		static bool CreateProject(const std::string& projectName, const std::string& projectParentPath);

		 
		static ProjectMetaData* GetProjectMetaData(const std::string& projectName);
		static ProjectMetaData* GetCurrentProjectMetaData() ;


		static const std::vector<ProjectMetaData*>& GetProjectMetaDataVector();
		


		static Project* GetCurrentProject();


		static void SelectProject(ProjectMetaData * projectMetaData);


		void SaveProjectMetaData();
		void LoadProjectMetaData();


	

		//입력된 프로젝트 경로가 문제없다고 취급한다(즉 경로의 적법성을따지지않는다)
		//bool StartCreatingProject(const std::string& projectName, const std::string& projectParentPath);


		//프로젝트생성 또는 로드후, 설정된 맵에 에디터전용오브젝트들을 추가하는작업을 수행한다.(카메라,광원,기즈모,그리드 등 생성하는작업) (현재 GamePlayWindow,GamePlayUiWindow에 설정된 프로젝트의 맵에 넣는다)
		static void InitProjectEditorMode();
		
		//프로젝트 생성시 유저에게 제공되는 기본 objects
		static void GenerateProjectDefaultUserObjects();

		static bool SaveCurrentMap();


		static void SetOhterWindowSystemOn();
		static void SetOhterWindowSystemOff();

		static void SetOtherWindowVisibleOnOff(bool flag);


	private:
		static bool CreateProjectDirectoryHierarchy(const std::string& projectParentPath);

		



		static EditorMap* CreateDefaultMap(const std::string & mapName);
	//	static EditorMap* CretaeDefaultUiMap(const std::string& mapName);



		static void CopyDefaultAssetToUser();
		static void CopyDefaultTextureAssetToUser(JsonParser * parser,const std::string & editorAssetPath);
		static void CopyDefaultMaterialAssetToUser(JsonParser * parser, const std::string& editorAssetPath);
		static void CopyDefaultMeshAssetToUser(JsonParser * parser, const std::string& editorAssetPath);
		static void CopyDefaultEffectToUser(JsonParser* parser,const std::string& editorAssetPath);
		static void LoadUserDefaultEffect();



	private:
		//CreatingProejctWindowController* mWindowController;
		FrameWindow* mFrameWindow;
		std::vector<DockingWindowController*> mNormalWindowControllerVector;
		PopupWindowController* mPopupWindowController;

		FrameWindowController* mFrameWindowController;
	


		std::vector<ProjectMetaData*> mProjectMetaDataVector;
		Project* mCurrProject;
		FrameWindowMainSceneDirector mFrameWindowMainSceneDirector;
	};
}

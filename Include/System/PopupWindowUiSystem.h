#pragma once

#include"Predeclare.h"
#include"System.h"
#include"Utility/Singleton.h"

#include<unordered_map>
#include<vector>
namespace Quad
{
	class PopupWindowUiSystem:public System,public Singleton<PopupWindowUiSystem>
	{
	public:
		PopupWindowUiSystem();
		virtual ~PopupWindowUiSystem();

		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map);
		virtual void Update(float deltaTime, bool playMode = true);
		virtual void OnResize(UINT clientWidth, UINT clientHeight);
		virtual void AddEntity(Object* object, int mapLayer = 0, bool egineEditObject = false) override;
	//	virtual void GetEntity(std::vector<Object*>& oObjectVector)override;
		virtual void HandleEvent(Event* event) override;
		//virtual const std::vector<MapLayer>& GetEntity()override;

		void ActivatePanel(int popupPanelID);
		std::pair<int, int> GetPopupPanelSize(int popupPanelID) const;
		

		void RegisterRootPanel(int popupPanelID, PanelUiEntity* rootPanel);




	private:
		void CreateFileUiPanel();




	private:
		//std::unordered_map<std::wstring , PanelUiEntity*> mRootPanelUnMap;

		std::vector<PanelUiEntity *>mRootPanelVector;

		int mCurrentPopupPanelID;

	};

}

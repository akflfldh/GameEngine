#pragma once

#include"PanelUiEntity.h"

#include"EditorMacro.h"

namespace Quad
{
	class ListPanel;

	class ComboBoxPanel:public  PanelUiEntity
	{
	public:
		ComboBoxPanel(const std::string& name = "");
		virtual ~ComboBoxPanel();

		virtual void Initialize() override;

		static ComboBoxPanel* Create(Map* map, int mapLayerIndex = 0, float width = 100 ,int listNum = 3 , float listItemHeight = 20  );


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

		virtual void SetDrawFlag(bool flag) override;


		void SetText(const std::string& text, int listItemIndex);
		void RegisterLButtonDownCallback(int listItemIndex ,const std::function<void(Event*)>& callback);


		void ClickDonwButton();
		void ClickListItem(int index);
		int GetCurrentSelectedListItemIndex() const;
		const char* GetCurrentSelectedListItemText() const;


		CLASSNAME(ComboBoxPanel)
	protected:
		void InitCreating(float width, int listNum, float listItemHeight);

		virtual void HandleLButtonDown(Event* pEvent) override;
		
		void ExpandListPanel();
		void CollapseListPanel();

	private:
		float mWidth;
		float mListItemHeight;
		int mListNum;
		int mCurrentListItemIndex;
		
		PanelUiEntity* mHeader;
		TextBoxPanel* mHeaderTextBoxPanel;
		ListPanel* mListPanel;
		bool mDownButtonState;
		std::vector<TextBoxPanel*> mTextBoxPanelListItemVector;
		


	};
	REGISTEREDITORCLASS(ComboBoxPanel)

}

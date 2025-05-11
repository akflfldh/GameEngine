#pragma once

#include"Predeclare.h"
#include"PanelUiEntity.h"
#include"EditorMacro.h"

namespace Quad
{


	class SubMeshListPanel :public PanelUiEntity
	{
	public:
		SubMeshListPanel(const std::string& name="");
		virtual ~SubMeshListPanel();

		virtual void Initialize() override;

		static SubMeshListPanel* Create(Map* map, int mapLayerIndex, float width, float height);
 		

		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

		CLASSNAME(SubMeshListPanel)
		CLASSSIZE(SubMeshListPanel)
		void SetListNum(int i);
		int GetListNum() const;



		virtual void SetDrawFlag(bool flag) override;
		virtual void SetActiveFlag(bool flag)override;
		virtual void SetStencilRefValue(unsigned char value) override;
		//virtual void SetSize(float width, float height)override;
	protected:
		void InitCreating(float width, float height);


	private:
	
		void HandleLButtonDown(Event* pEvent) override;
		void HandleGameObjectSelect(Event* pEvent);
		void HandleHover(Event* pEvent)override;

		TextBoxPanel* mTextBoxPanel;

		Object* mSelectedObject;
		int mListNum; //서브메시인덱스이기도하다.
		int mCurrentSubMeshindex;

	};
	REGISTEREDITORCLASS(SubMeshListPanel)


}
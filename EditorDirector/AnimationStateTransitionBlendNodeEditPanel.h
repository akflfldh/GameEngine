#pragma once


#include"PanelUiEntity.h"
#include"EditorMacro.h"

#include"TextBoxPanel.h"
#include"Singleton.h"

namespace Quad
{
	class AnimationStateTransitionBlendNode;
	class ComboBoxPanel;
	class AnimationStateTransitionBlendNodeEditPanel :public PanelUiEntity,public Singleton< AnimationStateTransitionBlendNodeEditPanel>
	{
	public:
		AnimationStateTransitionBlendNodeEditPanel(const std::string& name = "");
		virtual ~AnimationStateTransitionBlendNodeEditPanel();

		virtual void Initialize() override;

		static AnimationStateTransitionBlendNodeEditPanel* Create(Map* map, int mapLayerIndex = 0);


		//블랜드노드선택했을때 호출한다.
		void SetDestBlendNode(AnimationStateTransitionBlendNode* blendNode);


		static void NotifyStateNodeConnect(AnimationStateTransitionBlendNode* blendNode);




		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;





		void NotifyTransitionDurationToBlendNode();

		//전이조건판넬이 적절히 다 채워졌다면  이함수가 blendNode에게 알려서 에니메이션컴포넌트에 전이조건을 설정할것이다.
		void SetTransitionCondition();

		CLASSNAME(AnimationStateTransitionBlendNodeEditPanel)



	protected:
		void InitCreating();

	private:
		Quad::PanelUiEntity* CreateTransitionConditionEditKeyboardPanel();
		Quad::PanelUiEntity* CreateTransitionConditionEditMousePanel();



	private:
		AnimationStateTransitionBlendNode* mDestBlendNode;
		TextBoxPanel* mSourceStateNameTextBoxPanel;
		TextBoxPanel* mDestStateNameTextBoxPanel;

		TextBox* mTransitionDurationTextBox;
		TextBoxPanel* mInputTypeTextBoxPanel;

		PanelUiEntity* mTransitionConditionEditKeyboardMainPanel;


		//키보드 입력에따른 조건설정들
		ComboBoxPanel* mKeyStateComboBox;
		TextBox* mKeyValueTextBox;

		int mInputDevice; //0 -없음, 1 키보드 2,마우스

	};
	REGISTEREDITORCLASS(AnimationStateTransitionBlendNodeEditPanel)

}

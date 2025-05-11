#pragma once

#include"PanelUiEntity.h"
#include"EditorMacro.h"
#include"Utility/Singleton.h"
#include"AnimationNodeType.h"
namespace Quad
{
	class AnimationStateTransitionBaseNode;
	class TextBoxPanel;
	class AnimationStateTransitionBlendNodeEditPanel;

	class AnimationStateTransitionNodeEditDetailMainPanel:public PanelUiEntity,Singleton< AnimationStateTransitionNodeEditDetailMainPanel>
	{
	public:
		AnimationStateTransitionNodeEditDetailMainPanel(const std::string& name = "");
		virtual ~AnimationStateTransitionNodeEditDetailMainPanel();

		virtual void Initialize() override;

		static AnimationStateTransitionNodeEditDetailMainPanel* Create(Map* map, int mapLayerIndex = 0);
		static void NotifySelectNode(AnimationStateTransitionBaseNode* node);

		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		
		CLASSNAME(AnimationStateTransitionNodeEditDetailMainPanel)
	protected:
		void InitCreating();
		void CreateBlendNodeEditPanel();

		void OffPreSelectedNodeEditPanel(EAnimationNodeType nodeType);

		virtual void HandleWindowResize(Event* event);

	private:


		AnimationStateTransitionBaseNode* mSelectedNode;
		TextBoxPanel* mSelectedNodeTypeTextBoxPanel;
		AnimationStateTransitionBlendNodeEditPanel* mBlendNodeEditPanel;

	};
	REGISTEREDITORCLASS(AnimationStateTransitionNodeEditDetailMainPanel)


}
#pragma once



#include"Predeclare.h"

#include"PanelUiEntity.h"
#include"EditorMacro.h"
namespace Quad
{
	class AnimationStateTransitionStartNode;
	class AnimationStateTransitionEndNode;

	class AnimationStateTransitionNode:public PanelUiEntity
	{
	public:
		AnimationStateTransitionNode(const std::string& name = "");
		virtual ~AnimationStateTransitionNode();

		virtual void Initialize() override;

		static AnimationStateTransitionNode* Create(Map* map, int mapLayerIndex = 0);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

		CLASSNAME(AnimationStateTransitionNode)
			CLASSSIZE(AnimationStateTransitionNode)

			void SetIsCreatingFlag(bool flag);
		void SetStartLinkNodePosWorld(const DirectX::XMFLOAT3& startPosW);
	protected:
		void InitCreating();

	private:


		virtual void HandleMouseMove(Event* pEvent) override;
		virtual void HandleLButtonUp(Event* pEvent) override;

		//Line* mLine;
		//AnimationStateTransitionStartNode* mStartNode;
		//AnimationStateTransitionEndNode* mEndNode;
		DirectX::XMFLOAT3 mStartPosW;
		//생성중인지를 나타내는(정확히 첫 생성후 마우스를 up하기전까지를 나타냄)
		bool mIsCreating;
	};
	REGISTEREDITORCLASS(AnimationStateTransitionNode)

}

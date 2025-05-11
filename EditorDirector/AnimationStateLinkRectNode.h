#pragma once


//에니메이션상태노드들에 있는 전이노드들이 출발,도착지점으로 사용할 노드
#include"PanelUiEntity.h"
#include"EditorMacro.h"
namespace Quad
{
	class AnimationStateTransitionNode;
	class AnimationStateNode;
	class AnimationStateLinkRectNode:public PanelUiEntity
	{
	public:
		AnimationStateLinkRectNode(const std::string& name = "");
		virtual ~AnimationStateLinkRectNode();


		virtual void Initialize() override;

		static AnimationStateLinkRectNode* Create(Map* map, int mapLayerIndex = 0);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		virtual void SetDrawFlag(bool flag)override;

		CLASSNAME(AnimationStateLinkRectNode)
		CLASSSIZE(AnimationStateLinkRectNode)

	protected:
		void InitCreating();

	private:

		virtual void HandleLButtonDown(Event* pEvent) override;
		virtual void HandleLButtonUp(Event* pEvent) override;
		virtual void HandleMouseMove(Event* pEvent) override;
		virtual void HandleHover(Event* pEvent) override;



		AnimationStateTransitionNode* mCurrStateTransitionNode;//클릭-드래그해서 생성하여 여전히 캡처되고있는 상태전이노드(마우스 LUp하는순간 제거되거나,전이상태가 만들어지거나)
		AnimationStateNode* mTargetNode;//라인을 드래그해서 놓았을때 ,그순간의 목적 노드를 나타내는용

		Spline* mSpline; 


	};
	REGISTEREDITORCLASS(AnimationStateLinkRectNode)

}

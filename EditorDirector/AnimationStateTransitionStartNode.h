#pragma once


#include"Predeclare.h"
#include"PanelUiEntity.h"
#include"EditorMacro.h"
namespace Quad
{
	class AnimationStateTransitionNode;
	class AnimationStateNode;
	class AnimationStateTransitionBaseNode;
	class Spline;

	class AnimationStateTransitionStartNode:public PanelUiEntity
	{
	public:
		AnimationStateTransitionStartNode(const std::string& name = "");
		virtual ~AnimationStateTransitionStartNode();

		virtual void Initialize() override;

		static AnimationStateTransitionStartNode* Create(Map* map, int mapLayerIndex);

		

		virtual void Update(float deltaTime)override;
		virtual void EndUpdate(float deltaTime)override;
		virtual void OnEvent(Event* event) override;


		virtual void Serialize() override;
		virtual void DeSerialize() override;


		//startNode에 연결되어 시작하는 스플라인
		//제거는 호출한쪽에서 책임
		//beziercurve 기반
		Spline* CreateSpline();


		void AddLinkedSpline(Spline* spline);

		void SetUpdateSplineFlag();
		void UpdateSpline();
		


		CLASSNAME(AnimationStateTransitionStartNode)
	protected:
		void InitCreating();

		virtual void HandleLButtonDown(Event* pEvent) override;
		virtual void HandleLButtonUp(Event* pEvent) override;
		virtual void HandleMouseMove(Event* pEvent) override;
		virtual void HandleHover(Event* pEvent) override;

	private:

		AnimationStateTransitionNode* mCurrStateTransitionNode;//클릭-드래그해서 생성하여 여전히 캡처되고있는 상태전이노드(마우스 LUp하는순간 제거되거나,전이상태가 만들어지거나)
		AnimationStateTransitionBaseNode* mTargetNode;//라인을 드래그해서 놓았을때 ,그순간의 목적 노드를 나타내는용

		Spline* mSpline;
		bool mSendLbuttonUpEventFlag;



		std::vector<Spline*> mLinkedSplineVector;

		bool mSplineUpdateFlag;

	};
	REGISTEREDITORCLASS(AnimationStateTransitionStartNode)


}
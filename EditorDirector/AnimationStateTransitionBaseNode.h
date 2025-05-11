#pragma once

#include"PanelUiEntity.h"
#include"EditorMacro.h"

#include"AnimationNodeType.h"

namespace Quad
{	

	
	


	class Curve;

	class AnimationStateTransitionBaseNode:public PanelUiEntity
	{
	public:
		AnimationStateTransitionBaseNode(EAnimationNodeType nodeType,  const std::string& name = "");
		virtual ~AnimationStateTransitionBaseNode() = 0;

		virtual void Initialize() override;


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		

		//node들에 연결된 spline의 curve들의 양끝컨트롤포인트을통해 중간컨트롤포인트들을 계산해준다.
		void SetCurveControlPoint(Curve* curve);

		EAnimationNodeType GetAnimationNodeType() const;




		void SetNodeID(unsigned long long id);
		const unsigned long long GetNodeID() const;

		virtual void Serialize() override;
		virtual void DeSerialize() override;
		

	protected:
		virtual void HandleMouseMove(Event* pEvent) override;
		virtual void HandleLButtonDown(Event* pEvent) override;

	private:
		unsigned long long mNodeID; 
		EAnimationNodeType mAnimationNodeType;


	};

}

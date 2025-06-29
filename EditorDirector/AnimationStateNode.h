#pragma once


#include"AnimationStateTransitionBaseNode.h"
#include"EditorMacro.h"



namespace Quad
{
	class AnimationStateLinkRectNode;
	class AnimationStateTransitionStartNode;
	class AnimationStateTransitionEndNode;


	class AnimationStateNode:public AnimationStateTransitionBaseNode
	{
	public:

		AnimationStateNode(const std::string& name = "");
		virtual ~AnimationStateNode();

		virtual void Initialize() override;

		static AnimationStateNode* Create(Map* map, int mapLayerIndex = 0);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

		//virtual void SetSize(float width, float height) override;

		void SetStateName(const std::string& name);
		const std::string& GetStateName() const;

	//	virtual void SetPosition(const DirectX::XMFLOAT3& posL) override;
	//	virtual void SetPosition(float x, float y, float z) override;


		bool RayCastingLinkEndRectNode(Ray& ray);
		void SetLinkEndRectNodeHighLight(const DirectX::XMFLOAT3 & colorIntensity);



		void LinkSpline(Spline* spline,bool flagStartEnd);		//false : start, true : end

		//노드가움직일때마다 linkNode에 연결된 spline들의 위치도 업데이트한다.
		void UpdateSpline();

		void SetUpdateLinkedSpline();



		virtual void Serialize() override;
		virtual void DeSerialize() override;

		CLASSNAME(AnimationStateNode)
		CLASSSIZE(AnimationStateNode)
		//자신과 자식에대한 draw flag 설정

				


	protected:
		void InitCreating();

	private:

		virtual void HandleLButtonDown(const MouseInputData & mouseInputData) override;
		virtual void HandleLButtonUp(const MouseInputData& mouseInputData) override;
		virtual void HandleMouseMove(const MouseInputData& mouseInputData)override;
		virtual void HandleHover(const MouseInputData& mouseInputData)override;


		std::vector< AnimationStateLinkRectNode*> mLinkNodeVector;// 0 : left - inputRectNode , 1 :right - outputRectNode
		

		AnimationStateTransitionStartNode* mLinkStartNode;
		AnimationStateTransitionEndNode* mLinkEndNode;


		std::string mStateName;
		TextBoxPanel* mStateNameTextBoxPanel;

	};
	REGISTEREDITORCLASS(AnimationStateNode)
}

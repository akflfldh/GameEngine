#pragma once



#include"AnimationStateTransitionBaseNode.h"
#include"EditorMacro.h"
#include"InputSystemType.h"
namespace Quad
{
	class AnimationStateTransitionStartNode;
	class AnimationStateTransitionEndNode;
	class AnimationStateNode;

	class AnimationStateTransitionBlendNode:public AnimationStateTransitionBaseNode
	{
	public:
		AnimationStateTransitionBlendNode(const std::string& name = "");
		virtual ~AnimationStateTransitionBlendNode();

		virtual void Initialize() override;

		static AnimationStateTransitionBlendNode* Create(Map* map, int mapLayerIndex = 0);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		virtual void SetSize(float width, float height) override;

		bool RayCastingLinkEndRectNode(Ray& ray);
		void SetLinkEndRectNodeHighLight(const DirectX::XMFLOAT3& colorIntensity);


		bool SetSourceAnimationStateNode(AnimationStateNode* animStateNode);
		bool SetDestAnimationStateNode(AnimationStateNode* animStateNode);

		AnimationStateNode* GetSourceAnimationStateNode() const;
		AnimationStateNode* GetDestAnimationStateNode() const;

		virtual void SetPosition(const DirectX::XMFLOAT3& posL) override;
		virtual void SetPosition(float x, float y, float z) override;


		void SetTransitionDuration(float t);
		float GetTransitionDuration()const;

		void SetInputDevice(int index);
		int GetInputDevice() const;

		void SetInputState(int index);
		int GetInputState() const;
		
		void SetInputKeyValue(int value);
		int GetInputKeyValue() const;




		void SetTransitionCondition(int inputDevice,unsigned char key , EInputState inputState);


		//linkEndNode로 연결
		void LinkSpline(Spline* spline, bool flagStartEnd);
		void SetUpdateLinkedSpline();

		virtual void Serialize() override;
		virtual void DeSerialize() override;






		CLASSNAME(AnimationStateTransitionBlendNode);
	protected:
		void InitCreating();
		void CreateAnimaitonBlend();


	private:
		AnimationStateTransitionStartNode* mLinkStartNode;
		AnimationStateTransitionEndNode* mLinkEndNode;




		AnimationStateNode* mSourceStateNode;	//끝나는 에니메이션 상태노드
		AnimationStateNode* mDestStateNode;		//시작하는 에니메이션 상태노드

		float mTransitionDuration;



		//condition 

		//없음/키보드/마우스(0,1,2)
		int mInputDevice;


		//키보드이면, key value, key input State 
		//keystate =key down, up, held
		 //keyvalue = ascll 

		//마우스이면

		int mInputState; 
		int mInputKeyValue; 





	};
	REGISTEREDITORCLASS(AnimationStateTransitionBlendNode)

}

#pragma once




#include"Predeclare.h"
#include<DirectXMath.h>
#include<vector>
#include<unordered_map>
#include"Component/ModelBaseComponent.h"

#include"CoreDllExport.h"
#include"Core/BoneAnimSRT.h"

namespace Quad
{
	struct CORE_API_LIB AnimationStateTransition
	{
		std::string mDestState;
		float mTransitionDuration = 0.0f;

		bool operator==(const AnimationStateTransition& stateTransition)
		{
			if (mDestState != stateTransition.mDestState)
				return false;


			return true;
		}


		void Serialize(const std::string & tag);
		void DeSerialize(const std::string & tag);

	};












	class CORE_API_LIB AnimationState
	{
	public:
		AnimationState(const std::string & name ="");
		~AnimationState() = default;


		void Start();
		void Update(float deltaTime, std::vector<BoneAnimSRT> & oBoneAnimSRTVector);

		void SetAnimationClip(AnimationClip* animationClip);

		void SetName(const std::string& name);
		const std::string& GetName() const;


		void SetDestAnimationComponent(AnimationComponent* component);
		AnimationComponent* GetDestAnimationComponent()const;

		AnimationClip* GetAnimationClip() const;

		
		void SetLocalTimeSecondUpdate(float localTime, std::vector<BoneAnimSRT>& oBoneAnimSRTVector);
		void SetLocalTimeFrameUpdate(int frameCount, std::vector<BoneAnimSRT>& oBoneAnimSRTVector);


		void SetLocalStartTime(float startTime);
		void SetLocalEndTime(float endTime);

		float GetLocalStartTime()const;
		float GetLocalEndTime()const;


		void Serialize(const std::string & tag);
		void DeSerialize(const std::string& tag);


		void RegisterStateTransition(const AnimationStateTransition& transition);
		void RegisterStateTransition( AnimationStateTransition&& transition);

		bool GetAnimStateTransition(const std::string destStateName, AnimationStateTransition& oAnimStateTransition);

		//얻은직후만 바로사용할것
		AnimationStateTransition* GetAnimationStateTransition(const std::string & destStateName);

	private:
		std::string mName;
		AnimationComponent* mDestAnimationComponent;
		
		AnimationClip* mAnimationClip;



		//float mLocalStartTime;


		float mLocalStartTime = 0.0f;		//변경할수있다.
		float mLocalCurrentTime;
		float mLocalEndTime;				//변경할수있다. 




		std::vector<AnimationStateTransition>  mStateTransitionVector;

			

	};


	class CORE_API_LIB AnimationComponent:public ModelBaseComponent
	{
	



	public:
		AnimationComponent(SceneElement * sceneElement);
		~AnimationComponent();


		AnimationComponent & operator=(const AnimationComponent& component);

		void Start();
		virtual void Update(float deltaTime) override;

	


		//void SetAnimationClip(AnimationClip* clip);
		void SetSkeleton(Skeleton* skeleton);

		const std::vector<DirectX::XMFLOAT4X4>& GetFinalTransformMatrixVector() const;
		const std::vector<DirectX::XMFLOAT4X4>& GetFinalInverseTransposeTransformMatrixVector() const;


		bool AddState(const std::string& stateName, AnimationClip* animationClip);
		bool RemoveState(const std::string& stateName);


		bool RegisterStateTransition(const std::string& sourceStateName, const std::string& destStateName);

		void RequestStateTransition(const std::string& destStateName);




		Skeleton* GetSkeleton()const;

		void SetCurrentAnimationState(const std::string& stateName);
		AnimationState* GetCurrentAnimationState()const;

		bool GetAnimationComponentState() const;



		const std::unordered_map<std::string, AnimationState*>& GetAnimationStateTable() const ;
		


		void SetLocalTimeSecondUpdate(float localTime);
		void SetLocalTimeFrameUpdate(int frameCount);
		void SetUpdateFlag(bool flag);
		bool GetUpdateFlag()const;

		AnimationState* GetAnimationState(const std::string& stateName) const;



		void Reset();

		virtual void Serialize(const std::string& tag) override;
		virtual void DeSerialize(const std::string& tag) override;



		CLASSSIZE(AnimationComponent)
	private:
		void CacluateFinalTransformMatrix(const std::vector<DirectX::XMFLOAT4X4>& boneAnimationMatrixVector);
		
		

		//보간된 뼈대 srt 벡터를 받아서 행렬로 계산 /게산된 행렬들은 CacluateFinalTransformMatrix로 넘어가 최종 모델공간에서의 행렬로 변환
		void CacluateBoneLocalTransformMatrix(std::vector<BoneAnimSRT>& boneSRTVector, std::vector<DirectX::XMFLOAT4X4>& oBoneAnimationMatrixVector);


		void CrossFading(const std::vector<BoneAnimSRT>& sourceBoneSRTVector, const std::vector<BoneAnimSRT>& destBoneSRTVector, std::vector<BoneAnimSRT>& oBoneSRTVector);
		void EndAnimStateTransition();


	private:
		std::unordered_map<std::string, AnimationState*> mAnimationStateUnMap;
		AnimationState* mCurrentAnimationState = nullptr;


		//AnimationClip* mAnimationClip;
		Skeleton* mSkeleton;

		std::vector<DirectX::XMFLOAT4X4> mFinalMatrixVector;
		std::vector<DirectX::XMFLOAT4X4> mFinalInverseTransposeMatrixVector;
		
		float mCurrentAnimationClipLocalTime = 0.0f;


		bool mUpdateFlag= true;






		//상태전이중이라면 true 그렇지않으면 false
		bool mStateTransitionFlag;
		AnimationState* mDestAnimationState;

		AnimationStateTransition* mCurrAnimStateTransition;
		float mCurrAnimStateTransitionTime;

	};


}


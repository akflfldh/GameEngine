#pragma once


#include"Core/CoreDllExport.h"

#include"InputSystemType.h"

#include"Component/AnimationComponent.h"
#include<vector>
#include"Predeclare.h"
namespace Quad
{
	class RegisterAnimStateTransitionCallbackClassManager;

	struct AnimationStateTransitionInputContext
	{
		int key= 0;
		EInputState mInputState = EInputState::eKeyDown;
		AnimationState* mSourceState;
		AnimationState* mDestState;
	};
	class AnimationComponent;

	class CORE_API_LIB RegisterAnimStateTransitionCallbackClass
	{
	public:
		RegisterAnimStateTransitionCallbackClass( );
		~RegisterAnimStateTransitionCallbackClass();

		void Start();
		void SetDestRegisterAnimStateTransitionCallbackClassMangaer(RegisterAnimStateTransitionCallbackClassManager* destManager);


		void RegisterAnimStateTransitionInputCallback(int key, EInputState InputState, AnimationState* sourceState,
		AnimationState* destState);

		void SetAnimaitonComponent(AnimationComponent* animComponent);
		AnimationComponent* GetAnimationComponent() const;

		
		void Callback(const InputContext& inpputContext);



		void Serialize();
		void DeSerialize();


	private:

		std::vector< AnimationStateTransitionInputContext> mStateTransitionInputContextVector;
		AnimationComponent* mAnimationComponent;


		RegisterAnimStateTransitionCallbackClassManager* mDestManager;



	};

}

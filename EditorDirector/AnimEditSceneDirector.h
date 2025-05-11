#pragma once

#include"Predeclare.h"

#include"Utility/Singleton.h"
#include"ISceneDirector.h"
#include<vector>
#include<unordered_map>


namespace Quad
{

	class AnimationStateTransitionGraphBasePanel;

	class AnimEditSceneDirector:public Singleton<AnimEditSceneDirector>, public ISceneDirector
	{
	public:
		AnimEditSceneDirector();
		~AnimEditSceneDirector();

		void Initialize(FrameWindowUiSystem* frameWindowUiSystem);


		bool ActiveScene(Object* object);
		void InActiveScene() override;


	private:
		void CreateAnimStateEditSecene();
		
		void CreateAnimNodePopupPanel();
		
		

	private:
		FrameWindowUiSystem* mFrameWindowUiSystem;

		PanelUiEntity* mMainPanel;

		ScrollListPanel* mStateScrollListPanel;
		AnimationStateTransitionGraphBasePanel* mStateTransitionGraphBasePanel;
		//선택된 오브젝트
		Object* mDestObject;

		int mPopupCreatingNodePanelNum;;


	};

}


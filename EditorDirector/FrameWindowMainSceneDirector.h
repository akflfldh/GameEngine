#pragma once

#include"Predeclare.h"
#include"Utility/Singleton.h"
#include"AnimEditSceneDirector.h"
#include"ProjectSelectSceneDirector.h"
#include<unordered_map>
#include<string>



namespace Quad
{
	class ISceneDirector;

	class FrameWindowMainSceneDirector:public Singleton<FrameWindowMainSceneDirector>
	{
	public:
		FrameWindowMainSceneDirector();
		~FrameWindowMainSceneDirector() = default;
			
		//메인씬들을 만든다.
		//메인씬은 ~~ 이 있다.
		//ProjectSelectScene;

		void Initialize(FrameWindowUiSystem* frameWindowUiSystem);

	
		//씬을 비활성화한다.
		void SetNullScene();

		void SetProjectSelectSecene();
		void SetAnimStateEditScene(Object* destObject);


	private:


	private:
		ISceneDirector* mCurrSceneDirector;



		FrameWindowUiSystem* mFrameWindowUiSystem;
		AnimEditSceneDirector mAnimStateEditDirector;
		ProjectSelectSceneDirector mProjectSelectSceneDirector;
	};

}

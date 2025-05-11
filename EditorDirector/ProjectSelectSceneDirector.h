#pragma once

#include"System/FrameWindowUiSystem.h"
#include"Utility/Singleton.h"
#include"ISceneDirector.h"

namespace Quad
{
	class ProjectSelectSceneDirector:public Singleton<ProjectSelectSceneDirector>,public ISceneDirector
	{
	public:
		ProjectSelectSceneDirector();
		~ProjectSelectSceneDirector();

		
		void Initialize(FrameWindowUiSystem* frameWindowUiSystem);

		void ActiveScene();
		virtual void InActiveScene() override;



	private:
		FrameWindowUiSystem* mFrameWindowUiSystem;

		PanelUiEntity* mMainScenePanel;
	};


}


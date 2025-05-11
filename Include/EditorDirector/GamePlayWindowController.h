#pragma once

#include<Shlwapi.h>

#pragma comment(lib,"Shlwapi.lib") 

#include"Predeclare.h"
#include"EditorDirector/DockingWindowController.h"

#include"Utility/Singleton.h"


namespace Quad
{
	//world edit이라고 생각하면되
	class GamePlayWindowController : public DockingWindowController,public Singleton<GamePlayWindowController>
	{
	public:
		GamePlayWindowController(RenderSystem* renderSystem);
		virtual ~GamePlayWindowController() = default;


	private:



	};

}
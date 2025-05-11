#pragma once

#include"Predeclare.h"
#include"Object/Camera/OrthogoanlCamera.h"
#include"Object/Macro.h"
#include"EditorMacro.h"
namespace Quad
{
	class ChromeSystemCamera :public OrthogoanlCamera
	{
	public:
		ChromeSystemCamera(const std::string& name="");
		~ChromeSystemCamera() = default;

		virtual void Initialize() override;

		static ChromeSystemCamera* Create(Map* map, int mapLayerIndex, UINT width, UINT height);


		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event* event) override;


		CLASSNAME(ChromeSystemCamera)
		CLASSSIZE(ChromeSystemCamera)
	protected:
		void InitCreating(UINT width, UINT height);



	private:
		void HandleWindowResizeEvent(Event* pEvent) ;


	};
	REGISTEREDITORCLASS(ChromeSystemCamera)
}


#pragma once



#include"Predeclare.h"
#include"Object/Camera/OrthogoanlCamera.h"
#include"Object/Macro.h"
#include"EditorMacro.h"

namespace Quad
{
	class DockingSystemCamera :public OrthogoanlCamera
	{
	public:
		
		DockingSystemCamera(const std::string& name="");
		~DockingSystemCamera() = default;

		virtual void Initialize() override;

		static DockingSystemCamera* Create(Map* map, int mapLayerIndex, UINT width, UINT height);


		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event * event) override;
		CLASSNAME(DockingSystemCamera)
		CLASSSIZE(DockingSystemCamera)

	protected:
		void InitCreating(UINT width, UINT height);

		virtual void HandleWindowResizeEvent(Event* pEvent) override;

	private:


	};
	REGISTEREDITORCLASS(DockingSystemCamera)
}

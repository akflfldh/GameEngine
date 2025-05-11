#pragma once

#include"Predeclare.h"
#include"Object/Camera/OrthogoanlCamera.h"
#include <rapidjson/document.h>
#include"EditorMacro.h"
namespace Quad
{
	class PopupSystemCamera:public OrthogoanlCamera
	{
	public:
		PopupSystemCamera(const std::string& name="");
		~PopupSystemCamera() = default;

		virtual void Initialize() override;

		static PopupSystemCamera* Create(Map* map, int mapLayerIndex, UINT width, UINT height);


		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event * event) override;
		//void UpdateCollider(float deltaTime);

		CLASSNAME(PopupSystemCamera)
		CLASSSIZE(PopupSystemCamera)

	protected:
		void InitCreating(UINT width, UINT height);


	private:
		void HandleWindowResizeEvent(Event* pEvent);


	};
	REGISTEREDITORCLASS(PopupSystemCamera)
}


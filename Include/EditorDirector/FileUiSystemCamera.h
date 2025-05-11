#pragma once

#include"Predeclare.h"
#include"Object/Camera/OrthogoanlCamera.h"
#include"EditorMacro.h"
namespace Quad
{

	class FileUiSystemCamera:public OrthogoanlCamera
	{
	public:
		FileUiSystemCamera(const std::string& name="");
		~FileUiSystemCamera() = default;

		virtual void Initialize();
		static FileUiSystemCamera* Create(Map* map, int mapLayerIndex,UINT width, UINT height);


		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event * event) override;

		CLASSNAME(FileUiSystemCamera)
		CLASSSIZE(FileUiSystemCamera)

	protected:
		void InitCreating(UINT width, UINT height);

	private:

		//void HandleWindowResizeEvent();

	private:

	};
	REGISTEREDITORCLASS(FileUiSystemCamera)
}


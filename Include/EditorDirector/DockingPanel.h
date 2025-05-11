#pragma once

#include"Predeclare.h"
#include"Object//UiEntity.h"
#include"EditorMacro.h"
namespace Quad
{
	class DockingPanel :public UiEntity
	{
	public:
		DockingPanel(const std::string& name="");
		virtual ~DockingPanel() = default;

		virtual void Initialize() override;

		static DockingPanel* Create(Map * map, int mapLayerIndex = 0);


		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event * event) override;
	

		CLASSNAME(DockingPanel)
		void SetSize(float width, float height);
		float GetWidth()const;
		float GetHeight()const;

		void SetPosition(float x, float y, float z);
	protected:

		void InitCreating();

	private:

		float mWidth;
		float mHeight;
	};
	REGISTEREDITORCLASS(DockingPanel)

}


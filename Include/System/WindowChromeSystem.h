#pragma once


#include"Predeclare.h"
#include"System.h"

namespace Quad
{
	class WindowChromeSystem:public System
	{
	public:
		WindowChromeSystem(ESystemID systemID);
		virtual ~WindowChromeSystem() = default;

		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map);
		virtual void Update(float deltaTime, bool playMode=true)override;
		virtual void OnResize(UINT clientWidth, UINT clientHeight);

		virtual void AddEntity(Object* entity, int mapLayer = 0, bool egineEditObject = false) override;
		//void GetEntity(std::vector<Object*>& oEntityVector) override;
		//virtual const std::vector<MapLayer>& GetEntity()override;
		void SetTitleBarSize(int width ,int height);
		int GetTitleBarHeight()const;


		TitleBarEntity* GetTitleBarEntity() const;

		

	private:

		TitleBarEntity* mTitleBar;
		int mTitleBarHeight;

		unsigned long long mTitleBarId;

	};
}


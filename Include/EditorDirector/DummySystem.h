#pragma once

#include"Predeclare.h"
#include"System.h"

//namespace Quad
//{
//
//	class DummySystem:public System
//	{
//	public:
//		DummySystem();
//		virtual ~DummySystem()=default ;
//
//		virtual void Initialize(UINT clientWidth, UINT clientHeight) ;
//		virtual void Update(float deltaTime) override;
//		virtual void OnResize(UINT clientWidth, UINT clientHeight) override;
//		virtual void GetRenderSettingItem(RenderSettingItem& renderSettingItem) const;
//
//
//
//		virtual void AddEntity(Object* entity) override;
//		virtual void GetEntity(std::vector<Object*>& oEntityVector)override;
//		virtual void HandleEvent(Event* event) override;
//
//	private:
//		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map) override;
//
//	};
//}
//

//#pragma once
//#include"UiSystem.h"
//#include"RenderSystem.h"
//
//
//#include"MathHelper.h"
//#include"Event.h"
//#include"MouseEvent.h"
//#include"SystemState.h"
//#include"Singleton.h"
//
//
//
//namespace Quad
//{
//
//
//	class GamePlayUiSystem :public UiSystem,public Singleton<GamePlayUiSystem>
//	{
//	public:
//		GamePlayUiSystem() ;
//		~GamePlayUiSystem() = default;
//
//		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)override;
//		virtual void Update(float deltaTime, bool playMode) override;
//		virtual void OnResize(UINT clientWidth, UINT clientHeight)override;
//
//
//		void AddEntity(Object* entity, int mapLayer = 0, bool egineEditObject = false) override;
////void GetEntity(std::vector<Object*>& oUiEntityVector) override;
//
//		//virtual const std::vector<MapLayer>& GetEntity()override;
//
//	private:
//		//void HandleLButtonDownEvent(MouseEvent* event);
//
//
//	private:
//		//SystemState* mState;
//
//
//	};
//}
//

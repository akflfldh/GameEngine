//#pragma once
//
//#include"System.h"
//#include"Utility/Singleton.h"
//namespace Quad
//{
//	class GameUiSystem:public System,public Singleton<GameUiSystem>
//	{
//	public:
//		GameUiSystem();
//		virtual ~GameUiSystem();
//
//		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map);
//		virtual void Update(float deltaTime, bool playMode = true);
//		virtual void EndUpdate(float deltaTime) override;
//		virtual void OnResize(UINT clientWidth, UINT clientHeight);
//
//
//		//이함수들은 제거될것이다.//map에서 직접 addEntity하니깐 
//		virtual void AddEntity(Object* entity, int mapLayer = 0, bool egineEditObject = false) override;
//
//
//	private:
//
//	};
//}
//

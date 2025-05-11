#pragma once


#include"System.h"
#include"Gizmo.h"
#include"Singleton.h"
namespace Quad
{
	class GamePlaySystem :public System,public Singleton<GamePlaySystem>
	{
	public:
		GamePlaySystem();
		~GamePlaySystem() = default;

		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map);
		virtual void Update(float deltaTime, bool playMode)override;
		virtual void OnResize(UINT clientWidth, UINT clientHeight);

		virtual void AddEntity(Object* entity, int mapLayer = 0, bool egineEditObject = false) override;
	//	virtual void GetEntity(std::vector<Object*>& oUiEntityVector) override;
		//virtual const std::vector<MapLayer>& GetEntity()override;



	private:
		


	};

}

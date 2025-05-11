#pragma once
#pragma once

#include"Predeclare.h"
#include"Core/System.h"


namespace Quad
{
	class FileUiSystem:public System
	{
	public:
		FileUiSystem();
		virtual ~FileUiSystem();

		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map);
		virtual void Update(float deltaTime,bool playMode);
		virtual void OnResize(UINT clientWidth, UINT clientHeight);

		virtual void AddEntity(Object* entity, int mapLayer = 0, bool egineEditObject = false);
		//virtual void GetEntity(std::vector<Object*>& oUiEntityVector);
	//	virtual const std::vector<MapLayer>& GetEntity()override;
		virtual void HandleEvent(Event* event);

	private:



	};

}


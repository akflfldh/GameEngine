#pragma once
#include"Event/Event.h"

//event GamePlayOn, GamePlayOff 
namespace Quad
{
	class GamePlayEvent :public Event
	{
	public:
		GamePlayEvent(const std::string& name);
		~GamePlayEvent() = default;


	private:



	};

}
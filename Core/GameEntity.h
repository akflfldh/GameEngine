#pragma once


#include"Object/Entity.h"
namespace Quad
{
	class GameEntity:public Entity
	{
	public:
		GameEntity();
		~GameEntity();

		//virtual void Initialize() override;

	protected:

		void InitCreating();


	private:

	};

}

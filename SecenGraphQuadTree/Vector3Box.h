#pragma once

#include"Predeclare.h"
#include"BoxEntity.h"
//#include"Text.h"


namespace Quad
{
	class Vector3Box:public BoxEntity
	{
	public:
		Vector3Box(const std::string & name);
		~Vector3Box() = default;
		virtual void Initialize() override;

		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event * event) override;

	private:
		//Text mTag[3];	//x,y,z
		//Text mText[3];	//x ,y,z
			


	};



}




#pragma once
#include"ObjectManager/BaseObjectManager.h"


namespace Quad
{
	class EditorRuntimeUserObjectManager:public BaseObjectManager
	{
	public:
		
		static EditorRuntimeUserObjectManager* GetInstance();

		virtual ~EditorRuntimeUserObjectManager();


	private:
		EditorRuntimeUserObjectManager();

	};

}

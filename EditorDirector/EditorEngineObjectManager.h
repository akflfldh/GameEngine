#pragma once


#include"ObjectManager/BaseObjectManager.h"


namespace Quad
{
	class EditorEngineObjectManager:public BaseObjectManager
	{
	public:
		static EditorEngineObjectManager* GetInstance();


	
		virtual ~EditorEngineObjectManager();

	private:
		EditorEngineObjectManager();
	};

}

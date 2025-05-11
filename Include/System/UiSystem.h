#pragma once




#include"System.h"
#include"CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB UiSystem :public System
	{
	public:
		UiSystem(ESystemID systemID,const std::string & name);
		virtual ~UiSystem() = 0;

		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map);
		virtual void Update(float deltaTime, bool playMode)override;
		virtual void OnResize(UINT clientWidth, UINT clientHeight);

		//virtual void AddUiEntity(Entity* entity) =0;
		//virtual void GetUiEntity(std::vector<Object*>& oUiEntityVector)=0 ;

	private:

	};
}




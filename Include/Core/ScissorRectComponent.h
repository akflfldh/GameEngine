#pragma once


#include<Windows.h>
#include"BaseComponent.h"
#include"CoreDllExport.h"

namespace Quad
{
	
	//UiEntity의 파생클래스들만 소유해야한다.
	class CORE_API_LIB ScissorRectComponent:public BaseComponent
	{
	public:
		ScissorRectComponent();
		virtual ~ScissorRectComponent();



		virtual void Start() override;
		virtual void Update(float delta) override;

		const RECT GetRect() const;

		void SetParentRectOverlapFlag(bool flag);



		CLASSNAME(ScissorRectComponent)
		CLASSNAMESTATIC(ScissorRectComponent)


	private:
		void SetRect(const RECT & rect);


	private:
		bool mParentRectOverlapFlag;		//이 컴포넌트의 소유 오브젝트의 부모 오브젝트의 rect에 영향을 받아, 자체설정한rect가 무효화되는지 여부 true이면 부모 rect와 동일한 값을 가진다. 

	//	RECT mParentRect;
		RECT mRect;

	};
	REGISTERCOMPONENTCLASS(ScissorRectComponent)

}

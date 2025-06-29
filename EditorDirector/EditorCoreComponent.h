#pragma once

#include<BaseComponent.h>
#include<Object/Macro.h>
namespace Quad
{
		//editor에서 object들에게 주는 에디터전용컴포넌트 , object에 들어가지않고, 별도의 시스템(ObjectEditorComponentTableSystem)
		// 이 컴포넌트를 관리유지한다.
	class EditorCoreComponent:public BaseComponent
	{
	public:
		EditorCoreComponent();
		virtual ~EditorCoreComponent();

		virtual void Initialize(Object* destObject);//생성될떄 호출되는 ,


		virtual void Start();//게임플레이 시작시한번호출
		virtual void Update(float delta);//게임플레이 매프레임 호출

		void SetEngineObjectFlag(bool flag);
		bool GetEngineObjectFlag() const;

		CLASSNAME(EditorCoreComponent)
		CLASSNAMESTATIC(EditorCoreComponent)

	private:
		bool mEngineObjectFlag;	//engine전용 오브젝트인지 userObject인지 구별
	};
	REGISTERCOMPONENTCLASS(EditorCoreComponent)
	
}

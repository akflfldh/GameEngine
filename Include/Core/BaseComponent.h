#pragma once

#include"Predeclare.h"
#include"Core/CoreDllExport.h"
#include"Object/Macro.h"


namespace Quad
{

	enum class EComponentType
	{
		EDefaultComponentType =0,
		ESceneComponentType
	
	};





	class Object;
	class CORE_API_LIB BaseComponent
	{
	public:
		BaseComponent(EComponentType componentType= EComponentType::EDefaultComponentType);
		virtual ~BaseComponent() = 0 ;

		virtual void Initialize(Object * destObject) ;//생성될떄 호출되는 ,


		
		virtual void Start() = 0;//게임플레이 시작시한번호출
		virtual void Update(float delta) = 0;//게임플레이 매프레임 호출
		
		void SetDestObject(Object * object); //유저는 메서드는 사용하지말것
		Object* GetDestObject() const;


		//map에 있는 object에 새롭게 이 component가 추가될떄 이 함수를 호출한다.
		//그리고 object가 맵에 처음 생성된후 이함수를 호출한다.
		//따라서 여러 시스템, 콜리전월드에 컴포넌트를 추가한다던가 콜라이더를 넣는다던가하는작업을 수행할수있다.
		virtual void OnActive();
		virtual void DeActive();


		EComponentType GetComponentType() const;



		virtual void SetActiveState(bool activeState);
		virtual bool GetActiveState() const;

		virtual void Serialize(const std::string& tag = "") ;
		virtual void DeSerialize(const std::string& tag = "") ;
		//컴포넌트들은 무조건 CLASSNAME 매크로를 사용할것
		CLASSNAME(BaseComponent)

	private:
		Object* mDestObject;

		bool mActiveState;

		EComponentType mEComponentType;

	};

}

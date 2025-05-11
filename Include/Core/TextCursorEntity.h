#pragma once


#include"Predeclare.h"
#include"Object/UiEntity.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB TextCursorEntity:public UiEntity
	{
	public:
		TextCursorEntity();
		virtual ~TextCursorEntity();

		virtual void Initialize() override;

		static TextCursorEntity* Create(Map* map, int mapLayerIndex = 0);

		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		CLASSNAME(TextCursorEntity)
		CLASSSIZE(TextCursorEntity)
		int GetPositionInText() const;
		void SetPositionInText(int position);

	protected:
		void InitCreating();


	private:
		int mPositionInText;//0번이다 = 다음글자를입력하면 0번위치에 글자가 삽입되고 기존에있던 0번 이후로의문자들은 뒤로밀린다.
		//삭제될시 3번이다 그럼 3-1 = 2번 글자가 삭제되고 3번이후에있던 문자들이 앞으로 이동한다. 

		


	};
	REGISTERCLASS(TextCursorEntity)

}

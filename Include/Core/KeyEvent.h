#pragma once

#include"Predeclare.h"
#include"Core/Event.h"
#include<string>
#include"Core/CoreDllExport.h"

namespace Quad
{
	//keyDown , keyUp
	class CORE_API_LIB KeyEvent:public Event
	{
	public:
		KeyEvent(const std::string & name);
		virtual ~KeyEvent()=default;
		
		void SetKey(int key);
		int GetKey()const;

		void SetCharacter(wchar_t character);
		wchar_t GetCharacter() const;

		void SetVKFlag(bool flag);
		bool GetVKFlag()const;
		//일반적으로 텍스트박스말고는 무조건 vk만 받을수있다.
		//텍스트박스의경우에는 문자와 vk가 값이 같은경우가있으니 구분해야된다.


		void SetHangulFlag(bool flag);
		bool GetHangulFlag() const;

		void SetHangulCompositionFlag(bool flag);
		bool GetHangulCompositionFlag()const;


	private:
		int mKey;//윈도우즈의 가상키에서 영어는 사실 아스키코드값하고같아서 문제가없을수도있고
		//유니코드도 아스키코드왕와의 호환성을 고려하면 또 문제가없을수도있나
		//결국 한글같은것은 어떻게할것인가.
		wchar_t mCharacterKey;
		
		bool mHangulFlag;//문자가 한글인가.
		bool mHangulCompositionFlag;	//현재 전달되는 한글문자가 조합중인지,아니면 완성된문자인지를 알려준다.
		

		bool mVKFlag;		//true이면 가상키이고 false 문자이다.


	};

}


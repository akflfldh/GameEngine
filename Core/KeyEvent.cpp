#include "Core/KeyEvent.h"

Quad::KeyEvent::KeyEvent(const std::string& name)
	:Event(name),mHangulFlag(false),mHangulCompositionFlag(false)
{
}

void Quad::KeyEvent::SetKey(int key)
{
	mKey = key;
	wchar_t tt = mKey;
	int a = 2;
}

int Quad::KeyEvent::GetKey() const
{

	return mKey;
}

void Quad::KeyEvent::SetCharacter(wchar_t character)
{
	mCharacterKey = character;
}

wchar_t Quad::KeyEvent::GetCharacter() const
{
	return mCharacterKey;
}

void Quad::KeyEvent::SetVKFlag(bool flag)
{
	mVKFlag = flag;
}

bool Quad::KeyEvent::GetVKFlag() const
{
	return mVKFlag;
}

void Quad::KeyEvent::SetHangulFlag(bool flag)
{
	mHangulFlag = flag;
}

bool Quad::KeyEvent::GetHangulFlag() const
{
	return mHangulFlag;
}

void Quad::KeyEvent::SetHangulCompositionFlag(bool flag)
{
	mHangulCompositionFlag = flag;
}

bool Quad::KeyEvent::GetHangulCompositionFlag() const
{
	return mHangulCompositionFlag;
}

#include "WindowHandleRef.h"

QuadPF::WindowHandleRef::WindowHandleRef(HANDLE handle)
	:mControlBlock(nullptr)
{
	if (handle == INVALID_HANDLE_VALUE)
		return;

	mControlBlock = std::make_shared<HandleControlBlock>(handle);
}

QuadPF::WindowHandleRef::~WindowHandleRef()
{

}

QuadPF::WindowHandleRef::WindowHandleRef(const QuadPF::WindowHandleRef& rhs)
	:mControlBlock(rhs.mControlBlock)
{
	

}

QuadPF::WindowHandleRef& QuadPF::WindowHandleRef::operator=(const QuadPF::WindowHandleRef& rhs)
{
	mControlBlock = rhs.mControlBlock;

	return *this;
}

QuadPF::WindowHandleRef::WindowHandleRef(QuadPF::WindowHandleRef&& rhs)
	:mControlBlock(std::move(rhs.mControlBlock))
{
	

}

QuadPF::WindowHandleRef& QuadPF::WindowHandleRef::operator=(QuadPF::WindowHandleRef&& rhs)
{
	mControlBlock = std::move(rhs.mControlBlock);
	return *this;
}

HANDLE QuadPF::WindowHandleRef::GetHandle() const
{
	if (mControlBlock != nullptr)
		return mControlBlock->mHandle;

	return INVALID_HANDLE_VALUE;
}



QuadPF::HandleControlBlock::~HandleControlBlock()
{
	if (mHandle && mHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(mHandle);
	}
}

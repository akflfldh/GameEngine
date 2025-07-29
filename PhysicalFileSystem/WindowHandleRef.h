#pragma once


#ifdef _WINDOWS

#include<Windows.h>
#include<memory>

namespace QuadPF
{

	struct HandleControlBlock {
	
		HANDLE mHandle;

		~HandleControlBlock();
		HandleControlBlock(HANDLE handle) :mHandle(handle){}
		HandleControlBlock(const HandleControlBlock&) = default;
		HandleControlBlock& operator=(const HandleControlBlock&) = default;
		HandleControlBlock(HandleControlBlock&&)= default;
		HandleControlBlock& operator=(HandleControlBlock&&)= default;
	};

	class WindowHandleRef
	{
	public:
		WindowHandleRef(HANDLE handle);
		~WindowHandleRef();


		WindowHandleRef(const WindowHandleRef&);
		WindowHandleRef& operator=(const WindowHandleRef&);

		WindowHandleRef(WindowHandleRef&&);
		WindowHandleRef& operator=(WindowHandleRef&&);
		
		HANDLE GetHandle() const;

	private:
		std::shared_ptr< HandleControlBlock> mControlBlock;


	};

}
#endif

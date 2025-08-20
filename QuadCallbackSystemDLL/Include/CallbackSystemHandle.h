#pragma once

#include<stdint.h>
// raii는 구현하지않는다.
// 핸들을 받는 사용자에게 100% 책임을 전가한다.
#include"CallbackDllMacro.h"
class QUAD_CALLBACK_SYSTEM_API CallbackSystemHandle
{
public:
	explicit CallbackSystemHandle(uint32_t id);
	~CallbackSystemHandle() = default;

	CallbackSystemHandle(const CallbackSystemHandle&) = default;
	CallbackSystemHandle& operator=(const CallbackSystemHandle&) = default;

	CallbackSystemHandle(CallbackSystemHandle&&) = default;
	CallbackSystemHandle& operator=(CallbackSystemHandle&&) = default;

	uint32_t GetHandleID() const;

private:
	uint32_t mID;
};

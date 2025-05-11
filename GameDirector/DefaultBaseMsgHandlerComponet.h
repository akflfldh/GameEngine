#pragma once

#include"BaseWindowMsgHandlerComponent.h"

class DefaultBaseMsgHandlerComponet:public Quad::BaseWindowMsgHandlerComponent
{
public:
	DefaultBaseMsgHandlerComponet()=default;
	~DefaultBaseMsgHandlerComponet()=default;

	virtual LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& ret) override;

private:


};


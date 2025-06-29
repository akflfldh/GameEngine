#pragma once

#include<Windows.h>
#include<Object/Macro.h>
namespace Quad
{
class CORE_API_LIB GlobalAppHelper
{
public:
	static GlobalAppHelper* GetInstance();

	void Initialize(HINSTANCE hinstance);

	static HINSTANCE GetHinstance();

private:

	static HINSTANCE mHinstance;


};

}

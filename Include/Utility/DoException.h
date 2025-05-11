#pragma once
#include<windows.h>
#include<string>
#include<comdef.h>




#ifdef UTILITY_LIB_EXPORT
#define UTILITY_LIB_API __declspec(dllexport)
#else
#define UTILITY_LIB_API __declspec(dllimport)
#endif







namespace Quad
{
	class UTILITY_LIB_API  DoException
	{
	public:
		DoException() = default;
		DoException(HRESULT hr, const std::wstring& functionName,
			const std::wstring& filename, int lineNumber);
		~DoException() = default;

		std::wstring toString() const;

	private:
		HRESULT mErrorCode = S_OK;
		std::wstring mFunctionName;
		std::wstring mFileName;
		int mLineNumber = -1;

	};

}

//디버그를 위한 매크로와 문자처리함수는 나중에 알아볼것 지금은예제를 복사했다.

extern inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}



#ifndef ThrowIfFailed
#define ThrowIfFailed(x)															\
{																					\
	HRESULT	hr__ =(x);																\
	std::wstring wfn =AnsiToWString(__FILE__);										\
	if(FAILED(hr__)){ throw DoException(hr__,L#x , wfn, __LINE__);}				\
}
#endif 



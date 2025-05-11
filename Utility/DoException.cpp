#include "Utility/DoException.h"
namespace Quad
{
	DoException::DoException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
		mErrorCode(hr), mFunctionName(functionName), mFileName(filename), mLineNumber(lineNumber)
	{
	}

	std::wstring DoException::toString() const
	{

		_com_error err(mErrorCode);		//이부분은 그냥 예제를 일단 따라한다.
		std::wstring msg = err.ErrorMessage();
		return mFunctionName + L"failed in  " + mFileName + L" ; Line " + std::to_wstring(mLineNumber) + L"error :" + msg;

	}
}
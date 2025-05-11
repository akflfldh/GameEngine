#pragma once

#include<Windows.h>
#include<tchar.h>


#ifdef UTILITY_LIB_EXPORT
#define UTILITY_LIB_API __declspec(dllexport)
#else
#define UTILITY_LIB_API __declspec(dllimport)
#endif


namespace Quad
{
	class UTILITY_LIB_API ReaderWriter
	{
	public:
		ReaderWriter();
		~ReaderWriter();

		void Initialize();


		void ReaderStart();
		void ReaderEnd();

		void WriterStart();
		void WriterEnd();
	private:
		HANDLE mCommonMutex;
		HANDLE mReaderCountMutex;
		HANDLE mWriterStartEvent;


		size_t mReaderCount;




	};

}
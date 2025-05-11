#include "Utility/ReaderWriter.h"
namespace Quad
{
	ReaderWriter::ReaderWriter()
		:mCommonMutex(nullptr), mReaderCountMutex(nullptr), mWriterStartEvent(nullptr), mReaderCount(0)
	{


	}

	ReaderWriter::~ReaderWriter()
	{
		if (mCommonMutex != nullptr)
			CloseHandle(mCommonMutex);

		if (mReaderCountMutex != nullptr)
			CloseHandle(mReaderCountMutex);

		if (mWriterStartEvent != nullptr)
			CloseHandle(mWriterStartEvent);

	}

	void ReaderWriter::Initialize()
	{
		mCommonMutex = CreateMutex(nullptr, false, NULL);
		if (mCommonMutex == nullptr)
		{
			throw  GetLastError();
		}

		mReaderCountMutex = CreateMutex(nullptr, false, NULL);
		if (mReaderCountMutex == nullptr)
		{
			throw GetLastError();
		}

		mWriterStartEvent = CreateEvent(nullptr, true, true, nullptr);
		if (mWriterStartEvent == nullptr)
		{
			throw GetLastError();
		}

	}

	void ReaderWriter::ReaderStart()
	{

		WaitForSingleObject(mCommonMutex, INFINITE);


		WaitForSingleObject(mReaderCountMutex, INFINITE);		//readercount는 크리티컬 섹션을 사용해도 좋을거같다.

		mReaderCount++;
		if (mReaderCount == 1)
		{
			ResetEvent(mWriterStartEvent);
		}

		ReleaseMutex(mReaderCountMutex);
		ReleaseMutex(mCommonMutex);

		//읽기수행

	}

	void ReaderWriter::ReaderEnd()
	{

		WaitForSingleObject(mReaderCountMutex, INFINITE);
		mReaderCount--;

		if (mReaderCount == 0)
		{
			SetEvent(mWriterStartEvent);
		}

		ReleaseMutex(mReaderCountMutex);

	}

	void ReaderWriter::WriterStart()
	{

		WaitForSingleObject(mCommonMutex, INFINITE);

		WaitForSingleObject(mWriterStartEvent, INFINITE);

	}

	void ReaderWriter::WriterEnd()
	{

		ReleaseMutex(mCommonMutex);

	}

}
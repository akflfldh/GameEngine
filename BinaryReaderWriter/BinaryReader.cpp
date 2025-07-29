#include "BinaryReader.h"
#include<PhysicalFileSystem.h>
#include<assert.h>
QuadRW::BinaryReader::BinaryReader()
	:mPhysicalFileSystem(QuadPF::PhysicalFileSystem::GetInstance())
{

}

QuadRW::BinaryReader::~BinaryReader()
{
}

bool QuadRW::BinaryReader::StartRead(const std::string& filePath)
{
	FlushBuffer();
	mReadPointer = 0;
	return mPhysicalFileSystem->ReadFileToBuffer(filePath, mBuffer);
}


template<typename T>
inline bool QuadRW::BinaryReader::Read(T& oData)
{

	if constexpr (std::is_fundamental_v<T>)
	{

		if (mBuffer.size() < mReadPointer + sizeof(oData))
		{
			//log
			assert(0);
		}

		memcpy((void*)(&oData), (const void*)&mBuffer[mReadPointer], sizeof(oData));
		mReadPointer += sizeof(oData);
		return true;

	}
	return false;
}


bool  QuadRW::BinaryReader::Read(std::string& oStr)
{


	size_t size;
	bool ret =Read(size);
	if (ret == false)
		return false;


	if (mBuffer.size() < mReadPointer + size)
		return false;



	oStr.resize(size);

	if (size != 0)
	{
		memcpy((void*)oStr.data(), (const void*)&mBuffer[mReadPointer], size);

		mReadPointer += size;
	}
	return true;
}

bool QuadRW::BinaryReader::Read(void** oData, size_t& oSize)
{

	bool ret =Read(oSize);
	if (ret == false)
		return false;


	if (mBuffer.size() < mReadPointer + oSize)
		return false;

	*oData = (void *)&mBuffer[mReadPointer];
	mReadPointer += oSize;
	return true;
}

bool QuadRW::BinaryReader::Read(void* oBuffer, size_t size)
{

	if (mBuffer.size() < mReadPointer + size)
		return false;


	memcpy(oBuffer, (const void*)&mBuffer[mReadPointer], size);
	mReadPointer += size;
	return true;
}

bool QuadRW::BinaryReader::SetReadPointer(uint64_t pointerPos)
{
	if (mBuffer.size() <= pointerPos)
		return false;


	mReadPointer = pointerPos;
	return true;
}

uint64_t QuadRW::BinaryReader::GetReadPointer() const
{
	return mReadPointer;
}

void QuadRW::BinaryReader::FlushBuffer()
{

	mBuffer.clear();

}

uint64_t QuadRW::BinaryReader::GetFileSize() const
{
	return mBuffer.size();


}


template bool QuadRW::BinaryReader::Read<bool>(bool & data);
template bool QuadRW::BinaryReader::Read<char>(char& data);
template bool QuadRW::BinaryReader::Read<unsigned char>(unsigned char & data);
template bool QuadRW::BinaryReader::Read<short>(short & data);
template bool QuadRW::BinaryReader::Read<unsigned short>(unsigned short & data);
template bool QuadRW::BinaryReader::Read<int>(int & data);
template bool QuadRW::BinaryReader::Read<unsigned int>(unsigned int  &data);
template bool QuadRW::BinaryReader::Read<long>(long  &data);
template bool QuadRW::BinaryReader::Read<unsigned long>(unsigned long  &data);
template bool QuadRW::BinaryReader::Read<long long>(long long & data);
template bool QuadRW::BinaryReader::Read<unsigned long long>(unsigned long long & data);
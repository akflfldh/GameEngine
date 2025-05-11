#include "LoadRequestFileEvent.h"

Quad::LoadRequestFileEvent::LoadRequestFileEvent()
	:Event("LoadRequestFile")
{
}

Quad::LoadRequestFileEvent::~LoadRequestFileEvent()
{
}

void Quad::LoadRequestFileEvent::SetFileVector(const std::vector<std::string>& fileVector)
{
	mFileVector = fileVector;
}

void Quad::LoadRequestFileEvent::SetFileVector(std::vector<std::string>&& fileVector)
{
	mFileVector = std::move(fileVector);
}

const std::vector<std::string>& Quad::LoadRequestFileEvent::GetFileVector() const
{
	return mFileVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

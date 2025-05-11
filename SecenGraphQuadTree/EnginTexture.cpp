#include "EnginTexture.h"

Quad::EnginTexture::EnginTexture(const std::string& name)
	:Texture(name)
{


}

void Quad::EnginTexture::SetFilePath(const std::string& filePath)
{
	mFilePath = filePath;
}

const std::string& Quad::EnginTexture::GetFilePath() const
{
	return mFilePath;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::EnginTexture::Serialize(const std::string& tag )
{
	Texture::Serialize(tag);
}

void Quad::EnginTexture::DeSerialize(const std::string& tag)
{
	Texture::DeSerialize(tag);
}

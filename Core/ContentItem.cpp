#include "Core/ContentItem.h"
#include"Parser/JsonParser.h"

Quad::ContentItem::ContentItem(const std::string& name, EContentItemType contentItemType)
	:mName(name), mContentItemType(contentItemType), mIsEngineContentItem(false)
{
}



Quad::ContentItem::~ContentItem()
{

}


void Quad::ContentItem::SetName(const std::string& name)
{
	mName = name;
}

void Quad::ContentItem::SetName(std::string&& name)
{
	mName = std::move(name);
}

const std::string& Quad::ContentItem::GetName() const
{

	return mName;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::EContentItemType Quad::ContentItem::GetEContentItemType() const
{
	return mContentItemType;
}

void Quad::ContentItem::Serialize(const std::string& tag )
{
	 
	JsonParser::Write("ContentItem_Name", mName);

}

void Quad::ContentItem::DeSerialize(const std::string& tag )
{
	JsonParser::Read("ContentItem_Name", mName);

}

void Quad::ContentItem::SetEngineContentItemFlag(bool flag)
{
	mIsEngineContentItem = flag;
}

bool Quad::ContentItem::GetEngineContentItemFlag() const
{
	return mIsEngineContentItem;
}

bool Quad::ContentItem::Copy(ContentItem* source)
{

	//전처리
	if (source == nullptr || source == this || !CheckType(source))
		return false;

	bool ret =InnerCopy(source);


	//후처리

	return ret;

}

bool Quad::ContentItem::CheckType(const ContentItem * source )
{

	return true;
}

bool Quad::ContentItem::InnerCopy(const ContentItem* source)
{
	mContentItemType = source->mContentItemType;
	mIsEngineContentItem = source->mIsEngineContentItem;

	return true;
}

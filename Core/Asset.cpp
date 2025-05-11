#include "Asset/Asset.h"
#include"Parser/JsonParser.h"


Quad::Asset::Asset(const std::string& name ,EAssetType assetType)
	:ContentItem(name,EContentItemType::eAsset),mAssetType(assetType), mUniqueID(0),mDestLogicalFolderUniqueID(0)
{
}



Quad::Asset::~Asset()
{

}


Quad::EAssetType Quad::Asset::GetAssetType() const
{
	return mAssetType;
}

unsigned long long Quad::Asset::GetUniqueID() const
{
	return mUniqueID;
}

unsigned long long Quad::Asset::GetDestLogicalFolderUnqiueID() const
{
	return mDestLogicalFolderUniqueID;
}

void Quad::Asset::SetDestLogicalFolderUnqiueID(unsigned long long id)
{
	mDestLogicalFolderUniqueID = id;
}

void Quad::Asset::Serialize(const std::string& tag)
{
	ContentItem::Serialize();
	JsonParser::Write("Asset_ID", mUniqueID);

}

void Quad::Asset::DeSerialize(const std::string& tag)
{
	ContentItem::DeSerialize();
}

bool Quad::Asset::CheckType(const ContentItem* source)
{
	bool ret =	ContentItem::CheckType(source);
	if (!ret)
		return false;

	if (source->GetEContentItemType() != EContentItemType::eAsset)
		return false;


	return true;
}

bool Quad::Asset::InnerCopy(const ContentItem* source)
{
	
	ContentItem::InnerCopy(source);

	const Asset* sourceAsset = static_cast<const Asset*>(source);

	mAssetType = sourceAsset->mAssetType;
	mDestLogicalFolderUniqueID = sourceAsset->mDestLogicalFolderUniqueID;

	return true;
}

void Quad::Asset::SetUniqueID(unsigned long long id)
{
	mUniqueID = id;
}

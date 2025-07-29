#include "LogicalFile.h"


QuadLF::LogicalFile::LogicalFile(LogicalNodeID id)
	:LogicalNode(id, ELogicalNodeType::eFile)
{
}

QuadLF::LogicalFile::~LogicalFile()
{
}

void QuadLF::LogicalFile::SetAssetInfo(const LogicalFileAssetInfo& fileAssetInfo)
{
	mAssetInfo = fileAssetInfo;

}

QuadLF::LogicalFileAssetInfo& QuadLF::LogicalFile::GetAssetInfo()
{



	return mAssetInfo;
}

const QuadLF::LogicalFileAssetInfo& QuadLF::LogicalFile::GetAssetInfo() const
{
	
	return mAssetInfo;

}

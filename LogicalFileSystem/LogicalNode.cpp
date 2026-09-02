#include "LogicalFileSystem/LogicalNode.h"

QuadLF::LogicalNode::LogicalNode(LogicalNodeID id, ELogicalNodeType nodeType) : mID(id), mNodeType(nodeType) {}

QuadLF::LogicalNode::~LogicalNode() {}

const std::string &QuadLF::LogicalNode::GetName() const
{
    return mName;
}

QuadLF::LogicalNodeID QuadLF::LogicalNode::GetNodeID() const
{
    return mID;
}

QuadLF::ELogicalNodeType QuadLF::LogicalNode::GetNodeType() const
{
    return mNodeType;
}

std::string QuadLF::LogicalNode::GetFullPath() const
{
    //// 부모가 root의경우 (Root의 부모는 nullptr)
    // if (mParent == nullptr)
    //     return "";

    // if (mParent->GetParent() == nullptr)
    //     return mParent->GetName() + mName;

    // return mParent->GetFullPath() + "/" + mName;

    if (mParent == nullptr)
        return mName;

    std::string parentPath = mParent->GetFullPath();

    if (parentPath == "")
    {
        return mName;
    }
    else
    {
        return parentPath + "/" + mName;
    }
}

QuadLF::LogicalNode *QuadLF::LogicalNode::GetParent() const
{

    return mParent;
}

void QuadLF::LogicalNode::SetParent(LogicalNode *parent)
{

    mParent = parent;

    // 파일시스템메서드호출
}

bool QuadLF::LogicalNode::GetPhyscicalBindingFlag() const
{
    return mIsPhysicalNodeBinding;
}

void QuadLF::LogicalNode::SetName(const std::string &name)
{
    mName = name;
}

void QuadLF::LogicalNode::SetPhysicalBindingFlag(bool flag)
{
    mIsPhysicalNodeBinding = flag;
}

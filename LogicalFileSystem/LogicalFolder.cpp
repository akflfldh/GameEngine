#include "LogicalFolder.h"
#include"LogicalFileSystem.h"


QuadLF::LogicalFolder::LogicalFolder(LogicalNodeID id)
    :LogicalNode(id,ELogicalNodeType::eFolder)
{
}

QuadLF::LogicalFolder::~LogicalFolder()
{
}

bool QuadLF::LogicalFolder::HasChildNamed(const std::string& name) const
{

    std::unordered_map<std::string,LogicalNode*>::const_iterator  it =  mChildNodeTable.find(name);

    return it == mChildNodeTable.cend() ? false : true;
}

QuadLF::LogicalNode* QuadLF::LogicalFolder::GetChild(const std::string& name) const
{
    std::unordered_map<std::string, LogicalNode*>::const_iterator  it = mChildNodeTable.find(name);


    return it == mChildNodeTable.cend() ? nullptr : it->second;


}

QuadLF::LogicalFolder* QuadLF::LogicalFolder::CreateChildFolder(const std::string& name)
{
   return  LogicalFileSystem::GetInstance()->CreateFolder(name, this);
}

void QuadLF::LogicalFolder::AddChildNode(LogicalNode* node)
{
    mChildNodeVector.push_back(node);
    mChildNodeTable[node->GetName()] = node;
}

const std::vector<QuadLF::LogicalNode*>& QuadLF::LogicalFolder::GetChildNodeVector() const
{

    return mChildNodeVector;
    // TODO: 여기에 return 문을 삽입합니다.
}

void QuadLF::LogicalFolder::removeChildNode(LogicalNode* node)
{

    const std::string& name = node->GetName();

   std::unordered_map<std::string,LogicalNode*>::iterator tableIt = mChildNodeTable.find(name);
    
    if (tableIt != mChildNodeTable.end())
    {
        mChildNodeTable.erase(tableIt);
    }

    std::vector<LogicalNode*>::iterator vectorIt = std::find(mChildNodeVector.begin(), mChildNodeVector.end(), node);
    if (vectorIt != mChildNodeVector.end())
    {

        mChildNodeVector.erase(vectorIt);

    }





    return;
}

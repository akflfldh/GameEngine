#pragma once

#include "LogicalFileSystem/LogicalNode.h"
#include <string>
#include <unordered_map>
#include <vector>
namespace QuadLF
{

class LogicalFolder : public LogicalNode
{
    friend class LogicalFileSystem;

  public:
    virtual ~LogicalFolder();

    bool HasChildNamed(const std::string &name) const;
    LogicalNode *GetChild(const std::string &name) const;

    LogicalFolder *CreateChildFolder(const std::string &name);

    const std::vector<LogicalNode *> &GetChildNodeList() const;

  private:
    LogicalFolder(LogicalNodeID id);

    // fileSystem만이 호출할것이다.
    // 그렇게 하나의 컨트롤타워에서만 사용함으로써 순회문제등을 방지한다.
    void AddChildNode(LogicalNode *node);
    const std::vector<LogicalNode *> &GetChildNodeVector() const;

    // 내부구조에서 지운다.
    void removeChildNode(LogicalNode *node);

  private:
    // vector를 사용할것이냐, unordered_map을사용할것이냐
    // 일단 노드의 name이 유일성은 보장되니 key로 사용할수있고
    // 자식의 수에 상관없이 항상 일정한 상수시간의 탐색시간을 가진다는장점
    // 다만 name이바뀌면 테이블의 요소도 제거후 다시 생성해줘야한다는점-> 항상 filesystem을통해서
    // 변경할수있게함으로써(안정성을 높이자)
    std::vector<LogicalNode *> mChildNodeVector;
    std::unordered_map<std::string, LogicalNode *> mChildNodeTable;
};

} // namespace QuadLF

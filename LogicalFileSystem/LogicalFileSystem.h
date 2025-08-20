#pragma once

#include "LogicalFileSystem/LogicalCommon.h"
#include <string>
#include <vector>

namespace QuadPF
{
class PhysicalFileSystem;
}
namespace QuadRW
{
class BinaryWriter;
class BinaryReader;
} // namespace QuadRW

namespace QuadLF
{
class LogicalFolder;
class LogicalFile;
class LogicalNode;

// 물리적파일,폴더과 논리적파일과폴더와 연결되서 생성되었다면
// 물리적파일,폴더를 제거할떄도 논리적파일시스템을 통해야 올바른 추적과 동작이가능하다.

// LogicalDirectoryStructure(LDS)에서 folder표현
// LDS를저장할때 이구조체를바탕으로 저장한다.
struct LDSFolder
{
    std::string path;
    bool pyhsicalBindingFlag;
};

class LogicalFileSystem
{

  public:
    static LogicalFileSystem *GetInstance();
    LogicalFileSystem(QuadPF::PhysicalFileSystem *physicalFileSystem);
    ~LogicalFileSystem();

    // physcialRootPath  최상위 루프폴더가 위치한물리주소(논리적파일시스템이 물리적파일시스템을 사용하지않는다면
    // 빈문자열을 전달하면된다) ex) C:/aaa/bbb/ccc	맨 뒤 '/' 룰 붙이지 않는다
    void Initialize(const std::string &physicalRootPath);

    // logical file name(실제 물리적파일과 이름이 같을거같다),
    // logical path = 논리적경로(실제 물리적경로와 일치할수도있고 의도적으로 다르게할수도있다),
    // physicalFileFlag,physicalFolderFlag =실제 물리적파일을 생성할지여부
    LogicalFile *MakeFile(const LogicalFileAssetInfo &logicalAssetFileInfo, const std::string &fileName,
                          const std::string &path, bool physicalFileFlag = false);
    LogicalFile *MakeFile(const LogicalFileAssetInfo &logicalAssetFileInfo, const std::string &fileName,
                          LogicalFolder *parentFolder, bool physicalFileFlag = false);
    QuadLF::LogicalFolder *CreateFolder(const std::string &folderName, LogicalFolder *parentFolder,
                                        bool physicalFolderFlag = false);

    // 제거한다(완전히)
    bool RemoveFolder(LogicalFolder *folder);
    bool RemoveFile(LogicalFile *file);

    // folder.file이건 path를통해 유일한 node를 얻는다.
    LogicalNode *GetNode(const std::string &path) const;
    // 경로로부터 폴더를 얻는다.
    LogicalFolder *GetFolder(const std::string &path) const;
    LogicalFolder *GetRootFolder() const;

    // parentFolder밑에 존재하는 모든 file들에대해서 탐색한다 그러면
    // 디스플레이 이름이 겹칠수있다. 상관없음
    // parentFolder의 디폴트는 nullptr - > RootFolder이다.
    // 필터는 AssetType 열거형의 OR조합
    std::vector<LogicalFile *> GetFile(const std::string &fileName, uint32_t assetTypeFilter,
                                       LogicalFolder *parentFolder = nullptr);

    // 물리적RootPath를 반영한 물리주소를 리턴한다.
    // 외부시스템들은 이것을 받아서 실제로 파일에 접근해서 read,write를수행한다.
    std::string GetPhysicalFullPath(LogicalNode *node) const;

    // LDS function

    // path /fileName.lds; 로저장한다.
    bool SaveLogicalDirectoryStructureAsBinaryWriter(QuadRW::BinaryWriter &binaryWriter, const std::string &path,
                                                     const std::string &fileName);
    //	void SaveLogicalDirectoryStructureAsJsonWriter();

    // 로드후 자동으로 구축된다
    // 기존의 구조는 없어진다. or 애초에 맨처음 한번호출하도록하는 제약을 두자.
    bool LoadLogicalDirectoryStructureAsBinaryReader(QuadRW::BinaryReader &binaryReader, const std::string &filePath);

    // 단순히 flag만설정함으로 true로 변경한다해서 물리적 파일이나,폴더를 생성하거나 , 제거하는 기능은없다.
    //   이미 물리적파일,폴더가 존재해서 연결하는것또는 의도적으로 연결을끊기위해서 설정하는것이 주 용도
    void SetPhysicalBindingFlag(LogicalNode *node, bool flag);

    LogicalFolder *GetCurrentLogicalFolder() const;
    void SetCurrentLogicalFolder(LogicalFolder *folder);

  private:
    static LogicalFileSystem *mInstance;

    LogicalNodeID GetNextNodeID();
    void ReleaseNodeID(LogicalNodeID id);

    bool CreatePhysicalFolder(LogicalFolder *parentFolde, const std::string &folderName);
    LogicalFolder *CreateLogicalFolder(LogicalFolder *parentFolder, const std::string &folderName,
                                       bool physicalFolderBindingFlag);

    LogicalFolder *CreateFolderInstance(LogicalNodeID id);
    LogicalFile *CreateFileInstance(LogicalNodeID id);

    // 제거전 데이터정리(필요하다면) (확정x)
    void CleanFileDataUp(LogicalFile *file);

    bool ReleaseFolderInstance(LogicalFolder *folder);
    bool ReleaseFileInstance(LogicalFile *file);

    void InitSetting(LogicalNode *node, LogicalFolder *parentFolder, const std::string &name);

    // LDS function

    // root folder는 포함되지않는다.
    std::vector<LDSFolder> GetLDSFolderAll() const;

  private:
    QuadPF::PhysicalFileSystem *mPhysicalFileSystem;
    LogicalFolder *mRootFolder;

    LogicalNodeID mNextNodeID;
    std::vector<LogicalNodeID> mFreeNodeIDVector;

    std::string mPhysicalRootPath;

    LogicalFolder *mCurrentFolder;
};

} // namespace QuadLF
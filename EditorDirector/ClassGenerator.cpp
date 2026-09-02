#include "ClassGenerator.h"
#include <Core/ProjectConfig.h>
#include <CoreBase/AsyncThreadPool.h>
#include <EditorDirector/ProjectGenerator.h>
ClassGenerator *ClassGenerator::GetInstance()
{
    static ClassGenerator instance;
    return &instance;
}

ClassGenerator::ClassGenerator() {}

ClassGenerator::~ClassGenerator() {}

ClassGenerationTaskHandle ClassGenerator::RequestClassGenerate(const std::string &className,
                                                               const std::string &parentClassName, bool isCoreClass)
{

    auto threadPool = AsyncThreadPool::GetInstance();

    std::shared_ptr<ClassGenerationNotifyChannel> notifyChannel = std::make_shared<ClassGenerationNotifyChannel>();

    ClassGenerationTaskHandle taskHandle;
    taskHandle.mNotifyChannel = notifyChannel;

    threadPool->Submit([this, className, parentClassName, isCoreClass, notifyChannel]()
                       { GenerateClass(className, parentClassName, isCoreClass, notifyChannel); });

    return taskHandle;
}

void ClassGenerator::GenerateClass(const std::string &className, const std::string &parentClassName, bool isCoreClass,
                                   ClassGenerationJobContext jobContext)
{

    auto projectConfig = Quad::ProjectConfig::GetInstance();
    // projectConfig->GetProjectPath();

    std::filesystem::path targetPath = projectConfig->GetProjectPath() / "Source";

    bool bRet = false;
    //   PrograssContext prograssContext(mWorkingTaskContext);

    jobContext.ReportProgress(0.1f, "파일 생성중");
    bRet = ProjectGenerator::GenerateObjectCXXFile(parentClassName, className, targetPath, isCoreClass);
    if (!bRet)
    {
        // 이미 생성된것들을 없애는 작업 이 필요하다.

        jobContext.ReportFailed("파일 생성 실패");

        return;
    }

    jobContext.ReportProgress(0.6f, "파일 빌드 진행중");
    bRet = ProjectGenerator::ReBuildCMake(projectConfig->GetProjectPath(), projectConfig->GetProjectName());

    if (!bRet)
    {
        //
        jobContext.ReportFailed("파일 빌드 실패");
        return;
    }

    jobContext.ReportCompleted("클래스 파일 생성 완료");
}

#include "EditorBuildManager.h"
#include "PhysicalFileSystem/PhysicalFileSystem.h"
#include <Core/CollisionChannelSystem.h>
#include <Core/CoreType.h>
#include <Core/GameBuildManifest.h>
#include <Core/Map.h>
#include <Core/ProjectConfig.h>
#include <Core/World.h>
#include <CoreAsset/AssetCommon.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/PakWriter.h>
#include <CoreAsset/Texture.h>
#include <CoreBase/AsyncThreadPool.h>
#include <CoreBase/BinaryArch.h>
#include <EditorDirector/CMakeProjectBuilder.h>
#include <EditorDirector/EditorConfig.h>
#include <EditorDirector/EditorProjectManager.h>
#include <EditorDirector/EditorSceneManager.h>
#include <EditorDirector/Shader/ShaderCompiler.h>
#include <EditorDirector/TaskUIController.h>
#include <Logger/Logger.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <LogicalFileSystem/LogicalFolder.h>
#include <RenderFrontend/ShaderSourceLibrary.h>
#include <stack>
#include <utility>

Quad::EditorBuildManager *Quad::EditorBuildManager::GetInstance()
{

    static EditorBuildManager instance;
    return &instance;
}

Quad::EditorBuildManager::EditorBuildManager() {}

Quad::EditorBuildManager::~EditorBuildManager() {}

void Quad::EditorBuildManager::Initialize(UI::UICanvas *canvas)
{

    mUIController = ProjectBuildTaskUIController::GetInstance();
    mUIController->Initialize(canvas);

    mUIController->mOnClickedExitButtonCallbackSystem.Register([this]() { OnClickedExitButton(); });
}

void Quad::EditorBuildManager::Update()
{

    while (mTaskHandle.TryPopEvent(mCurrentEvent))
    {
        // UpdateUI(mCurrentEvent);
        mUIController->SetMessage(mCurrentEvent.mMessage);

        if (mCurrentEvent.mType == EProjectBuildEventType::eProgress)
        {
            mUIController->SetProgress(mCurrentEvent.mProgress);
        }
        else if (mCurrentEvent.mType == EProjectBuildEventType::eCompleted)
        {

            // post
            //   PostProcess(mCurrentEvent.mResult);
            SetActiveState(false);
            FinishBuild(true);
        }
        else if (mCurrentEvent.mType == EProjectBuildEventType::eFailed)
        {

            // post
            //  PostProcess(mCurrentEvent.mResult);
            SetActiveState(false);
            FinishBuild(false);
        }
    }
}

bool Quad::EditorBuildManager::RequestBuildProject(const std::filesystem::path &path)
{
    if (GetActiveState())
        return false;

    // 경로 검증
    if (std::filesystem::is_directory(path) == false)
    {
        return false;
    }

    // 프로젝트 저장
    EditorProjectManager::GetInstance()->SaveProject();

    /*
    빌드에 필요한 정보 수집
    */
    ProjectBuildRequest request;
    if (!BuildRequestSnapshot(path, request))
    {
        return false;
    }

    // 작업전달
    std::shared_ptr<ProjectBuildNotifyChannel> notifyChannel = std::make_shared<ProjectBuildNotifyChannel>();

    ProjectBuildJobContext jobContext(notifyChannel);
    mTaskHandle.mNotifyChannel = notifyChannel;

    SetActiveState(true);
    mUIController->SetActive(true);
    AsyncThreadPool *threadPool = AsyncThreadPool::GetInstance();

    threadPool->Submit([this, jobContext, request]() { ExecuteBuild(jobContext, request); });

    return true;
}

bool Quad::EditorBuildManager::BuildRequestSnapshot(const std::filesystem::path &outputDirectory,
                                                    ProjectBuildRequest &oRequest)
{

    auto editorConfig = EditorConfig::GetInstance();
    auto projectConfig = ProjectConfig::GetInstance();
    auto editorSceneManager = EditorSceneManager::GetInstance();

    oRequest.mOutputDirectory = outputDirectory;
    oRequest.mProjectDirectory = projectConfig->GetProjectPath();
    oRequest.mRuntimeTemplateDirectory = editorConfig->GetExecutableDirectory() / "GameRuntimeTemplate";
    oRequest.mProjectName = projectConfig->GetProjectName();
    oRequest.mEngineDirectory = editorConfig->GetEditorRootPath();

    oRequest.mStartUpMapID = projectConfig->GetStartMapID();

    // asset list

    auto assetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();
    auto logicalFileSystem = QuadLF::LogicalFileSystem::GetInstance();
    QuadLF::LogicalFolder *logicalAssetFolder = logicalFileSystem->GetAssetFolder();
    std::stack<QuadLF::LogicalFolder *> folderStack;

    std::vector<PakWriteAssetRecord> pakWriteAssetRecordList;
    folderStack.push(logicalAssetFolder);
    while (!folderStack.empty())
    {

        QuadLF::LogicalFolder *folder = folderStack.top();
        folderStack.pop();

        const std::vector<QuadLF::LogicalNode *> &childNodeList = folder->GetChildNodeList();
        for (size_t i = 0; i < childNodeList.size(); ++i)
        {

            if (childNodeList[i]->GetNodeType() == QuadLF::ELogicalNodeType::eFile)
            {

                QuadLF::LogicalFile *logicalFile = static_cast<QuadLF::LogicalFile *>(childNodeList[i]);
                const auto &assetInfo = logicalFile->GetAssetInfo();
                PakWriteAssetRecord record;
                record.mAssetID = assetInfo.mAssetID;
                record.mAssetType = assetInfo.mAssetType;
                record.mRegistryPath = logicalFile->GetFullPath();
                record.mSerializedAssetPath = logicalFileSystem->GetPhysicalFullPath(logicalFile);

                auto *metaData = assetMetaDataManager->GetMetaData(assetInfo.mAssetID);

                std::filesystem::path rawPath = projectConfig->GetProjectRawAssetPath() / metaData->mRawFileName;

                if (std::filesystem::is_regular_file(rawPath))
                {
                    record.mRawDataPath = rawPath;
                }

                pakWriteAssetRecordList.push_back(std::move(record));
            }
            else
            {

                // FOLDER
                folderStack.push(static_cast<QuadLF::LogicalFolder *>(childNodeList[i]));
            }
        }
    }

    // 별도의 engine font texture 수집
    if (!AppendRuntimeEngineAssetRecords(pakWriteAssetRecordList))
    {
        return false;
    }

    oRequest.mPakWriteAssetRecordList = std::move(pakWriteAssetRecordList);

    return true;
}

bool Quad::EditorBuildManager::AppendRuntimeEngineAssetRecords(std::vector<PakWriteAssetRecord> &oRecords) const
{
    constexpr const char *defaultFontAtlasRegistryPath = "Engine/font_atlas.png";

    auto assetManager = CoreAsset::AssetManager::GetInstance();
    CoreAsset::AssetPtr fontAtlas = assetManager->GetAsset<CoreAsset::Texture>(defaultFontAtlasRegistryPath);
    CoreAsset::Asset *asset = fontAtlas.Get();
    if (asset == nullptr)
    {
        return false;
    }

    auto assetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();
    CoreAsset::AssetMetaData *metaData = assetMetaDataManager->GetMetaData(asset);
    if (metaData == nullptr || metaData->mDomain != CoreAsset::EAssetDomain::eEngine)
    {
        return false;
    }

    const std::filesystem::path engineAssetDirectory = EditorConfig::GetInstance()->GetEditorAssetPath();

    PakWriteAssetRecord record;
    record.mAssetID = asset->GetID();
    record.mAssetType = asset->GetType();
    record.mRegistryPath = defaultFontAtlasRegistryPath;
    record.mSerializedAssetPath =
        engineAssetDirectory / "Seri" / (std::string(asset->GetName().c_str()) + CoreAsset::GetAssetDotFileExtension());

    if (!std::filesystem::is_regular_file(record.mSerializedAssetPath))
    {
        return false;
    }

    if (metaData->mHasRawData)
    {
        record.mRawDataPath = engineAssetDirectory / "Raw" / metaData->mRawFileName;
        if (!std::filesystem::is_regular_file(record.mRawDataPath))
        {
            return false;
        }
    }

    oRecords.push_back(std::move(record));
    return true;
}

void Quad::EditorBuildManager::ExecuteBuild(ProjectBuildJobContext jobContext,
                                            const ProjectBuildRequest &projectBuildRequest)
{

    CMakeConfigureDesc cmakeConfigueDesc;
    cmakeConfigueDesc.mTargetPath = projectBuildRequest.mProjectDirectory;
    cmakeConfigueDesc.mSubDirectory = "game-build";

    int result = CMakeProjectBuilder::Configure(cmakeConfigueDesc);
    CMakeBuildDesc cmakeBuildDesc;
    if (result)

    {

        cmakeBuildDesc.mTargetPath = cmakeConfigueDesc.mTargetPath;
        cmakeBuildDesc.mSubDirectory = cmakeConfigueDesc.mSubDirectory;
        cmakeBuildDesc.mConfig = EBuildConfig::eDebug;

        result = CMakeProjectBuilder::Build(cmakeBuildDesc);
    }

    if (!result)
    {
        // 실패처리
        jobContext.ReportFailed("실패");
        return;
    }
    else
    {
        jobContext.ReportProgress(100.0f, "빌드용 프로젝트 빌드성공");
    }

    // targetPath /game-build / Debug(config)  / ProjectBuildRequest.mProjectName.dll
    // ,mProjectName.lib(임포트라이브러리)

    // 사용자 프로젝트 DLL 빌드 - 결과 DLL을 GameModule.dll로서 복사  ,  outputDirectory위치로 복사 .

    std::filesystem::path userBuildFolderPath = projectBuildRequest.mProjectDirectory / cmakeConfigueDesc.mSubDirectory;
    if (cmakeBuildDesc.mConfig == Quad::EBuildConfig::eDebug)
    {
        userBuildFolderPath /= "Debug";
    }
    else
    {
        userBuildFolderPath /= "Release";
    }

    std::filesystem::path userDll = userBuildFolderPath / (projectBuildRequest.mProjectName + ".dll");
    std::filesystem::path userLib = userBuildFolderPath / (projectBuildRequest.mProjectName + ".lib");

    std::filesystem::path outputUserDll = projectBuildRequest.mOutputDirectory / "GameModule.dll";
    std::filesystem::path outputUserLib = projectBuildRequest.mOutputDirectory / "GameModule.lib";

    // 해당 파일 복사. 이름은 GameModule.dll ,GameModule.lib로
    if (!std::filesystem::copy_file(userDll, outputUserDll))
    {
        jobContext.ReportFailed("실패");
        return;
    }

    if (!std::filesystem::copy_file(userLib, outputUserLib))
    {
        jobContext.ReportFailed("실패");
        return;
    }

    // GameDirector.exe도 outputDir로 복사
    // 그외에 필요한 dll들도 모두 복사
    //(근데 이거는 사실 실제 엔진배포시 별도로 어딘가에 모아둔다던가해서 복사하거나, 아니면 그 엔진실행파일의
    // 디렉터리에서 고정된 파일들만 복사하는걸로 생각하는데

    auto physicalFileSystem = QuadPF::PhysicalFileSystem::GetInstance();

    std::vector<std::filesystem::path> fileList;

    physicalFileSystem->GetFileList(projectBuildRequest.mRuntimeTemplateDirectory, fileList);
    for (const auto &entry : fileList)
    {
        std::filesystem::path fileName = entry.filename();
        std::filesystem::copy_file(entry, projectBuildRequest.mOutputDirectory / fileName);
    }
    // 복사성공

    // 프로젝트 저장 파일을 복사하지 않고 현재 채널 설정을 배포한다.
    // 빌드 작업이 완료될 때까지 채널 데이터의 편집과 프로젝트 전환이 차단되어야 한다.
    BinaryArch collisionArch(false);
    collisionArch.SetFile(projectBuildRequest.mOutputDirectory / "ProjectCollision.cfg");
    collisionArch.Start();
    Core::CollisionChannelSystem::GetInstance()->Serialize(collisionArch);
    collisionArch.End();

    if (collisionArch.IsFail())
    {
        jobContext.ReportFailed("충돌 채널 설정 저장 실패");
        return;
    }

    if (BuildShaders(projectBuildRequest) == false)
    {
        jobContext.ReportFailed("세이더 빌드파일 생성 실패");
        return;
    }

    /*
      에셋 패키징
     */
    PakWriter pakWriter;
    bool ret = pakWriter.StorePak(projectBuildRequest.mOutputDirectory / "Asset.pak",
                                  projectBuildRequest.mPakWriteAssetRecordList);

    if (!ret)
    {
        jobContext.ReportFailed("실패");
        return;
    }

    Core::GameBuildManifest gameBuildManifest;
    gameBuildManifest.mStartupMapAssetID = projectBuildRequest.mStartUpMapID;
    gameBuildManifest.mGameModuleRelativePath = "GameModule.dll";
    gameBuildManifest.mGameName = projectBuildRequest.mProjectName;

    std::filesystem::path gameBuildManifestPath = projectBuildRequest.mOutputDirectory / "GameBuildManifest.bin";

    Core::WriteGameBuildManifest(gameBuildManifestPath, gameBuildManifest);

    jobContext.ReportCompeleted("성공");
}

void Quad::EditorBuildManager::FinishBuild(bool success) {}

void Quad::EditorBuildManager::OnClickedExitButton()
{
    mUIController->SetActive(false);

    // 마우스캡처 풀기 등등
}

bool Quad::EditorBuildManager::BuildShaders(const ProjectBuildRequest &projectBuildRequest)
{
    auto shaderSourceLibrary = Render::ShaderSourceLibrary::GetInstance();

    std::vector<Render::ShaderVariantKey> keyList = shaderSourceLibrary->GetShaderKeyList();

    auto shaderCompiler = ShaderCompiler::GetInstance();

    Core::ShaderRecordSet recordSet;

    BinaryArch buildArch(false);
    buildArch.SetFile(projectBuildRequest.mOutputDirectory / "Shader.bin");
    buildArch.Start();

    uint32_t keyNum = keyList.size();
    recordSet.mRecordNum = keyNum;

    for (const auto &key : keyList)
    {

        const std::filesystem::path sourcePath = projectBuildRequest.mEngineDirectory / "Shader" / key.mShaderPath;
        BinaryArch arch(true);
        arch.SetFile(sourcePath);
        arch.Start();
        if (arch.IsFail())
        {
            LOG_MESSAGE_ERROR("ShaderCompile", std::string("HLSL 파일 읽기 실패: ") + sourcePath.generic_string());
            return false;
        }

        std::vector<uint8_t> sourceBuffer(arch.GetFileSize());
        if (sourceBuffer.empty() || !arch.CopyFromBuffer(sourceBuffer.data(), sourceBuffer.size()))
        {
            LOG_MESSAGE_ERROR("ShaderCompile", std::string("HLSL 파일이 비어 있거나 복사에 실패했습니다: ") +
                                                   sourcePath.generic_string());
            return false;
        }

        // 등록된 변형과 동일한 컴파일 조건을 사용한다. 원본 버퍼는 이 루프 안에서
        // 동기 Compile 호출이 끝날 때까지 유지하고, 저장 레코드에는 원본 주소를 남기지 않는다.
        Render::ShaderSourceInfo info = {};
        info.mShadeCode = sourceBuffer.data();
        info.mShaderCodeSize = sourceBuffer.size();
        info.mEntryPoint = key.mEntryPoint;
        info.mTarget = key.mTarget;
        info.mStage = key.mStage;
        info.mShaderMacros = key.mShaderMacros;

        std::vector<uint8_t> compiledBuffer;
        if (shaderCompiler->Compile(info, compiledBuffer) == false)
            return false;

        Core::ShaderBytecodeRecord record;
        // 런타임에서 동일한 키로 조회할 수 있도록 메타데이터 전체를 보존하고,
        // 컴파일 결과 버퍼의 소유권은 레코드로 옮겨 직렬화가 끝날 때까지 유지한다.
        record.mKey = key;
        record.mBytecode = std::move(compiledBuffer);

        recordSet.mRecordList.push_back(record);
    }

    buildArch << recordSet;

    buildArch.End();

    if (buildArch.IsFail())
        return false;

    return true;
}

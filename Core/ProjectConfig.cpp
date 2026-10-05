#include "Core/ProjectConfig.h"
#include <CoreBase/BinaryArch.h>

#include <CoreAsset/GlobalAssetRegistrySystem.h>
#include <Logger/Logger.h>

Quad::ProjectConfig *Quad::ProjectConfig::GetInstance()
{
    static ProjectConfig instance;

    return &instance;
}

Quad::ProjectConfig::ProjectConfig() {}

Quad::ProjectConfig::~ProjectConfig() {}

void Quad::ProjectConfig::SetProjectPath(const std::filesystem::path &path)
{
    mProjectPath = path;

    mProjectConfigFilePath = mProjectPath / "ProjectConfig.cfg";
}

void Quad::ProjectConfig::SetProjectName(const std::string &name)
{

    mProjectName = name;
}

const std::string &Quad::ProjectConfig::GetProjectName() const
{
    return mProjectName;
    // TODO: 여기에 return 문을 삽입합니다.
}

const std::filesystem::path &Quad::ProjectConfig::GetProjectPath() const
{
    return mProjectPath;
    // TODO: 여기에 return 문을 삽입합니다.
}

std::filesystem::path Quad::ProjectConfig::GetProjectRawAssetPath() const
{
    return mProjectPath / "RawAsset";
}

void Quad::ProjectConfig::Load()
{
    BinaryArch arch(true);
    arch.SetFile(mProjectConfigFilePath);
    arch.Start();
    if (arch.IsFail())
    {
        LOG_MESSAGE_CRITICAL("ProjectConfig", "프로젝트 Config파일이없습니다.");
        arch.End();
        Save();
        return;
    }

    Serialize(arch);
    arch.End();
}

void Quad::ProjectConfig::Save()
{
    BinaryArch arch(false);
    arch.SetFile(mProjectConfigFilePath);
    arch.Start();
    Serialize(arch);

    // 파일 출력은 End에서 수행되므로 출력 이후에 실패 상태를 확인한다.
    arch.End();
    if (arch.IsFail())
        LOG_MESSAGE_CRITICAL("ProjectConfig", "프로젝트 Config파일 저장에 실패했습니다.");
}

void Quad::ProjectConfig::Serialize(Arch &arch)
{
    CoreAsset::GlobalAssetRegistrySystem *globalAssetRegistrySystem =
        CoreAsset::GlobalAssetRegistrySystem::GetInstance();

    // 기존 cfg의 uint64_t NextAssetID 한 필드 형식을 유지한다. 시작 맵 ID 등의 저장 형식 확장은 별도 작업이다.
    // ID 생성기의 소유자는 Registry이므로 저장 시 현재 값을 얻고, 로드 시 읽은 값을 Registry에 적용한다.
    // BinaryArch의 읽기 실패 상태 전파가 불완전하므로 기존 값으로 초기화하여 미초기화 ID가 적용되지 않게 한다.
    CoreAsset::AssetID nextAssetID = globalAssetRegistrySystem->PeekNextAssetID();
    arch << nextAssetID;

    CoreAsset::AssetID startMapAssetID = mStartMapID;
    arch << mStartMapID;

    if (arch.GetLoadingFlag() && arch.IsGood())
        globalAssetRegistrySystem->SetNextAssetID(nextAssetID);
}

void Quad::ProjectConfig::SetStartMapID(CoreAsset::AssetID id)
{

    mStartMapID = id;
}

CoreAsset::AssetID Quad::ProjectConfig::GetStartMapID() const
{
    return mStartMapID;
}

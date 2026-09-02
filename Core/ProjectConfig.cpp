#include "Core/ProjectConfig.h"
#include <BinaryReaderWriter/BinaryReader.h>
#include <BinaryReaderWriter/BinaryWriter.h>
#include <BinaryReaderWriter/TextReader.h>
#include <BinaryReaderWriter/TextWriter.h>

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
    QuadRW::BinaryReader reader;
    // QuadRW::TextReader reader;

    CoreAsset::GlobalAssetRegistrySystem *globalAssetRegistrySystem =
        CoreAsset::GlobalAssetRegistrySystem::GetInstance();

    bool ret = reader.StartRead(mProjectConfigFilePath);
    if (ret == false)
    {
        LOG_MESSAGE_CRITICAL("ProjectConfig", "프로젝트 Config파일이없습니다.");
        Save();
    }
    else
    {

        CoreAsset::AssetID nextAssetID;
        ret = reader.Read(nextAssetID);
        globalAssetRegistrySystem->SetNextAssetID(nextAssetID);
    }
}

void Quad::ProjectConfig::Save()
{
    QuadRW::BinaryWriter writer;

    writer.StartWrite();

    CoreAsset::GlobalAssetRegistrySystem *globalAssetRegistrySystem =
        CoreAsset::GlobalAssetRegistrySystem::GetInstance();

    writer.Write(globalAssetRegistrySystem->PeekNextAssetID());

    writer.Close(mProjectConfigFilePath);
}

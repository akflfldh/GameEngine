#include "Core/ProjectConfig.h"
#include <BinaryReaderWriter/BinaryReader.h>
#include <BinaryReaderWriter/BinaryWriter.h>

#include <CoreAsset/GlobalAssetRegistrySystem.h>
#include <Logger/Logger.h>

Quad::ProjectConfig *Quad::ProjectConfig::GetInstance()
{
    static ProjectConfig instance;

    return &instance;
}

Quad::ProjectConfig::ProjectConfig() {}

Quad::ProjectConfig::~ProjectConfig() {}

void Quad::ProjectConfig::SetProjectPath(const std::string &path)
{
    mProjectPath = path;

    mProjectConfigFile = mProjectPath + "/" + "ProjectConfig.cfg";
}

const std::string &Quad::ProjectConfig::GetProjectPath() const
{
    return mProjectPath;
    // TODO: 여기에 return 문을 삽입합니다.
}

void Quad::ProjectConfig::Load()
{
    QuadRW::BinaryReader reader;

    CoreAsset::GlobalAssetRegistrySystem *globalAssetRegistrySystem =
        CoreAsset::GlobalAssetRegistrySystem::GetInstance();

    bool ret = reader.StartRead(mProjectConfigFile);
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

    writer.Close(mProjectConfigFile);
}

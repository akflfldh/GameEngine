#include "LooseFileAssetRawDataSource.h"
#include <PhysicalFileSystem/PhysicalFileSystem.h>

CoreAsset::LooseFileAssetRawDataSource::LooseFileAssetRawDataSource(
    const std::filesystem::path &fileRootPath, const std::filesystem::path &engineRawDataFileRootPath)
    : mProjectRawDataRootPath(fileRootPath), mEngineRawDataRootPath(engineRawDataFileRootPath)
{
}

CoreAsset::LooseFileAssetRawDataSource::~LooseFileAssetRawDataSource() {}

bool CoreAsset::LooseFileAssetRawDataSource::ReadSerializedAssetData(const AssetSerializedDataRequest &request,
                                                                     std::vector<uint8_t> &oBuffer) const
{

    //   const std::filesystem::path filePath = mRawDataRootPath / request.mRawDataRelativePath;

    return QuadPF::PhysicalFileSystem::GetInstance()->ReadFileToBuffer(request.mPath, oBuffer);

    //  return false;
}

bool CoreAsset::LooseFileAssetRawDataSource::ReadRawData(const AssetRawDataRequest &request,
                                                         std::vector<uint8_t> &oBuffer) const
{

    std::filesystem::path filePath;
    if (request.mDomain == EAssetDomain::eProject)
    {
        filePath = mProjectRawDataRootPath / request.mRawDataRelativePath;
    }
    else
    {
        filePath = mEngineRawDataRootPath / request.mRawDataRelativePath;
    }

    return QuadPF::PhysicalFileSystem::GetInstance()->ReadFileToBuffer(filePath, oBuffer);
}

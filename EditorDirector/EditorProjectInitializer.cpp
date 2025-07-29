#include "EditorProjectInitializer.h"

#include<assert.h>
#include<AssetCommon.h>
#include<AssetLoader.h>
#include<AssetMetaDataType.h>
#include<BinaryReader.h>
#include<queue>
#include"SerializedAssetContainer.h"
#include<LogicalFileSystem.h>
#include<LogicalFolder.h>
#include<LogicalFile.h>
#include<PhysicalFileSystem.h>
#include<TextureManager.h>




extern std::string testProjectPath;

const std::string assetRawDataFolderPath = "C:/Users/dongd/gitproject/GameEngine";

Quad::EditorProjectInitializer* Quad::EditorProjectInitializer::mInstance = nullptr;

Quad::EditorProjectInitializer* Quad::EditorProjectInitializer::GetInstance()
{
    if (mInstance == nullptr)
    {
        //log
        assert(0);
    }

    return mInstance;
}

Quad::EditorProjectInitializer::EditorProjectInitializer(QuadLF::LogicalFileSystem* logicalFileSystem, QuadPF::PhysicalFileSystem* physicalFileSystem, CoreAsset::AssetLoader* assetLoader)
    :mLogicalFileSystem(logicalFileSystem),mPhysicalFileSystem(physicalFileSystem),mAssetLoader(assetLoader)
{

    if (mInstance != nullptr)
    {
        //log
        assert(0);
    }

    mInstance = this;


}

Quad::EditorProjectInitializer::~EditorProjectInitializer()
{
}


bool Quad::EditorProjectInitializer::Initialize()
{

    //2025  07 20 
	InitLogicalDirectoryStructure();

	LoadProjectAsset();



    return true;
}

void Quad::EditorProjectInitializer::InitLogicalDirectoryStructure()
{
    //프로젝트의 논리적 폴더 구조 로드 테스트

    const std::string testLDSFileName = "testLDSFile";
    QuadRW::BinaryReader binaryReader;
    const std::string testLDSFilePath = testProjectPath + "/" + testLDSFileName + ".lds";
    bool ret = mLogicalFileSystem->LoadLogicalDirectoryStructureAsBinaryReader(binaryReader, testLDSFilePath);


}

void Quad::EditorProjectInitializer::LoadProjectAsset()
{
	

    SerializedAssetTContainer* serializedAssetContainer = SerializedAssetTContainer::GetInstance();
   
    QuadLF::LogicalFolder* assetFolder = mLogicalFileSystem->GetRootFolder();


    std::queue<QuadLF::LogicalFolder*> folderQueue;
    folderQueue.push(assetFolder);

    //각 폴더내 asset파일들을 찾아서 Load한다.
    while (!folderQueue.empty())
    {

        QuadLF::LogicalFolder* folder = folderQueue.front();
        folderQueue.pop();

        std::vector<std::string> assetFileList;
        std::string physcialFolderPath = mLogicalFileSystem->GetPhysicalFullPath(folder);
        mPhysicalFileSystem->GetFileListByExtension(physcialFolderPath, CoreAsset::GetAssetFileExtension(), assetFileList);


        for (auto& assetPath : assetFileList)
        {
            //.asset 로드
            assetPath = physcialFolderPath + "/" + assetPath;
            CoreAsset::AssetMetaData assetMetaData;
            std::unique_ptr<CoreAsset::SerializedAsset> assetData = mAssetLoader->LoadAsset(assetPath, assetMetaData);


            //rawData load
            //rawDataFile path 

            std::string parentFolderLogicalPath = folder->GetFullPath();
            std::string logicalFilePath = parentFolderLogicalPath + "/" + assetData->mAssetName;
            std::string rawDataFilePath = assetRawDataFolderPath + "/"+CoreAsset::GetAssetRawFileName(logicalFilePath);

            //.raw로드
            std::unique_ptr<CoreAsset::SerializedAssetRawData>  assetRawData = mAssetLoader->LoadAssetRawData(rawDataFilePath, assetData->mAssetType);



            //table에 타입별로 보관
            serializedAssetContainer->Register(std::move(assetData), std::move(assetRawData), assetMetaData, parentFolderLogicalPath);


        }
    }



    CreateAssetAndLogicalFile();

    serializedAssetContainer->Clear();


}

void Quad::EditorProjectInitializer::CreateAssetAndLogicalFile()
{
    //asset 생성

    SerializedAssetTContainer* serializedAssetContainer = SerializedAssetTContainer::GetInstance();
 
    //2025 07 20 
    CreateTextureAndLogicalFile(serializedAssetContainer);

  


    //material





    //mesh










    
}

void Quad::EditorProjectInitializer::CreateTextureAndLogicalFile(SerializedAssetTContainer* serializedAssetContainer)
{
    //texture
    CoreAsset::TextureManager* textureManager = CoreAsset::TextureManager::GetInstance();


    const std::vector<SerializedAssetContainerContext>& serializedAssetContextVector =
        serializedAssetContainer->GetSerializedAssetContextVector(CoreAsset::EAssetType::eTexture);


    for (const auto& context : serializedAssetContextVector)
    {
        CoreAsset::SerializedTexture* textureData = static_cast<CoreAsset::SerializedTexture*>(context.mSerializedAsset.get());
        CoreAsset::SerializedTextureRawData* textureRawData = static_cast<CoreAsset::SerializedTextureRawData*>(context.mSerializedRawAssetData.get());


        //texture 생성
        CoreAsset::Texture* texture = textureManager->CreateTexture(*textureData, std::move(*textureRawData), context.assetMetaData, context.mParentFolderPath);


        //logicalFile 생성
        QuadLF::LogicalFileAssetInfo logicalFileInfo;
        logicalFileInfo.mAssetID = textureData->mAssetID;
        logicalFileInfo.mAssetType = textureData->mAssetType;
        logicalFileInfo.mName = textureData->mAssetName;

        QuadLF::LogicalFolder* parentFolder = mLogicalFileSystem->GetFolder(context.mParentFolderPath);
        QuadLF::LogicalFile* logicalFile = mLogicalFileSystem->MakeFile(logicalFileInfo, logicalFileInfo.mName, parentFolder);

        mLogicalFileSystem->SetPhysicalBindingFlag(logicalFile, true);

    }



}

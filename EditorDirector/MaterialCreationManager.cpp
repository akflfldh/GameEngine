#include "MaterialCreationManager.h"
#include <CoreAsset/IntermediateAsset.h>
#include <EditorDirector/EditorAssetManager.h>
#include <EditorDirector/TaskUIController.h>

MaterialCreationManager *MaterialCreationManager::GetInstance()
{
    static MaterialCreationManager instance;

    return &instance;
}

MaterialCreationManager::MaterialCreationManager() {}

MaterialCreationManager::~MaterialCreationManager() {}

void MaterialCreationManager::Initialize(UI::UICanvas *canvas)
{

    mMaterialCreationUIController = MaterialCreationUIController::GetInstance();
    mMaterialCreationUIController->Initialize(canvas);
    mMaterialCreationUIController->mOnClickedCreationMaterialRequestButtonCallbackSystem.Register(
        [this](const std::string &materialName) { OnMaterialCreationRequested(materialName); });
}

void MaterialCreationManager::RequestCreatingMaterial()
{

    if (mMaterialCreationUIController)
    {
        mMaterialCreationUIController->OpenPanel();
    }
}

void MaterialCreationManager::OnMaterialCreationRequested(const std::string &mateiralName)
{

    auto editorAssetManager = Quad::EditorAssetManager::GetInstance();

    CoreAsset::IntermediateMaterial intermediateMaterial;
    intermediateMaterial.mAssetName = mateiralName.c_str();
    intermediateMaterial.mAssetType = CoreAsset::EAssetType::eMaterial;

    editorAssetManager->CreateAsset(CoreAsset::EAssetType::eMaterial, &intermediateMaterial);
}

#include "MaterialWorkSpaceManager.h"
#include <Core/LightObject.h>
#include <Core/LogicalWindow.h>
#include <Core/Map.h>
#include <Core/StaticMeshComponent.h>
#include <Core/StaticMeshObject.h>
#include <Core/WorkSpace.h>
#include <Core/World.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Material.h>
#include <EditorDirector/EditorDirector.h>
#include <EditorDirector/EditorEditMode.h>
#include <EditorDirector/EditorProjectManager.h>
#include <EditorDirector/EditorSceneController.h>
#include <EditorDirector/EditorSceneManager.h>
#include <EditorDirector/EditorSelectionManager.h>
#include <EditorDirector/EditorUIUtility.h>
#include <EditorDirector/UIScrollBox.h>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIManager.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UIUtility.h>

MaterialWorkSpaceManager *MaterialWorkSpaceManager::GetInstance()
{

    static MaterialWorkSpaceManager instance;
    return &instance;
}

MaterialWorkSpaceManager::MaterialWorkSpaceManager() {}

MaterialWorkSpaceManager::~MaterialWorkSpaceManager() {}

void MaterialWorkSpaceManager::Initialize(Core::LogicalWindow *globalLogicalWindow,
                                          BaseSelectionManager *selectionManager, const UI::UITheme &uiTheme)
{

    // InitUICavnas
    auto uiManager = UI::UIManager::GetInstance();
    UI::UICanvasID canvasID = uiManager->CreateCanvas("MaterialCanvas", UI::ECanvasSizeMode::eFixSize);
    UI::UICanvas *canvas = uiManager->GetCanvas(canvasID);
    canvas->SetTheme(uiTheme);
    mWorkSpace = std::make_unique<Core::WorkSpace>();
    InitLogicalWindow(canvas);
    mWorkSpace->AddLogicalWindow(globalLogicalWindow);
    mWorkSpace->SetGlobalOverlayWindow(globalLogicalWindow);

    InitWorld();
    InitUI(canvas);

    OnWorkSpaceInActive();
}

void MaterialWorkSpaceManager::Update(float DeltaTime) {}

void MaterialWorkSpaceManager::SetMaterial(CoreAsset::Material *material)
{
    mTargetMaterial = material;
    CopyMaterial(mTargetMaterial, mPreviewMaterial);
    mMaterialEditUIController.SetTargetMaterial(mPreviewMaterial);
}

void MaterialWorkSpaceManager::OnWorkSpaceInActive()
{

    if (mWorld)
    {
        mWorld->SetActiveState(false);
    }
}

Core::WorkSpace *MaterialWorkSpaceManager::GetWorkSpace() const
{

    return mWorkSpace.get();
}

void MaterialWorkSpaceManager::OnWorkSpaceActive()
{

    if (mWorld)
    {
        mWorld->SetActiveState(true);
    }

    mMaterialEditUIController.BeginUI(mToolbarHeight);
}

void MaterialWorkSpaceManager::InitLogicalWindow(UI::UICanvas *canvas)
{
    mLogicalWindow = std::make_unique<Core::LogicalWindow>();
    mLogicalWindow->mViewportController.SetViewportMode(Core::EViewportMode::eAnchored);

    mLogicalWindow->mViewportController.SetAnchorLeftState(true);
    mLogicalWindow->mViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
    mLogicalWindow->mViewportController.SetAnchorLeftRelValue(0.0f);

    mLogicalWindow->mViewportController.SetAnchorRightState(true);
    mLogicalWindow->mViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
    mLogicalWindow->mViewportController.SetAnchorRightRelValue(0.0f);

    mLogicalWindow->mViewportController.SetAnchorTopState(true);
    mLogicalWindow->mViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
    mLogicalWindow->mViewportController.SetAnchorTopRelValue(0.0f);
    //    mLogicalWindow->mViewportController.SetAnchorTopPixelValue(200.0f);

    mLogicalWindow->mViewportController.SetAnchorBottomState(true);
    mLogicalWindow->mViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mLogicalWindow->mViewportController.SetAnchorBottomRelValue(0.0f);

    mLogicalWindow->m3DWorldViewportController.SetViewportMode(Core::EViewportMode::eAnchored);
    mLogicalWindow->m3DWorldViewportController.SetAnchorLeftState(true);
    mLogicalWindow->m3DWorldViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::ePixel);
    mLogicalWindow->m3DWorldViewportController.SetAnchorLeftPixelValue(500.0f);

    mLogicalWindow->m3DWorldViewportController.SetAnchorRightState(true);
    mLogicalWindow->m3DWorldViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::ePixel);
    mLogicalWindow->m3DWorldViewportController.SetAnchorRightPixelValue(600.0f);

    mLogicalWindow->m3DWorldViewportController.SetAnchorTopState(true);
    mLogicalWindow->m3DWorldViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
    mLogicalWindow->m3DWorldViewportController.SetAnchorTopRelValue(0.0f);
    //    mLogicalWindow->mViewportController.SetAnchorTopPixelValue(0.0f);

    mLogicalWindow->m3DWorldViewportController.SetAnchorBottomState(true);
    mLogicalWindow->m3DWorldViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mLogicalWindow->m3DWorldViewportController.SetAnchorBottomRelValue(0.35f);

    mLogicalWindow->SetDebugGridRender(true);
    mLogicalWindow->SetActiveCanvas(canvas);

    mLogicalWindow->SetBackBufferClearColor(0.4f, 0.4f, 0.5f, 1.0f);
    // mLogicalWindow->SetWorld(EditorSceneManager::GetInstance()->GetUserWorld());
    mWorkSpace->AddLogicalWindow(mLogicalWindow.get());
}

void MaterialWorkSpaceManager::InitWorld()
{

    mWorld = std::make_unique<World>();

    auto engineMode = new EditorEditMode;
    engineMode->GetTransformGizmo().SetComponentControlState(true);

    auto map = new Map;
    map->SetName("PlayMap");

    mWorld->Register(engineMode->GetEditorMap());
    mWorld->SetEngineMode(engineMode);

    mLogicalWindow->SetWorld(mWorld.get());
    Quad::EditorSceneManager::GetInstance()->RegisterWorld("MaterialEditWorld", mWorld.get());
    mWorld->SetActiveState(false);

    mWorld->Register(map);
    mWorld->SetCurrentMap(map);

    // 실제 광원/카메라는 유지하되, 편집용 조명 시각화와 기즈모는 생성하지 않는다.
    Quad::EditorProjectManager::GetInstance()->CreateEditorObjects(map, EditorMaterialSelectionManager::GetInstance(),
                                                                   false, false);

    // Sphere Object
    mSphereElement = mWorld->CreateEntity<Core::StaticMeshObject>("SphereElement");
    mSphereElement->mStaticMeshComponent->SetMesh("Engine/Sphere");

    // Directional Light
    Core::LightObject *light = mWorld->CreateEntity<Core::LightObject>("LightObject");
    light->SetRotationLocal(45, 0, 0);

    CoreAsset::IntermediateMaterial intermediateMaterial;
    intermediateMaterial.mAssetName = "PreviewMaterial";

    mPreviewMaterial = CoreAsset::AssetManager::GetInstance()
                           ->CreateAsset(CoreAsset::EAssetType::eMaterial, intermediateMaterial, "Engine", true)
                           .As<CoreAsset::Material>();

    mSphereElement->mStaticMeshComponent->SetSubMeshMaterial(0, mPreviewMaterial);
}

void MaterialWorkSpaceManager::InitUI(UI::UICanvas *canvas)
{
    CreateToolbar(canvas);
    mMaterialEditUIController.Initialize(canvas, mPreviewMaterial);
}

void MaterialWorkSpaceManager::CreateToolbar(UI::UICanvas *canvas)
{

    auto toolbar = EditorUIUtility::Create<UI::UIImage>(canvas, "Toolbar");
    toolbar->SetSize(3000, mToolbarHeight);
    toolbar->SetColor(UI::UIColor::DarkGray);
    toolbar->SetPositionLocal(0, 0);

    float marginX = 5.0f;
    float marginY = 5.0f;

    float posX = 0.0f;
    float posY = 0.0f;

    // ToDefaultEditButton

    auto toDefaultEditButton = EditorUIUtility::CreateSmallButton(toolbar, "toDefaultEditButton");
    // EditorUIUtility의 기본 높이 유지: toDefaultEditButton->SetSize(80, 40);
    toDefaultEditButton->SetWidth(80);
    posX += marginX;
    posY += marginY;
    toDefaultEditButton->SetPositionLocal(posX, posY);
    toDefaultEditButton->mUIImageComponent->UseTexture();
    toDefaultEditButton->mUIImageComponent->SetTexture("Engine/ArrowLeft");
    toDefaultEditButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [](float, float) { Quad::EditorDirector::GetInstance()->ChangeToDefaultEditWorkSpace(); });

    posX = UI::UIUtility::ShiftPosX(posX, toDefaultEditButton, marginX);
    auto ApplyButton = EditorUIUtility::CreateSmallTextButton(toolbar, "ApplyButton");
    ApplyButton->mUIImageComponent->SetUseBorderFlag(true);

    // EditorUIUtility의 기본 높이 유지: ApplyButton->SetSize(80, 40);
    ApplyButton->SetWidth(80);
    ApplyButton->SetPositionLocal(posX, posY);
    ApplyButton->mUIImageComponent->NotUseTexture();
    ApplyButton->mTextComponent->SetText("적용");
    // EditorUIUtility의 공통 폰트 규격 유지: ApplyButton->mTextComponent->SetFontSize(30.0f);
    ApplyButton->mTextComponent->SetPaddingLeft(10.0f);
    ApplyButton->mTextComponent->SetPaddingTop(5.0f);
    ApplyButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float)
                                                                         { ApplyPreviewMaterialToTargetMaterial(); });

    posX = UI::UIUtility::ShiftPosX(posX, ApplyButton, marginX);
}

void MaterialWorkSpaceManager::ApplyPreviewMaterialToTargetMaterial()
{

    CopyMaterial(mPreviewMaterial, mTargetMaterial);
    mTargetMaterial->SetRawDataDirty(true);
}

void MaterialWorkSpaceManager::CopyMaterial(CoreAsset::Material *sourceMaterial, CoreAsset::Material *targetMaterial)
{

    if (sourceMaterial == nullptr || targetMaterial == nullptr)
        return;

    targetMaterial->SetDiffuseColor(sourceMaterial->GetDiffuseColor());
    targetMaterial->SetDiffuseFactor(sourceMaterial->GetDiffuseFactor());
    targetMaterial->SetSpecular(sourceMaterial->GetSpecular());
    targetMaterial->SetSpecularFactor(sourceMaterial->GetSpecularFactor());
    targetMaterial->SetMetallic(sourceMaterial->GetMetallic());
    targetMaterial->SetRoughness(sourceMaterial->GetRoughness());
    targetMaterial->SetUseExplicitGpuMaterial(sourceMaterial->GetUseExplicitGpuMaterial());
    targetMaterial->SetEmissiveColor(sourceMaterial->GetEmissiveColor());
    targetMaterial->SetEmissiveIntensity(sourceMaterial->GetEmissiveIntensity());
    targetMaterial->SetUVTiling(sourceMaterial->GetUVTiling());
    // 열 때 원본→프리뷰, 적용할 때 프리뷰→원본에 동일한 UV 편집 속성을 전달한다.
    targetMaterial->SetUVRotation(sourceMaterial->GetUVRotation());
    targetMaterial->SetUVRotationPivot(sourceMaterial->GetUVRotationPivot());
    std::vector<CoreAsset::AssetMaterialTexResourceContext> &targetAlbedoList =
        targetMaterial->GetAlbedoTexResourceList();
    targetAlbedoList.clear();

    const std::vector<CoreAsset::AssetMaterialTexResourceContext> &previewAlbedoList =
        sourceMaterial->GetAlbedoTexResourceList();
    for (size_t i = 0; i < previewAlbedoList.size(); ++i)
    {
        targetMaterial->AddAlbedoTexSlot();
        targetMaterial->SetTextureResource(static_cast<int>(i), previewAlbedoList[i].mTexture.GetAssetID());
    }

    targetMaterial->SetHasNormalMap(sourceMaterial->HasNormalMap());
    if (sourceMaterial->HasNormalMap())
    {
        CoreAsset::Texture *normalTexture = sourceMaterial->GetNormalTexResource().mTexture.As<CoreAsset::Texture>();
        targetMaterial->SetNormalMapTexture(normalTexture);
    }

    //  targetMaterial->SetRawDataDirty(true);
    targetMaterial->SetUploadDirty();
}

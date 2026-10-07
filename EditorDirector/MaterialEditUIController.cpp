#include "MaterialEditUIController.h"
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Material.h>
#include <EditorDirector/EditorUIUtility.h>
#include <EditorDirector/UIAssetSlotPanel.h>
#include <EditorDirector/UIReflectVector2Panel.h>
#include <UIAssetSlotListPanel.h>
#include <UIReflectFloatPanel.h>
#include <UIReflectVector3Panel.h>
#include <UIScrollBox.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>

MaterialEditUIController::MaterialEditUIController() {}

MaterialEditUIController::~MaterialEditUIController() {}

void MaterialEditUIController::Initialize(UI::UICanvas *canvas, CoreAsset::Material *previewMaterial)
{
    mCanvas = canvas;
    mPreviewMaterial = previewMaterial;
}

void MaterialEditUIController::BeginUI(float panelPosY)
{
    if (mBegun)
        return;

    mBegun = true;

    auto ApplyPreviewButton = EditorUIUtility::CreateSmallTextButton(mCanvas, "ApplyPreviewButton");
    // EditorUIUtility의 기본 높이 유지: ApplyPreviewButton->SetSize(200, 40);
    ApplyPreviewButton->SetWidth(200);
    ApplyPreviewButton->SetHorizontalAnchor(1.0f);
    ApplyPreviewButton->SetHorizontalOffset(-(ApplyPreviewButton->GetSize().X + 0.0f));

    ApplyPreviewButton->SetVerticalAnchor(0.0f);
    ApplyPreviewButton->SetVerticalOffset(panelPosY);
    // 자체 Pivot이 (0, 0)이므로 기존 우측/하단 정렬은 크기를 포함한 음수 Offset으로 보존한다.
    ApplyPreviewButton->mOnChangedSizeCallbackSystem.Register(
        [](UI::UIElement *element)
        {
            element->SetHorizontalOffset(-(element->GetSize().X + 0.0f));
            element->UpdatePosAnchor();
        });

    ApplyPreviewButton->mUIImageComponent->NotUseTexture();
    ApplyPreviewButton->mTextComponent->SetText("프리뷰 적용");
    // EditorUIUtility의 공통 폰트 규격 유지: ApplyPreviewButton->mTextComponent->SetFontSize(30.0f);
    // EditorUIUtility의 기본 색상 유지: ApplyPreviewButton->mUIImageComponent->SetColor(0.3f, 0.3f, 0.3f);
    ApplyPreviewButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float)
                                                                                { OnClickedApplyEditDataButton(); });

    UIScrollBox *scrollProperetyBox = EditorUIUtility::Create<UIScrollBox>(mCanvas, "PropertyPanel");
    scrollProperetyBox->SetLayout(EUIScrollLayout::eVertical);
    scrollProperetyBox->SetSize(600, 1500);

    scrollProperetyBox->SetHorizontalAnchor(1.0f);
    scrollProperetyBox->SetHorizontalOffset(-(scrollProperetyBox->GetSize().X + 0.0f));

    scrollProperetyBox->SetVerticalAnchor(0.0f);
    scrollProperetyBox->SetVerticalOffset(panelPosY + 40);
    // 자체 Pivot이 (0, 0)이므로 기존 우측/하단 정렬은 크기를 포함한 음수 Offset으로 보존한다.
    scrollProperetyBox->mOnChangedSizeCallbackSystem.Register(
        [](UI::UIElement *element)
        {
            element->SetHorizontalOffset(-(element->GetSize().X + 0.0f));
            element->UpdatePosAnchor();
        });

    mScrollBox = scrollProperetyBox;

    mDiffuseColorReflectPanel = CreateVector3ReflectPanel(
        "디퓨즈 컬러", [this]() { return GetDiffuseColor(); },
        [this](const CoreMath::Vector3 &value) { SetDiffuseColor(value); });
    mScrollBox->AddItem(mDiffuseColorReflectPanel);

    mDiffuseFactorReflectPanel = CreateFloatReflectPanel(
        "디퓨즈 Factor", [this]() { return GetDiffuseFactor(); }, [this](float value) { SetDiffuseFactor(value); });

    mScrollBox->AddItem(mDiffuseFactorReflectPanel);

    mRoughnessReflectPanel = CreateFloatReflectPanel(
        "거칠기", [this]() { return GetRoughness(); }, [this](float value) { SetRoughness(value); });

    mMetalicReflectPanel = CreateFloatReflectPanel(
        "금속성", [this]() { return GetMetalic(); }, [this](float value) { SetMetalic(value); });

    mEmissiveColorReflectPanel = CreateVector3ReflectPanel(
        "Emissive 색상", [this]() { return GetEmissiveColor(); },
        [this](const CoreMath::Vector3 &value) { SetEmissiveColor(value); });

    mEmissiveIntensityReflectPanel = CreateFloatReflectPanel(
        "Emissive 강도", [this]() { return GetEmissiveIntensity(); },
        [this](float value) { SetEmissiveIntensity(value); });

    mUVTilingReflectPanel = EditorUIUtility::CreateVector2Field(mCanvas, "UVTilingPanel");
    mUVTilingReflectPanel->SetTagText("UV 타일링");
    mUVTilingReflectPanel->BindVector2([this]() { return GetUVTiling(); },
                                       [this](const CoreMath::Vector2 &value) { SetUVTiling(value); });

    // 기존 속성처럼 임시 편집 데이터에만 기록하고, 적용 버튼 경로에서 머터리얼에 반영한다.
    mUVRotationReflectPanel = CreateFloatReflectPanel(
        "UV 회전", [this]() { return GetUVRotation(); }, [this](float value) { SetUVRotation(value); });

    mUVRotationPivotReflectPanel = EditorUIUtility::CreateVector2Field(mCanvas, "UVRotationPivotPanel");
    mUVRotationPivotReflectPanel->SetTagText("UV 회전 피벗");
    mUVRotationPivotReflectPanel->BindVector2(
        [this]() { return GetUVRotationPivot(); },
        [this](const CoreMath::Vector2 &value) { SetUVRotationPivot(value); });

    mScrollBox->AddItem(mUVTilingReflectPanel);
    mScrollBox->AddItem(mUVRotationReflectPanel);
    mScrollBox->AddItem(mUVRotationPivotReflectPanel);
    mScrollBox->AddItem(mRoughnessReflectPanel);
    mScrollBox->AddItem(mMetalicReflectPanel);
    mScrollBox->AddItem(mEmissiveColorReflectPanel);
    mScrollBox->AddItem(mEmissiveIntensityReflectPanel);

    mDiffuseMapListSlotPanel = EditorUIUtility::Create<UIAssetSlotListPanel>(mCanvas, "DiffuseSlotListPanel");
    mDiffuseMapListSlotPanel->mOnClickedAddButtonCallbackSystem.Register([this]() { OnClickedDiffuseMapAddButtotn(); });
    mDiffuseMapListSlotPanel->mOnDroppedAssetCallbackSystem.Register([this](CoreAsset::AssetID id, int index)
                                                                     { OnDiffuseMapDropped(id, index); });

    mDiffuseMapListSlotPanel->SetDragPayloadType(EDragDropType::eAssetTexture);

    mDiffuseMapListSlotPanel->SetTagText("디퓨즈 텍스처");

    mScrollBox->AddItem(mDiffuseMapListSlotPanel);

    mNormalMapSlotPanel = EditorUIUtility::Create<UIAssetSlotPanel>(mCanvas, "NormalMapPanel");

    mNormalMapSlotPanel->mOnDroppedAssetCallbackSystem.Register([this](CoreAsset::AssetID assetID)
                                                                { OnNormalMapDropped(assetID); });
    mNormalMapSlotPanel->SetDragPayloadType(EDragDropType::eAssetTexture);
    mNormalMapSlotPanel->SetTagText("노멀 맵 ");

    mScrollBox->AddItem(mNormalMapSlotPanel);
}

void MaterialEditUIController::SetTargetMaterial(CoreAsset::Material *material)
{

    Clear();

    if (material == nullptr)
        return;

    mTargetMaterial = material;
    // 복사본으로 작업하고 적용버튼을 누르면 동기화를 하면서 적용해야할것
    BuildMaterialEdtiData(*mTargetMaterial, mMaterialEditData);
    ApplyEditDataToMaterial(mMaterialEditData, *mPreviewMaterial);

    if (mTargetMaterial)
        ReBuild();
}

void MaterialEditUIController::Clear()
{

    mDiffuseMapListSlotPanel->ActiveAssetSlotPanels(0);

    CoreAsset::Texture *tex =
        CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::Texture>("Engine/Texture").As<CoreAsset::Texture>();
    for (size_t i = 0; i < mDiffuseMapListSlotPanel->GetSlotNum(); ++i)
    {
        mDiffuseMapListSlotPanel->SetAssetSlotImage(tex, i);
    }

    mNormalMapSlotPanel->SetSlotImage(tex);
}

void MaterialEditUIController::ReBuild()
{

    // reflectPanel
    mDiffuseColorReflectPanel->RefreshFromSource();
    mDiffuseFactorReflectPanel->RefreshFromSource();
    mRoughnessReflectPanel->RefreshFromSource();
    mUVTilingReflectPanel->RefreshFromSource();
    mUVRotationReflectPanel->RefreshFromSource();
    mUVRotationPivotReflectPanel->RefreshFromSource();

    // ReBuildDiffuseMapListPanel

    mDiffuseMapListSlotPanel->ActiveAssetSlotPanels(mMaterialEditData.mAlbedoTextures.size());

    for (int i = 0; i < mMaterialEditData.mAlbedoTextures.size(); ++i)
    {
        mDiffuseMapListSlotPanel->SetAssetSlotImage(mMaterialEditData.mAlbedoTextures[i].As<CoreAsset::Texture>(), i);
    }

    mNormalMapSlotPanel->SetSlotImage(mMaterialEditData.mNormalTexture.As<CoreAsset::Texture>());
}

void MaterialEditUIController::OnClickedDiffuseMapAddButtotn()
{

    mMaterialEditData.mAlbedoTextures.push_back(nullptr);

    ReBuild();
}

void MaterialEditUIController::OnDiffuseMapDropped(CoreAsset::AssetID id, int index)
{

    if (mMaterialEditData.mAlbedoTextures.size() <= index)
        return;

    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();

    CoreAsset::Texture *texture = assetManager->GetAsset<CoreAsset::Texture>(id).As<CoreAsset::Texture>();

    if (texture == nullptr)
        return;

    mMaterialEditData.mAlbedoTextures[index] = texture;

    mDiffuseMapListSlotPanel->SetAssetSlotImage(texture, index);
}

void MaterialEditUIController::OnNormalMapDropped(CoreAsset::AssetID id)
{

    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();

    CoreAsset::Texture *texture = assetManager->GetAsset<CoreAsset::Texture>(id).As<CoreAsset::Texture>();

    if (texture == nullptr)
        return;

    mMaterialEditData.mNormalTexture = texture;

    mNormalMapSlotPanel->SetSlotImage(texture);

    mMaterialEditData.mHasNormalMap = true;
}

void MaterialEditUIController::BuildMaterialEdtiData(const CoreAsset::Material &material, MaterialEditData &oEditData)
{

    oEditData.mDiffuseColor = material.GetDiffuseColor();
    oEditData.mDiffuseFactor = material.GetDiffuseFactor();
    oEditData.mSpecular = material.GetSpecular();
    oEditData.mSpecularFactor = material.GetSpecularFactor();

    oEditData.mUVTiling = material.GetUVTiling();
    oEditData.mUVRotation = material.GetUVRotation();
    oEditData.mUVRotationPivot = material.GetUVRotationPivot();
    oEditData.mRoughness = material.GetRoughness();
    oEditData.mMetallic = material.GetMetallic();
    oEditData.mUseExplicitGpuMaterial = material.GetUseExplicitGpuMaterial();
    oEditData.mHasNormalMap = material.HasNormalMap();
    oEditData.mEmissiveColor = material.GetEmissiveColor();
    oEditData.mEmissiveIntensity = material.GetEmissiveIntensity();

    oEditData.mAlbedoTextures.clear();
    for (const auto &e : material.GetAlbedoTexResourceList())
    {
        oEditData.mAlbedoTextures.push_back(e.mTexture);
    }
    oEditData.mNormalTexture = material.GetNormalTexResource().mTexture;
}

void MaterialEditUIController::ApplyEditDataToMaterial(const MaterialEditData &editData, CoreAsset::Material &material)
{

    // dirty
    // 동기화 필요

    material.SetDiffuseColor(editData.mDiffuseColor);
    material.SetDiffuseFactor(editData.mDiffuseFactor);
    material.SetSpecular(editData.mSpecular);
    material.SetSpecularFactor(editData.mSpecularFactor);
    material.SetMetallic(editData.mMetallic);
    material.SetRoughness(editData.mRoughness);
    material.SetEmissiveColor(editData.mEmissiveColor);
    material.SetEmissiveIntensity(editData.mEmissiveIntensity);
    material.SetUseExplicitGpuMaterial(editData.mUseExplicitGpuMaterial);
    material.SetUVTiling(editData.mUVTiling);
    material.SetUVRotation(editData.mUVRotation);
    material.SetUVRotationPivot(editData.mUVRotationPivot);

    std::vector<CoreAsset::AssetMaterialTexResourceContext> &albedoList = material.GetAlbedoTexResourceList();
    albedoList.clear();
    for (int i = 0; i < editData.mAlbedoTextures.size(); ++i)
    {
        material.AddAlbedoTexSlot();
        material.SetTextureResource(i, editData.mAlbedoTextures[i].GetAssetID());
    }

    material.SetHasNormalMap(editData.mHasNormalMap);
    if (editData.mHasNormalMap)
    {
        CoreAsset::Texture *normalTexture = editData.mNormalTexture.As<CoreAsset::Texture>();
        if (normalTexture != nullptr)
        {
            material.SetNormalMapTexture(normalTexture);
        }
    }

    // material.SetRawDataDirty(true);
    material.SetUploadDirty();
}

void MaterialEditUIController::OnClickedApplyEditDataButton()
{
    ApplyEditDataToMaterial(mMaterialEditData, *mPreviewMaterial);
    // ReBuild();
}
CoreMath::Vector3 MaterialEditUIController::GetDiffuseColor() const
{

    return mMaterialEditData.mDiffuseColor;
}
float MaterialEditUIController::GetDiffuseFactor() const
{

    return mMaterialEditData.mDiffuseFactor;
}
float MaterialEditUIController::GetRoughness() const
{

    return mMaterialEditData.mRoughness;
}

float MaterialEditUIController::GetMetalic() const
{
    return mMaterialEditData.mMetallic;
}

CoreMath::Vector3 MaterialEditUIController::GetEmissiveColor() const
{

    return mMaterialEditData.mEmissiveColor;
}
float MaterialEditUIController::GetEmissiveIntensity() const
{

    return mMaterialEditData.mEmissiveIntensity;
}

CoreMath::Vector2 MaterialEditUIController::GetUVTiling() const
{

    return mMaterialEditData.mUVTiling;
}

float MaterialEditUIController::GetUVRotation() const
{
    return mMaterialEditData.mUVRotation;
}

CoreMath::Vector2 MaterialEditUIController::GetUVRotationPivot() const
{
    return mMaterialEditData.mUVRotationPivot;
}

void MaterialEditUIController::SetDiffuseColor(const CoreMath::Vector3 &value)
{

    mMaterialEditData.mDiffuseColor = value;
}
void MaterialEditUIController::SetDiffuseFactor(float value)
{
    mMaterialEditData.mDiffuseFactor = value;
}

void MaterialEditUIController::SetRoughness(float value)
{
    if (value < 0.0f)
        value = 0.0f;
    mMaterialEditData.mRoughness = value;
}

void MaterialEditUIController::SetMetalic(float value)
{

    if (value < 0.0f)
        value = 0.0f;
    mMaterialEditData.mMetallic = value;
}

void MaterialEditUIController::SetEmissiveColor(const CoreMath::Vector3 &value)
{

    mMaterialEditData.mEmissiveColor = value;
}
void MaterialEditUIController::SetEmissiveIntensity(float value)
{

    mMaterialEditData.mEmissiveIntensity = value;
}
void MaterialEditUIController::SetUVTiling(const CoreMath::Vector2 &value)
{

    mMaterialEditData.mUVTiling = value;
}

void MaterialEditUIController::SetUVRotation(float value)
{
    mMaterialEditData.mUVRotation = value;
}

void MaterialEditUIController::SetUVRotationPivot(const CoreMath::Vector2 &value)
{
    mMaterialEditData.mUVRotationPivot = value;
}

UIReflectVector3Panel *MaterialEditUIController::CreateVector3ReflectPanel(
    const std::string &tagText, std::function<CoreMath::Vector3()> getter,
    std::function<void(const CoreMath::Vector3 &)> setter)
{

    UIReflectVector3Panel *vectorReflectPanel = EditorUIUtility::CreateVector3Field(mCanvas, "Vector3ReflectPanel");
    vectorReflectPanel->SetTagText(tagText);
    vectorReflectPanel->SetWidth(600.0f);
    // vectorReflectPanel->SetColor(0.3f, 0.3f, 0.3f);
    vectorReflectPanel->BindVector3(getter, setter);

    return vectorReflectPanel;
}

UIReflectFloatPanel *MaterialEditUIController::CreateFloatReflectPanel(const std::string &tagText,
                                                                       std::function<float()> getter,
                                                                       std::function<void(float)> setter)
{

    UIReflectFloatPanel *floatReflectPanel = EditorUIUtility::CreateFloatField(mCanvas, "FloatReflectPanel");
    floatReflectPanel->SetTagText(tagText);
    floatReflectPanel->SetWidth(600.0f);
    //  floatReflectPanel->SetColor(0.3f, 0.3f, 0.3f);
    floatReflectPanel->BindFloat(getter, setter);
    return floatReflectPanel;
}

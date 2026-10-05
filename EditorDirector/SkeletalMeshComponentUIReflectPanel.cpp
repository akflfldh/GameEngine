#include "SkeletalMeshComponentUIReflectPanel.h"
#include <EditorDirector/EditorUIUtility.h>

#include <Core/SkeletalMeshComponent.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/SkinningMesh.h>
#include <CoreAsset/Texture.h>
#include <EditorDirector/GlobalOverlayType.h>
#include <EditorDirector/UIAssetSlotPanel.h>
#include <EditorDirector/UIDropTargetComponent.h>
#include <EditorDirector/UIFoldoutPanel.h>
#include <EditorInspectorUtility.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UIVerticalLayoutComponent.h>
#include <string>

SkeletalMeshComponentUIReflectPanel::SkeletalMeshComponentUIReflectPanel()
{
    SetStyleRole(UI::EUIStyleRole::ePanel);
    CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
}

SkeletalMeshComponentUIReflectPanel::~SkeletalMeshComponentUIReflectPanel() = default;

void SkeletalMeshComponentUIReflectPanel::Build(Component *component)
{
    mDestMeshComponent = static_cast<SkeletalMeshComponent *>(component);
    RefreshComponentMesh();
}

void SkeletalMeshComponentUIReflectPanel::BindProperty(void *, Quad::PropertyInfo *) {}

void SkeletalMeshComponentUIReflectPanel::Release()
{
    // pool 재사용 시 이전 컴포넌트로 drop callback이 적용되지 않도록 대상 참조를 해제한다.
    mDestMeshComponent = nullptr;
    RefreshComponentMesh();
}

void SkeletalMeshComponentUIReflectPanel::OnBegin()
{
    UI::UIImage::OnBegin();

    const float width = GetWidth();
    mMeshFoldPanel = EditorUIUtility::CreateFoldoutPanel(this, "SkeletalMeshFoldPanel");
    mMeshFoldPanel->SetHeaderText("Skeletal Mesh");
    mMeshFoldPanel->SetWidth(width);
    mMeshFoldPanel->SetExpanded(true);

    mMeshFoldPanel->SetHeaderColor(UI::UIColor::DarkYellow);

    mMeshPanel = EditorUIUtility::CreatePanel(this, "SkeletalMeshPanel");

    mMeshPanel->SetHeight(200.0f);
    mMeshFoldPanel->AddItem(mMeshPanel);

    auto meshTag = EditorUIUtility::CreateLabel(mMeshPanel, "MeshTag");
    meshTag->SetText("스키닝 메시");

    auto meshImage = EditorUIUtility::Create<UI::UIImage>(mMeshPanel, "MeshImagePanel");
    meshImage->SetSize(100.0f, 100.0f);
    meshImage->SetPositionLocal(10.0f, 50.0f);
    meshImage->UseTexture(true);
    meshImage->SetTexture("Engine/SkinningMesh");

    auto dropTarget = meshImage->CreateUIComponent<UIDropTargetComponent>("DropTargetCom");
    dropTarget->SetDragDropPayloadType(EDragDropType::eAssetSkinningMesh);
    dropTarget->mOnDroppedPayloadCallbackSystem.Register(
        [this](const DragPayload &payload) { SetMesh(payload.mAssetID); });

    mMeshText = EditorUIUtility::CreateLabel(mMeshPanel, "MeshText");
    mMeshText->SetPositionLocal(10.0f, 160.0f);
    mMeshText->SetWidth(width - 20.0f);

    mMaterialPanel = EditorUIUtility::CreatePanel(this, "SkeletalMaterialPanel");

    mMaterialPanel->SetSize(width, 0.0f);
    mMaterialPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
    mMeshFoldPanel->AddItem(mMaterialPanel);

    RefreshComponentMesh();
}

void SkeletalMeshComponentUIReflectPanel::SetMesh(CoreAsset::AssetID id)
{
    if (mDestMeshComponent == nullptr)
        return;

    auto mesh = CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::SkinningMesh>(id);
    if (mesh.As<CoreAsset::SkinningMesh>() == nullptr)
        return;

    // SetMesh가 인스턴스별 material 목록과 palette cache를 함께 재구축한다.
    mDestMeshComponent->SetMesh(id);
    Quad::CommitInspectorEdit(mDestMeshComponent);
    RefreshComponentMesh();
}

void SkeletalMeshComponentUIReflectPanel::SetSubMeshMaterial(CoreAsset::AssetID id, size_t index)
{
    if (mDestMeshComponent == nullptr || index >= mDestMeshComponent->GetSubMeshMaterialList().size())
        return;

    auto material = CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::Material>(id);
    if (material.As<CoreAsset::Material>() == nullptr)
        return;

    mDestMeshComponent->SetSubMeshMaterial(index, id);
    Quad::CommitInspectorEdit(mDestMeshComponent);
    RefreshComponentMesh();
}

UIAssetSlotPanel *SkeletalMeshComponentUIReflectPanel::CreateSubMaterialPanel(size_t index)
{
    auto panel = EditorUIUtility::Create<UIAssetSlotPanel>(mMaterialPanel, "SubMaterialPanel");
    panel->SetDragPayloadType(EDragDropType::eAssetMaterial);
    EditorUIUtility::ApplyPreset(panel, UI::EUIStyleRole::ePanel);
    panel->SetWidth(mMaterialPanel->GetWidth());
    panel->SetHeight(150.0f);
    auto materialIcon = CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::Texture>("Engine/MaterialTexture");
    if (auto *texture = materialIcon.As<CoreAsset::Texture>())
        panel->SetSlotImage(texture);
    panel->mOnDroppedAssetCallbackSystem.Register(
        [this, index](CoreAsset::AssetID id) { SetSubMeshMaterial(id, index); });
    return panel;
}

void SkeletalMeshComponentUIReflectPanel::RefreshComponentMesh()
{
    if (mMeshText == nullptr)
        return;

    CoreAsset::SkinningMesh *mesh = mDestMeshComponent != nullptr ? mDestMeshComponent->GetSkinningMesh() : nullptr;
    mMeshText->SetText(mesh != nullptr ? mesh->GetName().c_str() : "선택된 메시 없음");

    const std::vector<CoreAsset::AssetPtr> *materials =
        mDestMeshComponent != nullptr && mesh != nullptr ? &mDestMeshComponent->GetSubMeshMaterialList() : nullptr;
    const size_t materialCount = materials != nullptr ? materials->size() : 0;

    // Inspector panel은 pool에서 재사용되므로 슬롯도 재사용하고, 이전 메시의 남는 슬롯은 숨긴다.
    for (size_t index = mSubMaterialPanels.size(); index < materialCount; ++index)
        mSubMaterialPanels.push_back(CreateSubMaterialPanel(index));

    for (size_t index = 0; index < mSubMaterialPanels.size(); ++index)
    {
        UIAssetSlotPanel *panel = mSubMaterialPanels[index];
        panel->SetActiveFlag(index < materialCount);
        if (index >= materialCount)
            continue;

        CoreAsset::Asset *material = (*materials)[index].Get();
        const std::string materialName = material != nullptr ? material->GetName().c_str() : "없음";
        panel->SetTagText("머터리얼 " + std::to_string(index) + " : " + materialName);
    }
}

void SkeletalMeshComponentUIReflectPanel::OnTransformChanged(UI::ETransformChangeType type)
{
    UI::UIImage::OnTransformChanged(type);
    if (type == UI::ETransformChangeType::eAll || type == UI::ETransformChangeType::eSize)
    {
        if (mMeshFoldPanel)
            mMeshFoldPanel->SetWidth(GetWidth());
        if (mMaterialPanel)
            mMaterialPanel->SetWidth(GetWidth());
        if (mMeshText)
            mMeshText->SetWidth(GetWidth() - 20.0f);
        for (UIAssetSlotPanel *panel : mSubMaterialPanels)
            panel->SetWidth(GetWidth());
    }
}

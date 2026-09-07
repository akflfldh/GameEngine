#include "StaticMeshComponentUIReflectPanel.h"
#include <Core/StaticMeshComponent.h>
// #include <CoreAsset/AssetManager.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/StaticMesh.h>
#include <EditorDirector/UIDropdown.h>
#include <EditorDirector/UIFoldoutPanel.h>
#include <EditorInspectorUtility.h>
#include <GlobalOverlayType.h>
#include <UIBoolPanel.h>
#include <UIDropTargetComponent.h>
#include <UIReflectFloatPanel.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UIVerticalLayoutComponent.h>

StaticMeshComponentUIReflectPanel::StaticMeshComponentUIReflectPanel()
{
    SetStyleRole(UI::EUIStyleRole::ePanel);

    mVerticalLayoutCom = CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
}

StaticMeshComponentUIReflectPanel ::~StaticMeshComponentUIReflectPanel() {}

void StaticMeshComponentUIReflectPanel::Build(Component *com)
{

    mDestMeshComponent = static_cast<StaticMeshComponent *>(com);

    BuildComponentMesh();
    RefreshPhysicsProperties();
}

void StaticMeshComponentUIReflectPanel::OnBegin()
{
    UI::UIElement::OnBegin();
    auto canvas = GetDestCanvas();

    float width = GetWidth();

    mMeshFoldPanel = CreateChildUIElement<UIFoldoutPanel>("MeshFoldPanel");
    mMeshFoldPanel->SetHeaderText("Mesh");
    mMeshFoldPanel->SetWidth(width);
    mMeshFoldPanel->SetExpanded(true);
    mMeshFoldPanel->SetStyleRole(UI::EUIStyleRole::ePanel);
    mMeshFoldPanel->SetHeaderColor(UI::UIColor::DarkYellow);

    mMeshPanel = CreateChildUIElement<UI::UIImage>("MeshPanel");
    mMeshPanel->SetStyleRole(UI::EUIStyleRole::ePanel);
    // mMeshPanel->SetColor(0, 0.5f, 0);
    mMeshPanel->SetHeight(200);
    mMeshFoldPanel->AddItem(mMeshPanel);

    auto meshTag = mMeshPanel->CreateChildUIElement<UI::UIText>("MeshTag");
    // meshTag->SetTextColor({1, 1, 1});
    meshTag->SetText("메시");
    // meshTag->SetFontSize(20.0f);
    //    meshTag->SetPositionLocal(10, 10);

    auto mMeshImagePanel = mMeshPanel->CreateChildUIElement<UI::UIImage>("MeshImagePanel");
    mMeshImagePanel->SetSize(100, 100);
    mMeshImagePanel->SetPositionLocal(10, 50);
    UIDropTargetComponent *dropTargetCom = mMeshImagePanel->CreateUIComponent<UIDropTargetComponent>("DropTargetCom");
    dropTargetCom->SetDragDropPayloadType(EDragDropType::eAssetStaticMesh);
    dropTargetCom->mOnDroppedPayloadCallbackSystem.Register([this](const DragPayload &payload)
                                                            { SetMesh(payload.mAssetID); });

    mMeshText = mMeshPanel->CreateChildUIElement<UI::UIText>("MeshText");
    //    mMeshText->SetTextColor({1, 1, 1});
    //   mMeshText->SetFontSize(20.0f);
    mMeshText->SetPositionLocal(mMeshImagePanel->mTransform.GetLocalPosition().x,
                                mMeshImagePanel->mTransform.GetSize().y +
                                    mMeshImagePanel->mTransform.GetLocalPosition().y + 10.0f);

    mMaterialPanel = CreateChildUIElement<UI::UIImage>("MaterialPanel");
    mMaterialPanel->SetStyleRole(UI::EUIStyleRole::ePanel);
    // mMaterialPanel->SetColor(0.5f, 0, 0);
    mMaterialPanel->SetSize(600, 0);
    mMaterialPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");

    mMeshFoldPanel->AddItem(mMaterialPanel);

    BuildPhysicsProperties();

    if (mDestMeshComponent)
    {
        BuildComponentMesh();
        RefreshPhysicsProperties();
    }
}

void StaticMeshComponentUIReflectPanel::BuildPhysicsProperties()
{
    float width = GetWidth();

    mPhysicsPanel = CreateChildUIElement<UIFoldoutPanel>("PhysicsPanel");
    mPhysicsPanel->SetHeaderText("Physics");
    mPhysicsPanel->SetWidth(width);
    mPhysicsPanel->SetExpanded(true);
    mPhysicsPanel->SetStyleRole(UI::EUIStyleRole::ePanel);
    mPhysicsPanel->SetHeaderColor(UI::UIColor::DarkYellow);

    // mPhysicsPanel->SetColor(0.25f, 0.25f, 0.25f);
    // mPhysicsPanel->SetSize(600.0f, 230.0f);
    // mPhysicsPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");

    // auto physicsTitle = mPhysicsPanel->CreateChildUIElement<UI::UIText>("PhysicsTitle");
    // physicsTitle->SetText("Physics");
    ////  physicsTitle->SetTextColor({1.0f, 1.0f, 1.0f});
    ////  physicsTitle->SetFontSize(22.0f);
    // physicsTitle->SetHeight(35.0f);
    // physicsTitle->SetPositionLocal(20.0f, 5.0f);

    mPhysicsEnabledPanel = mPhysicsPanel->CreateChildUIElement<UIBoolPanel>("PhysicsEnabledPanel");
    mPhysicsPanel->AddItem(mPhysicsEnabledPanel);

    mPhysicsEnabledPanel->SetTagText("Enable Physics");
    //  mPhysicsEnabledPanel->SetColor(0.4f, 0.4f, 0.4f);
    mPhysicsEnabledPanel->SetWidth(mPhysicsPanel->mTransform.GetSize().x);

    mPhysicsEnabledPanel->mOnValueChanged.Register(
        [this](bool value)
        {
            if (mDestMeshComponent == nullptr)
                return;

            mDestMeshComponent->SetPhysicsEnabled(value);
            Quad::CommitInspectorEdit(mDestMeshComponent);
        });

    mPhysicsBodyTypeDropdown = mPhysicsPanel->CreateChildUIElement<UIDropdown>("PhysicsBodyTypeDropdown");
    // mPhysicsBodyTypeDropdown->SetSize(250.0f, 35.0f);
    mPhysicsBodyTypeDropdown->SetWidth(250.0f);
    mPhysicsBodyTypeDropdown->SetHeaderHeight(35.0f);
    mPhysicsBodyTypeDropdown->SetItemList({"Static", "Dynamic", "Kinematic"});
    mPhysicsBodyTypeDropdown->mOnSelectedItemChangedCallbackSystem.Register([this](size_t index)
                                                                            { SetPhysicsBodyTypeByIndex(index); });

    mPhysicsPanel->AddItem(mPhysicsBodyTypeDropdown);

    //   mPhysicsBodyTypeDropdown->Open();

    mPhysicsGravityPanel = mPhysicsPanel->CreateChildUIElement<UIBoolPanel>("PhysicsGravityPanel");
    mPhysicsGravityPanel->SetTagText("Use Gravity");
    mPhysicsGravityPanel->SetWidth(250.0f);
    //  mPhysicsGravityPanel->SetColor(0.4f, 0.4f, 0.4f);
    mPhysicsGravityPanel->mOnValueChanged.Register(
        [this](bool value)
        {
            if (mDestMeshComponent == nullptr)
                return;

            mDestMeshComponent->SetPhysicsGravityEnabled(value);
            Quad::CommitInspectorEdit(mDestMeshComponent);
        });

    mPhysicsPanel->AddItem(mPhysicsGravityPanel);

    mPhysicsMassPanel = mPhysicsPanel->CreateChildUIElement<UIReflectFloatPanel>("PhysicsMassPanel");
    mPhysicsMassPanel->SetTagText("Mass");
    mPhysicsMassPanel->BindFloat(
        [this]()
        {
            if (mDestMeshComponent == nullptr)
                return 1.0f;

            return mDestMeshComponent->GetPhysicsMass();
        },
        [this](float value)
        {
            if (mDestMeshComponent == nullptr)
                return;

            if (value <= 0.0f)
                value = 0.001f;

            mDestMeshComponent->SetPhysicsMass(value);
        });
    mPhysicsMassPanel->SetCommitNotifier(
        [this]()
        {
            if (mDestMeshComponent)
                Quad::CommitInspectorEdit(mDestMeshComponent);
        });

    mPhysicsPanel->AddItem(mPhysicsMassPanel);
}

void StaticMeshComponentUIReflectPanel::BindProperty(void *targetMemory, Quad::PropertyInfo *property) {}

void StaticMeshComponentUIReflectPanel::Release() {}

UI::UIImage *StaticMeshComponentUIReflectPanel::CreateSubMaterialPanel()
{

    auto subMatPanel = mMaterialPanel->CreateChildUIElement<UI::UIImage>("SubMatPanel");
    subMatPanel->SetStyleRole(UI::EUIStyleRole::ePanel);
    subMatPanel->SetWidth(600);
    subMatPanel->SetHeight(150.0f);
    // subMatPanel->SetColor(0.4f, 0.4f, 0.4f);

    auto imagePanel = subMatPanel->CreateChildUIElement<UI::UIImage>("SubMatImagePanel");
    imagePanel->SetSize(100, 100);
    imagePanel->SetPositionLocal({10, 30});

    UIDropTargetComponent *dropTargetCom = imagePanel->CreateUIComponent<UIDropTargetComponent>("DropTargetComponent");

    int index = mSubMaterialPanelList.size();

    dropTargetCom->mOnDroppedPayloadCallbackSystem.Register([this, index](const DragPayload &payload)
                                                            { DropMaterialPayload(payload, index); });

    // imagePanel. dragTargetCom .   callback

    mSubMaterialPanelList.push_back(subMatPanel);

    auto tagPanel = subMatPanel->CreateChildUIElement<UI::UIText>("SubMatTagPanel");
    tagPanel->SetText("머터리얼");
    tagPanel->SetPositionLocal(10, 0);
    //  tagPanel->SetFontSize(20.0f);
    auto namePanel = subMatPanel->CreateChildUIElement<UI::UIText>("SubMatNamePanel");
    // namePanel->SetFontSize(20.0f);
    namePanel->SetPositionLocal(100, 0);
    // namePanel->SetTextColor({1, 1, 1});
    return nullptr;
}

void StaticMeshComponentUIReflectPanel::DropMaterialPayload(const DragPayload &payload, int index)
{

    if (payload.mType != EDragDropType::eAssetMaterial)
    {
        return;
    }

    //  auto assetManager = CoreAsset::AssetManager::GetInstance();
    mDestMeshComponent->SetSubMeshMaterial(index, payload.mAssetID);
    Quad::CommitInspectorEdit(mDestMeshComponent);

    return;
}

void StaticMeshComponentUIReflectPanel::SetMesh(CoreAsset::AssetID id)
{

    //   CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::StaticMesh>(id).Get();
    if (mDestMeshComponent)
    {
        mDestMeshComponent->SetMesh(id);
        mDestMeshComponent->UpdateRenderProxy();
    }

    Quad::CommitInspectorEdit(mDestMeshComponent);
    BuildComponentMesh();
}

void StaticMeshComponentUIReflectPanel::BuildComponentMesh()
{
    if (mDestMeshComponent == nullptr || mMeshText == nullptr)
        return;

    if (mDestMeshComponent->GetStaticMesh())
        mMeshText->SetText(mDestMeshComponent->GetStaticMesh()->GetName().c_str());

    const std::vector<CoreAsset::AssetPtr> &subMatVec = mDestMeshComponent->GetSubMeshMaterialList();

    if (mSubMaterialPanelList.size() >= subMatVec.size())
    {
        // 기존의 panel로도 충분하다.
        // 넘치는 panel은 비활성화
        for (int i = 0; i < subMatVec.size(); ++i)
        {
            mSubMaterialPanelList[i]->SetActiveFlag(true);
            SetSubMaterialPanelNameText(mSubMaterialPanelList[i], subMatVec[i].Get()->GetName().c_str());
        }

        for (int i = subMatVec.size(); i < mSubMaterialPanelList.size(); ++i)
        {
            mSubMaterialPanelList[i]->SetActiveFlag(false);
        }
    }
    else
    {
        size_t m = subMatVec.size() - mSubMaterialPanelList.size();

        for (int i = 0; i < m; ++i)
        {
            CreateSubMaterialPanel();
        }

        for (int i = 0; i < subMatVec.size(); ++i)
        {
            mSubMaterialPanelList[i]->SetActiveFlag(true);
            SetSubMaterialPanelNameText(mSubMaterialPanelList[i], subMatVec[i].Get()->GetName().c_str());
        }
    }

    for (const auto &matPtr : mDestMeshComponent->GetSubMeshMaterialList())
    {
        CoreAsset::Material *mat = static_cast<CoreAsset::Material *>(matPtr.Get());
    }
}
void StaticMeshComponentUIReflectPanel::SetSubMaterialPanelNameText(UI::UIImage *subMaterialPanel,
                                                                    const std::string &name)
{
    if (subMaterialPanel == nullptr)
        return;

    auto &childVec = subMaterialPanel->GetChildVector();
    auto it = std::find_if(childVec.begin(), childVec.end(),
                           [](UI::UIElement *child)
                           {
                               if (child && child->GetName() == "SubMatNamePanel")
                                   return true;
                               return false;
                           });

    if (it == childVec.end())
        return;

    UI::UIText *textPanel = static_cast<UI::UIText *>(*it);
    textPanel->SetText(name);
}

void StaticMeshComponentUIReflectPanel::RefreshPhysicsProperties()
{
    if (mDestMeshComponent == nullptr)
        return;

    if (mPhysicsEnabledPanel)
        mPhysicsEnabledPanel->SetCheckValue(mDestMeshComponent->IsPhysicsEnabled(), false);

    if (mPhysicsBodyTypeDropdown)
        mPhysicsBodyTypeDropdown->SetSelectedIndex(GetPhysicsBodyTypeIndex(), false);

    if (mPhysicsGravityPanel)
        mPhysicsGravityPanel->SetCheckValue(mDestMeshComponent->IsPhysicsGravityEnabled(), false);

    if (mPhysicsMassPanel)
        mPhysicsMassPanel->RefreshFromSource();
}

size_t StaticMeshComponentUIReflectPanel::GetPhysicsBodyTypeIndex() const
{
    if (mDestMeshComponent == nullptr)
        return 0;

    switch (mDestMeshComponent->GetPhysicsBodyType())
    {
    case EPhysicsBodyType::eStatic:
        return 0;
    case EPhysicsBodyType::eDynamic:
        return 1;
    case EPhysicsBodyType::eKinematic:
        return 2;
    default:
        return 0;
    }
}

void StaticMeshComponentUIReflectPanel::SetPhysicsBodyTypeByIndex(size_t index)
{
    if (mDestMeshComponent == nullptr)
        return;

    EPhysicsBodyType bodyType = EPhysicsBodyType::eStatic;
    switch (index)
    {
    case 0:
        bodyType = EPhysicsBodyType::eStatic;
        break;
    case 1:
        bodyType = EPhysicsBodyType::eDynamic;
        break;
    case 2:
        bodyType = EPhysicsBodyType::eKinematic;
        break;
    default:
        return;
    }

    mDestMeshComponent->SetPhysicsBodyType(bodyType);
    Quad::CommitInspectorEdit(mDestMeshComponent);
}

void StaticMeshComponentUIReflectPanel::OnTransformChanged(UI::ETransformChangeType type)
{

    UIImage::OnTransformChanged(type);

    if (type == UI::ETransformChangeType::eSize)
    {
        float width = GetWidth();

        if (mMeshFoldPanel)
            mMeshFoldPanel->SetWidth(width);

        if (mPhysicsPanel)
            mPhysicsPanel->SetWidth(width);

        /* if (mMaterialPanel)
             mMaterialPanel->SetWidth(width);*/
    }
}

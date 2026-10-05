#include "BoxColliderComponentUIReflectPanel.h"
#include <EditorDirector/EditorUIUtility.h>

#include <Core/BoxColliderComponent.h>
#include <Core/CollisionChannelSystem.h>
#include <EditorDirector/EditorInspectorUtility.h>
#include <EditorDirector/UIBoolPanel.h>
#include <EditorDirector/UIDropdown.h>
#include <EditorDirector/UIFoldoutPanel.h>
#include <EditorDirector/UIReflectFloatPanel.h>
#include <EditorDirector/UIReflectVector3Panel.h>
#include <UiSystem/UIVerticalLayoutComponent.h>

BoxColliderComponentUIReflectPanel::BoxColliderComponentUIReflectPanel()
{
    SetStyleRole(UI::EUIStyleRole::ePanel);
    CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
}

BoxColliderComponentUIReflectPanel::~BoxColliderComponentUIReflectPanel() = default;

void BoxColliderComponentUIReflectPanel::Build(Component *component)
{
    // 재사용 중인 입력 상태와 이전 대상의 바인딩을 먼저 정리한다.
    Release();
    mDestColliderComponent = dynamic_cast<BoxColliderComponent *>(component);
    BindFields();
}

void BoxColliderComponentUIReflectPanel::BindProperty(void *, Quad::PropertyInfo *) {}

void BoxColliderComponentUIReflectPanel::Release()
{
    // pool에 있는 동안 지연된 입력 완료 콜백이 이전 컴포넌트를 수정하지 않게 한다.
    mDestColliderComponent = nullptr;
    if (mBoxCenterPanel)
    {
        mBoxCenterPanel->Unbind();
        mBoxCenterPanel->ClearDisplay();
    }
    if (mBoxHalfExtentPanel)
    {
        mBoxHalfExtentPanel->Unbind();
        mBoxHalfExtentPanel->ClearDisplay();
    }
    if (mPhysicsMassPanel)
    {
        mPhysicsMassPanel->Unbind();
        mPhysicsMassPanel->ClearDisplay();
    }
    if (mPhysicsEnabledPanel)
        mPhysicsEnabledPanel->SetCheckValue(false, false);
    if (mPhysicsGravityPanel)
        mPhysicsGravityPanel->SetCheckValue(false, false);
    if (mPhysicsBodyTypeDropdown)
    {
        mPhysicsBodyTypeDropdown->Close();
        mPhysicsBodyTypeDropdown->SetSelectedIndex(0, false);
    }
}

void BoxColliderComponentUIReflectPanel::OnBegin()
{
    UI::UIImage::OnBegin();

    // 자식 컨트롤 내부 UI가 준비되는 기존 OnBegin 생성 경로에서만 구성한다.
    mBoxPanel = EditorUIUtility::CreateFoldoutPanel(this, "BoxShapePanel");
    mBoxPanel->SetHeaderText("Box Collider (Local Shape)");
    mBoxPanel->SetWidth(GetWidth());
    mBoxPanel->SetExpanded(true);

    mBoxPanel->SetHeaderColor(UI::UIColor::DarkYellow);

    mBoxCenterPanel = EditorUIUtility::CreateVector3Field(mBoxPanel, "BoxCenterPanel");
    mBoxCenterPanel->SetTagText("Center");
    // Vector3 패널은 이전 값과 같으면 표시 갱신을 생략하므로 기본 중심 (0,0,0)도 먼저 표시한다.
    mBoxCenterPanel->ClearDisplay();
    mBoxPanel->AddItem(mBoxCenterPanel);

    mBoxHalfExtentPanel = EditorUIUtility::CreateVector3Field(mBoxPanel, "BoxHalfExtentPanel");
    mBoxHalfExtentPanel->SetTagText("Half Extent");
    mBoxHalfExtentPanel->ClearDisplay();
    mBoxPanel->AddItem(mBoxHalfExtentPanel);

    mPhysicsPanel = EditorUIUtility::CreateFoldoutPanel(this, "PhysicsPanel");
    mPhysicsPanel->SetHeaderText("Physics");
    mPhysicsPanel->SetWidth(GetWidth());
    mPhysicsPanel->SetExpanded(true);

    mPhysicsPanel->SetHeaderColor(UI::UIColor::DarkYellow);

    mPhysicsEnabledPanel = EditorUIUtility::CreateBoolField(mPhysicsPanel, "PhysicsEnabledPanel");
    mPhysicsEnabledPanel->SetTagText("Enable Physics");
    mPhysicsEnabledPanel->mOnValueChanged.Register(
        [this](bool value)
        {
            if (!mDestColliderComponent)
                return;
            mDestColliderComponent->SetPhysicsEnabled(value);
            CommitEdit();
        });
    mPhysicsPanel->AddItem(mPhysicsEnabledPanel);

    mPhysicsBodyTypeDropdown = EditorUIUtility::CreateDropdown(mPhysicsPanel, "PhysicsBodyTypeDropdown");
    mPhysicsBodyTypeDropdown->SetWidth(250.0f);
    mPhysicsBodyTypeDropdown->SetHeaderHeight(35.0f);
    mPhysicsBodyTypeDropdown->SetItemList({"Static", "Dynamic", "Kinematic"});
    mPhysicsBodyTypeDropdown->mOnSelectedItemChangedCallbackSystem.Register([this](size_t index)
                                                                            { SetPhysicsBodyTypeByIndex(index); });
    mPhysicsPanel->AddItem(mPhysicsBodyTypeDropdown);

    mPhysicsGravityPanel = EditorUIUtility::CreateBoolField(mPhysicsPanel, "PhysicsGravityPanel");
    mPhysicsGravityPanel->SetTagText("Use Gravity");
    mPhysicsGravityPanel->mOnValueChanged.Register(
        [this](bool value)
        {
            if (!mDestColliderComponent)
                return;
            mDestColliderComponent->SetPhysicsGravityEnabled(value);
            CommitEdit();
        });
    mPhysicsPanel->AddItem(mPhysicsGravityPanel);

    mPhysicsMassPanel = EditorUIUtility::CreateFloatField(mPhysicsPanel, "PhysicsMassPanel");
    mPhysicsMassPanel->SetTagText("Mass");
    mPhysicsPanel->AddItem(mPhysicsMassPanel);

    mCollisionChannelDropdown = EditorUIUtility::CreateDropdown(mPhysicsPanel, "CollisionChannelDropdown");
    mCollisionChannelDropdown->SetWidth(250.0f);

    Core::CollisionChannelSystem *collisionChannelSystem = Core::CollisionChannelSystem::GetInstance();
    mCollisionChannelDropdown->SetItemList(collisionChannelSystem->GetAllChannelName());
    mPhysicsPanel->AddItem(mCollisionChannelDropdown);

    collisionChannelSystem->mOnChannelListChanged.Register(
        [this]()
        {
            if (mCollisionChannelDropdown)
            {
                Core::CollisionChannelSystem *collisionChannelSystem = Core::CollisionChannelSystem::GetInstance();
                mCollisionChannelDropdown->SetItemList(collisionChannelSystem->GetAllChannelName());
                OnChangedCollisionChannelID();
            }
        });

    mCollisionChannelDropdown->mOnSelectedItemChangedCallbackSystem.Register([this](size_t index)
                                                                             { OnChangedCollisionChannelID(); });

    // Build가 OnBegin보다 먼저 호출된 경우에도 준비된 컨트롤에 현재 대상을 연결한다.
    BindFields();
}

void BoxColliderComponentUIReflectPanel::BindFields()
{
    if (!mDestColliderComponent || !mPhysicsMassPanel)
        return;

    // 형상 값은 컴포넌트 로컬 공간이다. 월드 스케일은 PhysicsBridgeSystem에서 적용하므로
    // UI에서는 미리 곱하지 않는다. 반크기는 전체 길이의 절반이며 각 축이 양수여야 한다.
    mBoxCenterPanel->BindVector3([this]() { return mDestColliderComponent->GetBoxCenter(); },
                                 [this](const CoreMath::Vector3 &value)
                                 { mDestColliderComponent->SetBoxCenter(value); });
    mBoxCenterPanel->SetCommitNotifier([this]() { CommitEdit(); });
    mBoxHalfExtentPanel->BindVector3([this]() { return mDestColliderComponent->GetBoxHalfExtent(); },
                                     [this](const CoreMath::Vector3 &value)
                                     { mDestColliderComponent->SetBoxHalfExtent(value); });
    mBoxHalfExtentPanel->SetCommitNotifier([this]() { CommitEdit(); });
    mPhysicsMassPanel->BindFloat([this]() { return mDestColliderComponent->GetPhysicsMass(); },
                                 [this](float value) { mDestColliderComponent->SetPhysicsMass(value); });
    mPhysicsMassPanel->SetCommitNotifier([this]() { CommitEdit(); });

    // 표시값 동기화는 사용자 편집이 아니므로 변경 콜백을 실행하지 않는다.
    // 숫자 입력이 setter에서 거부되면 기존 입력 패널이 getter 값으로 다시 표시한다.
    mPhysicsEnabledPanel->SetCheckValue(mDestColliderComponent->IsPhysicsEnabled(), false);
    mPhysicsGravityPanel->SetCheckValue(mDestColliderComponent->IsPhysicsGravityEnabled(), false);
    mPhysicsBodyTypeDropdown->SetSelectedIndex(GetPhysicsBodyTypeIndex(), false);

    Core::CollisionChannelID collisionChannelID = mDestColliderComponent->GetCollisionChannelID();
    Core::CollisionChannelSystem *collisionChannelSystem = Core::CollisionChannelSystem::GetInstance();

    std::string collisionChannelName;
    bool ret = collisionChannelSystem->GetChannelName(collisionChannelID, collisionChannelName);
    if (ret == false)
    {
        // 기본채널 설정 .
        collisionChannelSystem->GetChannelID(collisionChannelName, collisionChannelID);
        mDestColliderComponent->SetCollisionChannelID(collisionChannelID);
    }
    // 해당채널에맞는 index로 curr item  설정
    std::vector<std::string> channelList = collisionChannelSystem->GetAllChannelName();
    size_t index = std::find(channelList.begin(), channelList.end(), collisionChannelName) - channelList.begin();
    mCollisionChannelDropdown->SetItemList(collisionChannelSystem->GetAllChannelName());
    mCollisionChannelDropdown->SetSelectedIndex(index, false);
}

void BoxColliderComponentUIReflectPanel::CommitEdit()
{
    // 기존 Inspector 경로로 물리 설정을 flush하고 소속 map의 저장 dirty를 표시한다.
    if (mDestColliderComponent)
        Quad::CommitInspectorEdit(mDestColliderComponent);
}

size_t BoxColliderComponentUIReflectPanel::GetPhysicsBodyTypeIndex() const
{
    if (!mDestColliderComponent)
        return 0;
    switch (mDestColliderComponent->GetPhysicsBodyType())
    {
    case EPhysicsBodyType::eDynamic:
        return 1;
    case EPhysicsBodyType::eKinematic:
        return 2;
    default:
        return 0;
    }
}

void BoxColliderComponentUIReflectPanel::SetPhysicsBodyTypeByIndex(size_t index)
{
    if (!mDestColliderComponent)
        return;
    EPhysicsBodyType type;
    switch (index)
    {
    case 0:
        type = EPhysicsBodyType::eStatic;
        break;
    case 1:
        type = EPhysicsBodyType::eDynamic;
        break;
    case 2:
        type = EPhysicsBodyType::eKinematic;
        break;
    default:
        return;
    }
    mDestColliderComponent->SetPhysicsBodyType(type);
    CommitEdit();
}

void BoxColliderComponentUIReflectPanel::OnTransformChanged(UI::ETransformChangeType type)
{
    UI::UIImage::OnTransformChanged(type);
    if (type == UI::ETransformChangeType::eAll || type == UI::ETransformChangeType::eSize)
    {
        if (mBoxPanel)
            mBoxPanel->SetWidth(GetWidth());
        if (mPhysicsPanel)
            mPhysicsPanel->SetWidth(GetWidth());
    }
}

void BoxColliderComponentUIReflectPanel::OnChangedCollisionChannelID()
{

    if (mDestColliderComponent == nullptr || mCollisionChannelDropdown == nullptr)
        return;

    std::string channelName;
    mCollisionChannelDropdown->GetSelectedText(channelName);

    auto collisionChannelSystem = Core::CollisionChannelSystem::GetInstance();

    Core::CollisionChannelID channelID;

    bool ret = collisionChannelSystem->GetChannelID(channelName, channelID);

    // if (ret == false)
    //     channelID = Core::DefaultCollisionChannelID;

    mDestColliderComponent->SetCollisionChannelID(channelID);
}

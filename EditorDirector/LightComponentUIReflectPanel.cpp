#include "LightComponentUIReflectPanel.h"
#include <EditorDirector/EditorUIUtility.h>

#include <Core/LightComponent.h>
#include <EditorDirector/EditorInspectorUtility.h>
#include <EditorDirector/UIBoolPanel.h>
#include <EditorDirector/UIDropdown.h>
#include <EditorDirector/UIFoldoutPanel.h>
#include <EditorDirector/UIReflectFloatPanel.h>
#include <EditorDirector/UIReflectVector3Panel.h>
#include <UiSystem/UIVerticalLayoutComponent.h>

LightComponentUIReflectPanel::LightComponentUIReflectPanel()
{
    SetStyleRole(UI::EUIStyleRole::ePanel);
    CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
}

LightComponentUIReflectPanel::~LightComponentUIReflectPanel() = default;

void LightComponentUIReflectPanel::Build(Component *component)
{
    // 재사용 시 이전 대상의 입력 상태와 바인딩부터 해제한다.
    Release();
    mDestLightComponent = dynamic_cast<LightComponent *>(component);
    BindFields();
}

void LightComponentUIReflectPanel::BindProperty(void *, Quad::PropertyInfo *) {}

void LightComponentUIReflectPanel::Release()
{
    // pool에 반환된 뒤 지연된 UI 콜백이 이전 조명을 변경하지 않게 한다.
    mDestLightComponent = nullptr;
    if (mStrengthPanel)
    {
        mStrengthPanel->Unbind();
        mStrengthPanel->ClearDisplay();
    }
    if (mFalloffStartPanel)
    {
        mFalloffStartPanel->Unbind();
        mFalloffStartPanel->ClearDisplay();
    }
    if (mFalloffEndPanel)
    {
        mFalloffEndPanel->Unbind();
        mFalloffEndPanel->ClearDisplay();
    }
    if (mSpotPowerPanel)
    {
        mSpotPowerPanel->Unbind();
        mSpotPowerPanel->ClearDisplay();
    }
    if (mLightEnabledPanel)
        mLightEnabledPanel->SetCheckValue(false, false);
    if (mLightTypeDropdown)
    {
        mLightTypeDropdown->Close();
        mLightTypeDropdown->SetSelectedIndex(0, false);
    }
    mDisplayedLightTypeIndex = 0;
}

void LightComponentUIReflectPanel::OnBegin()
{
    UI::UIImage::OnBegin();

    mLightPanel = EditorUIUtility::CreateFoldoutPanel(this, "LightPanel");
    mLightPanel->SetHeaderText("Light");
    mLightPanel->SetWidth(GetWidth());
    mLightPanel->SetExpanded(true);

    mLightPanel->SetHeaderColor(UI::UIColor::DarkYellow);

    mLightEnabledPanel = EditorUIUtility::CreateBoolField(mLightPanel, "LightEnabledPanel");
    mLightEnabledPanel->SetTagText("Enabled (Runtime)");
    mLightEnabledPanel->mOnValueChanged.Register(
        [this](bool value)
        {
            if (!mDestLightComponent)
                return;
            // 이 상태는 현재 직렬화 대상이 아니므로 prefab 저장 dirty를 표시하지 않는다.
            mDestLightComponent->SetLightEnabled(value);
            mDestLightComponent->FlushPropertyDirty();
        });
    mLightPanel->AddItem(mLightEnabledPanel);

    mLightTypeDropdown = EditorUIUtility::CreateDropdown(mLightPanel, "LightTypeDropdown");
    mLightTypeDropdown->SetWidth(250.0f);
    mLightTypeDropdown->SetHeaderHeight(35.0f);
    mLightTypeDropdown->SetItemList({"Directional", "Point", "Spot"});
    mLightTypeDropdown->mOnSelectedItemChangedCallbackSystem.Register(
        [this](size_t index) { SetLightTypeByIndex(index); });
    mLightPanel->AddItem(mLightTypeDropdown);

    mStrengthPanel = EditorUIUtility::CreateVector3Field(mLightPanel, "StrengthPanel");
    mStrengthPanel->SetTagText("Strength");
    mStrengthPanel->ClearDisplay();
    mLightPanel->AddItem(mStrengthPanel);

    mFalloffStartPanel = EditorUIUtility::CreateFloatField(mLightPanel, "FalloffStartPanel");
    mFalloffStartPanel->SetTagText("Falloff Start");
    mLightPanel->AddItem(mFalloffStartPanel);

    mFalloffEndPanel = EditorUIUtility::CreateFloatField(mLightPanel, "FalloffEndPanel");
    mFalloffEndPanel->SetTagText("Falloff End");
    mLightPanel->AddItem(mFalloffEndPanel);

    mSpotPowerPanel = EditorUIUtility::CreateFloatField(mLightPanel, "SpotPowerPanel");
    mSpotPowerPanel->SetTagText("Spot Power");
    mLightPanel->AddItem(mSpotPowerPanel);

    // Build가 먼저 호출되더라도 자식 컨트롤 생성이 끝난 시점에 바인딩을 완료한다.
    BindFields();
}

void LightComponentUIReflectPanel::BindFields()
{
    if (!mDestLightComponent || !mSpotPowerPanel)
        return;

    mStrengthPanel->BindVector3(
        [this]() { return mDestLightComponent->GetStrength(); },
        [this](const CoreMath::Vector3 &value) { mDestLightComponent->SetStrength(value); });
    mStrengthPanel->SetCommitNotifier([this]() { CommitEdit(); });
    mFalloffStartPanel->BindFloat(
        [this]() { return mDestLightComponent->GetFalloffStart(); },
        [this](float value) { mDestLightComponent->SetFalloffStart(value); });
    mFalloffStartPanel->SetCommitNotifier([this]() { CommitEdit(); });
    mFalloffEndPanel->BindFloat(
        [this]() { return mDestLightComponent->GetFalloffEnd(); },
        [this](float value) { mDestLightComponent->SetFalloffEnd(value); });
    mFalloffEndPanel->SetCommitNotifier([this]() { CommitEdit(); });
    mSpotPowerPanel->BindFloat(
        [this]() { return mDestLightComponent->GetSpotPower(); },
        [this](float value) { mDestLightComponent->SetSpotPower(value); });
    mSpotPowerPanel->SetCommitNotifier([this]() { CommitEdit(); });

    // 화면 초기화는 편집이 아니므로 setter/저장 dirty 콜백을 실행하지 않는다.
    mLightEnabledPanel->SetCheckValue(mDestLightComponent->GetLightEnabled(), false);
    mDisplayedLightTypeIndex = GetLightTypeIndex();
    mLightTypeDropdown->SetSelectedIndex(mDisplayedLightTypeIndex, false);
    UpdateTypeVisibility();
}

void LightComponentUIReflectPanel::CommitEdit()
{
    // 기존 Inspector 경로로 프록시 변경을 flush하고 소속 map의 저장 dirty를 표시한다.
    if (mDestLightComponent)
        Quad::CommitInspectorEdit(mDestLightComponent);
}

size_t LightComponentUIReflectPanel::GetLightTypeIndex() const
{
    if (!mDestLightComponent)
        return 0;
    switch (mDestLightComponent->GetLightType())
    {
    case Core::ELightType::ePoint:
        return 1;
    case Core::ELightType::eSpot:
        return 2;
    default:
        return 0;
    }
}

void LightComponentUIReflectPanel::SetLightTypeByIndex(size_t index)
{
    if (!mDestLightComponent)
        return;

    // UI 목록 순서를 enum의 저장 값과 암묵적으로 결합하지 않는다.
    Core::ELightType type;
    switch (index)
    {
    case 0:
        type = Core::ELightType::eDirectional;
        break;
    case 1:
        type = Core::ELightType::ePoint;
        break;
    case 2:
        type = Core::ELightType::eSpot;
        break;
    default:
        return;
    }
    mDestLightComponent->SetLightType(type);
    mDisplayedLightTypeIndex = index;
    UpdateTypeVisibility();
    CommitEdit();
}

void LightComponentUIReflectPanel::UpdateTypeVisibility()
{
    if (!mDestLightComponent || !mSpotPowerPanel)
        return;

    // 값을 삭제하지 않고 표시/입력 대상에서만 제외한다. 타입을 되돌리면 기존 값을 유지한다.
    // 활성 상태 변경은 기존 VerticalLayout이 감지해 foldout 높이를 다시 계산한다.
    const Core::ELightType type = mDestLightComponent->GetLightType();
    const bool usesFalloff = type == Core::ELightType::ePoint || type == Core::ELightType::eSpot;
    mFalloffStartPanel->SetActiveFlag(usesFalloff);
    mFalloffEndPanel->SetActiveFlag(usesFalloff);
    mSpotPowerPanel->SetActiveFlag(type == Core::ELightType::eSpot);
}

void LightComponentUIReflectPanel::Update(float deltaTime)
{
    UI::UIImage::Update(deltaTime);
    if (!mDestLightComponent || !mSpotPowerPanel)
        return;

    // 게임 코드가 조명을 토글해도 UI 값만 동기화하고 편집 콜백은 호출하지 않는다.
    mLightEnabledPanel->SetCheckValue(mDestLightComponent->GetLightEnabled(), false);
    const size_t typeIndex = GetLightTypeIndex();
    if (typeIndex != mDisplayedLightTypeIndex)
    {
        // SetSelectedIndex는 펼쳐진 목록도 닫으므로 실제 타입 변경 시에만 호출한다.
        mDisplayedLightTypeIndex = typeIndex;
        mLightTypeDropdown->SetSelectedIndex(typeIndex, false);
        UpdateTypeVisibility();
    }
}

void LightComponentUIReflectPanel::OnTransformChanged(UI::ETransformChangeType type)
{
    UI::UIImage::OnTransformChanged(type);
    if (type == UI::ETransformChangeType::eAll || type == UI::ETransformChangeType::eSize)
    {
        if (mLightPanel)
            mLightPanel->SetWidth(GetWidth());
    }
}

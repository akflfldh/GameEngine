#include "UIEditorDebugHUD.h"
#include <CoreAsset/GlobalAssetRegistrySystem.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIMovableComponent.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UIVerticalLayoutComponent.h>
UIEditorDebugHUD::UIEditorDebugHUD()
{

    mBackgroundImageComponent = CreateUIComponent<UI::UIImageComponent>("BackgroundImageCom");
    mBackgroundImageComponent->SetColor(0.2, 0.2, 0.2);
    // mVerticalLayoutComponent = CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");

    mTextComponent = CreateUIComponent<UI::UITextComponent>("TextCom");
    mTextComponent->SetFontSize(20.0f);

    mAddedTextComponent = CreateUIComponent<UI::UITextComponent>("AddedTextCom");
    mAddedTextComponent->SetFontSize(20.0f);

    CreateUIComponent<UI::UIMovableComponent>("MovableCom");
    SetSize(400, 700);
}

UIEditorDebugHUD::~UIEditorDebugHUD() {}

void UIEditorDebugHUD::Update(float deltaTime)
{

    UIElement::Update(deltaTime);

    mUpdateTime += deltaTime;
    if (mUpdateTime > 0.5f)
    {
        // 데이터 갱신 출력
        std::string text;

        auto globalAssetRegistrySystem = CoreAsset::GlobalAssetRegistrySystem::GetInstance();

        size_t assetNum = globalAssetRegistrySystem->GetAssetNum();

        size_t dirtyAssetNum = globalAssetRegistrySystem->GetDirtyAssetList().size();

        text += "더티 에셋 수 : " + std::to_string(dirtyAssetNum) + "\n";
        text += "총 에셋 수 : " + std::to_string(assetNum);

        mTextComponent->SetText(text);
        mUpdateTime = 0.0f;

        mAddedTextComponent->SetPaddingTop(mTextComponent->GetActualBottom());
    }
}

void UIEditorDebugHUD::OnBegin()
{

    // Editor에관한정보들을 가져온다 .
}

void UIEditorDebugHUD::AddText(const std::string &str, int lineIndex)
{
    if (mAddedTextList.size() <= lineIndex)
        mAddedTextList.push_back(str);
    else
        mAddedTextList[lineIndex] = str;

    std::string textAll;
    for (const auto &text : mAddedTextList)
    {

        textAll += text;
        textAll += "\n";
    }

    mAddedTextComponent->SetText(textAll);
}

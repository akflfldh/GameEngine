#include "UIReflectSinglePrimitivePanel.h"

#include "UIReflectVector3Panel.h"
#include <EditorInspectorUtility.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIText.h>
UIReflectSinglePrimitivePanel::UIReflectSinglePrimitivePanel()
    : mTagText(nullptr), mEditBox(nullptr), mTargetMemory(nullptr), mPropertyInfo(nullptr)
{
}

UIReflectSinglePrimitivePanel::~UIReflectSinglePrimitivePanel() {}

void UIReflectSinglePrimitivePanel::OnBegin()
{
    UI::UIImage::OnBegin();
    SetHeight(80);

    SetColor({0.4f, 0.4f, 0.4f});

    mTagText = CreateChildUIElement<UI::UIText>("TagText");
    mTagText->SetTextColor({0, 0, 0});
    mTagText->SetPositionLocal({20, 20});
    mEditBox = CreateChildUIElement<UI::UIEditBox>("EditBox");

    mEditBox->SetSize(300, 40);
    mEditBox->SetTextColor({0, 0, 0});

    mEditBox->SetBackgroundColor(1.0f, 1.0f, 1.0f);

    mEditBox->mOnFinishInputCallbackSystem.Register(
        [this](const std::string &str)
        {
            if (str.empty())
                return;

            if (mTypeSpecificSetter)
            {
                mTypeSpecificSetter(str);
                BaseClass *baseClass = reinterpret_cast<BaseClass *>(mTargetMemory);
                Quad::CommitInspectorEdit(baseClass);
            }
        });
}

void UIReflectSinglePrimitivePanel::Update(float deltaTime)
{

    UI::UIImage::Update(deltaTime);
}

void UIReflectSinglePrimitivePanel::SetTagText(const std::string &tag)
{

    if (mTagText)
    {
        mTagText->SetText(tag);
        int tagBoxWidth = tag.size() * 10;
        mTagText->SetSize(tagBoxWidth, 40);
        mEditBox->SetPositionLocal({tagBoxWidth + 30 + mTagText->mTransform.GetLocalPosition().x, 20});
    }
}

void UIReflectSinglePrimitivePanel::SetTextFilterMode(UI::EUITextInputType type)
{

    if (mEditBox)
    {
        mEditBox->SetTextInputType(type);
    }
}

void UIReflectSinglePrimitivePanel::BindProperty(void *targetMemory, Quad::PropertyInfo *property)
{

    int numType = 0;
    mTargetMemory = targetMemory;
    mPropertyInfo = property;

    if ((strcmp(property->mType, "float") == 0))
    {

        mTypeSpecificSetter = [this](const std::string &str)
        {
            if (str.empty())
                return;

            float value = std::stof(str);
            mPropertyInfo->SetValue<float>(mTargetMemory, value);
        };
        numType = 1;
    }
    else if (strcmp(property->mType, "double") == 0)
    {
        mTypeSpecificSetter = [this](const std::string &str)
        {
            double value = std::stod(str);
            mPropertyInfo->SetValue<double>(mTargetMemory, value);
        };
        numType = 1;
    }
    else if (strcmp(property->mType, "int") == 0)
    {
        mTypeSpecificSetter = [this](const std::string &str)
        {
            int value = std::stoi(str);
            mPropertyInfo->SetValue<int>(mTargetMemory, value);
        };
    }
    else if ((strcmp(property->mType, "short") == 0))
    {
        mTypeSpecificSetter = [this](const std::string &str)
        {
            short value = std::stoi(str);
            mPropertyInfo->SetValue<short>(mTargetMemory, value);
        };
    }
    else if ((strcmp(property->mType, "long") == 0))
    {
        mTypeSpecificSetter = [this](const std::string &str)
        {
            long value = std::stol(str);
            mPropertyInfo->SetValue<long>(mTargetMemory, value);
        };
    }
    else if ((strcmp(property->mType, "long long") == 0))
    {
        mTypeSpecificSetter = [this](const std::string &str)
        {
            long long value = std::stol(str);
            mPropertyInfo->SetValue<long long>(mTargetMemory, value);
        };
    }
    else if ((strcmp(property->mType, "unsigned int") == 0))
    {
        mTypeSpecificSetter = [this](const std::string &str)
        {
            unsigned int value = std::stoul(str);
            mPropertyInfo->SetValue<unsigned int>(mTargetMemory, value);
        };
    }
    else if ((strcmp(property->mType, "unsigned short") == 0))
    {
        mTypeSpecificSetter = [this](const std::string &str)
        {
            unsigned short value = std::stoul(str);
            mPropertyInfo->SetValue<unsigned short>(mTargetMemory, value);
        };
    }
    else if ((strcmp(property->mType, "unsigned long") == 0))
    {
        mTypeSpecificSetter = [this](const std::string &str)
        {
            unsigned long value = std::stoul(str);
            mPropertyInfo->SetValue<unsigned long>(mTargetMemory, value);
        };
    }
    else if ((strcmp(property->mType, "unsigned long long") == 0))
    {
        mTypeSpecificSetter = [this](const std::string &str)
        {
            unsigned long long value = std::stoul(str);
            mPropertyInfo->SetValue<unsigned long long>(mTargetMemory, value);
        };
    }
    if (numType == 1)
    {
        SetTextFilterMode(UI::EUITextInputType::eNumber);
    }
    else if (numType == 0)
    {
        SetTextFilterMode(UI::EUITextInputType::eInteger);
    }
}

void UIReflectSinglePrimitivePanel::Release() {}

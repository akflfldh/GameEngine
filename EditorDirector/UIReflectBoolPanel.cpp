#include "UIReflectBoolPanel.h"
#include "CoreBase/BaseClass.h"
#include "IPropertyBindable.h"
#include <EditorInspectorUtility.h>

UIReflectBoolPanel::UIReflectBoolPanel() : mTargetMemory(nullptr), mPropertyInfo(nullptr) {}

UIReflectBoolPanel::~UIReflectBoolPanel() {}

void UIReflectBoolPanel::OnBegin()
{

    UIBoolPanel::OnBegin();
    mOnValueChanged.Register(
        [this](bool value)
        {
            if (mPropertyInfo)
            {
                mPropertyInfo->SetValue<bool>(mTargetMemory, value);
                BaseClass *baseClass = reinterpret_cast<BaseClass *>(mTargetMemory);
                Quad::CommitInspectorEdit(baseClass);
            }
        });
}

void UIReflectBoolPanel::BindProperty(void *targetMemory, Quad::PropertyInfo *propertyInfo)
{

    SetTagText(propertyInfo->mPropertyName);

    SetCheckValue(propertyInfo->GetValue<bool>(targetMemory));

    mTargetMemory = targetMemory;
    mPropertyInfo = propertyInfo;
}

void UIReflectBoolPanel::Release() {}

void UIReflectBoolPanel::ClearBindCallback()
{

    mOnValueChanged.UnRegisterAll();
}

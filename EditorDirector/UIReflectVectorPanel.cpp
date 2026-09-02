#include "UIReflectVectorPanel.h"
#include <EditorDirector/IPropertyBindable.h>
#include <EditorDirector/UIReflectPanelFactory.h>
#include <ReflectSystem/ReflectionPropertyInfo.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIImage.h>
UIReflectVectorPanel::UIReflectVectorPanel() : mTargetMemory(nullptr) {}

UIReflectVectorPanel::~UIReflectVectorPanel() {}

void UIReflectVectorPanel::OnBegin()
{

    UIFoldoutPanel::OnBegin();
}

void UIReflectVectorPanel::Update(float deltaTime)
{

    UIFoldoutPanel::Update(deltaTime);
}

void UIReflectVectorPanel::BindProperty(void *targetMemory, Quad::PropertyInfo *property)
{
    auto reflectPanelFactory = UIReflectPanelFactory::GetInstance();
    mTargetMemory = (char *)targetMemory + property->mOffset;

    ITypeDescriptor_Vector *ITypeDescriptorVector = static_cast<ITypeDescriptor_Vector *>(property->mTypeDescriptor);

    size_t size = ITypeDescriptorVector->GetSize(mTargetMemory);

    for (size_t i = 0; i < size; ++i)
    {

        // element 에대해서 동작
        void *elementTargetMemory = ITypeDescriptorVector->GetElementPtr(mTargetMemory, i);

        const std::string tagName = "[" + std::to_string(i) + "]";

        Quad::PropertyInfo *dummyPropertyInfo = new Quad::PropertyInfo;
        dummyPropertyInfo->mType = property->mTemplateElementTypeName;
        dummyPropertyInfo->mOffset = 0;
        dummyPropertyInfo->mIsBuiltinType = property->mIsTemplateElementBuiltinType;
        dummyPropertyInfo->mIsTemplateType = false;

        std::vector<UI::UIElement *> childList =
            reflectPanelFactory->GetReflectPanel(elementTargetMemory, mContentPanel, dummyPropertyInfo, tagName);

        if (childList.empty() == false)
        {
            for (auto child : childList)
            {

                child->SetWidth(mTransform.GetSize().r);
                AddItem(child);
                mElementPanelList.push_back(child);
            }
        }
    }
}

void UIReflectVectorPanel::Release()
{
    auto factory = UIReflectPanelFactory::GetInstance();

    for (auto child : mElementPanelList)
    {
        factory->ReleaseReflectPanel(child);
    }

    mElementPanelList.clear();
}

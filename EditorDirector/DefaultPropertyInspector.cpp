#include "DefaultPropertyInspector.h"
#include <DefaultEditorInspectorManager.h>
#include <EditorSelectionManager.h>
#include <PropertyPanel.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIElementPtr.h>

DefaultPropertyInspector *DefaultPropertyInspector::GetInstance()
{
    static DefaultPropertyInspector instance;
    return &instance;
}

DefaultPropertyInspector::DefaultPropertyInspector() {}

DefaultPropertyInspector::~DefaultPropertyInspector() {}

void DefaultPropertyInspector::Initialize(UI::UICanvas *canvas)
{
    mCanvas = canvas;
}

void DefaultPropertyInspector::BeginUI()
{

    if (mCanvas == nullptr)
        return;

    auto editorSelectionManager = EditorSelectionManager::GetInstance();

    mPropertyPanel = mCanvas->CreateUIElement<PropertyPanel>("PropertyPanel");
    mPropertyPanel->Initialize(editorSelectionManager);
    mPropertyPanel->SetSize(700, 800);
    mPropertyPanel->SetPositionLocal(0, 500);
    mPropertyPanel->SetBackgrounColor(0.6f, 0.6f, 0.6f);
    mPropertyPanel->SetLayout(EUIScrollLayout::eVertical);

    editorSelectionManager->mOnSelectedObjectCallbackSystem.Register(
        [this](Object *object)
        {
            DefaultEditorInspectorManager::GetInstance()->ActivateInsepctor(EDefaultEditorInspectorType::eProprety, {});
            mPropertyPanel->OnSelectedObject(object);
        });

    // editorSelectionManager->mOnSelectedComponentCallbackSystem.Register(mPropertyPanel,
    //                                                                     &PropertyPanel::OnSelectedComponent);

    editorSelectionManager->mOnSelectedComponentCallbackSystem.Register(
        [this](Component *com)
        {
            DefaultEditorInspectorManager::GetInstance()->ActivateInsepctor(EDefaultEditorInspectorType::eProprety, {});
            mPropertyPanel->OnSelectedComponent(com);
        });

    DeActivateInspector();
}

void DefaultPropertyInspector::ActivateInspector()
{

    if (mPropertyPanel)
    {
        mPropertyPanel->SetActiveFlag(true);
    }
}

void DefaultPropertyInspector::DeActivateInspector()
{

    if (mPropertyPanel)
    {
        mPropertyPanel->SetActiveFlag(false);
    }
}

void DefaultPropertyInspector::SetInspectorContext(const InspectorContext &inspectorContext) {}

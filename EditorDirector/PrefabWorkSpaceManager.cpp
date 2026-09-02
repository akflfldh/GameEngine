#include "PrefabWorkSpaceManager.h"
#include <Core/LogicalWindow.h>
#include <Core/Map.h>
#include <Core/Prefab.h>
#include <Core/WorkSpace.h>
#include <Core/World.h>
#include <EditorDirector/EditorDirector.h>
#include <EditorDirector/UIScrollBox.h>
#include <EditorMode.h>
#include <EditorProjectManager.h>
#include <EditorSceneController.h>
#include <EditorSceneManager.h>
#include <EditorSelectionManager.h>
#include <GlobalOverlayManager.h>
#include <ReflectSystem/ReflectionClassInfo.h>
#include <ReflectSystem/ReflectionPropertyInfo.h>
#include <ReflectSystem/ReflectionSystem.h>

#include <Core/CameraComponent.h>
#include <Core/Component.h>
#include <Core/Entity.h>
#include <Core/Object.h>
#include <Core/StaticMeshComponent.h>
#include <EditorDirector/EditorEditMode.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UITextButton.h>

PrefabWorkSpaceManager *PrefabWorkSpaceManager::GetInstance()
{

    static PrefabWorkSpaceManager instance;
    return &instance;
}

PrefabWorkSpaceManager::PrefabWorkSpaceManager() : mToolbarHeight(50.0f) {}

PrefabWorkSpaceManager::~PrefabWorkSpaceManager() {}

void PrefabWorkSpaceManager::Initialize(UI::UICanvas *canvas, Core::LogicalWindow *globalLogicalWindow,
                                        BaseSelectionManager *selectionManager)
{
    mSelectionManager = selectionManager;
    mSelectionManager->mOnSelectedComponentCallbackSystem.Register([this](Component *com)
                                                                   { OnSelectedComponent(com); });

    mWorkSpace = std::make_unique<Core::WorkSpace>();

    // Init logical Window
    InitLogicalWindow(canvas);
    mWorkSpace->AddLogicalWindow(globalLogicalWindow);
    mWorkSpace->SetGlobalOverlayWindow(globalLogicalWindow);

    // Init world
    mWorld = std::make_unique<World>();

    auto engineMode = new EditorEditMode;
    engineMode->GetTransformGizmo().SetComponentControlState(true);

    auto map = new Map;
    map->SetName("PlayMap");
    mWorld->Register(engineMode->GetEditorMap());
    mWorld->SetEngineMode(engineMode);

    mLogicalWindow->SetWorld(mWorld.get());
    Quad::EditorSceneManager::GetInstance()->RegisterWorld("PrefabWorld", mWorld.get());
    mWorld->SetActiveState(false);

    mWorld->Register(map);
    mWorld->SetCurrentMap(map);
    mWorld->GetCurrentMap()->CreateEntity<Quad::EditorSceneController>("SceneController");
    Quad::EditorProjectManager::GetInstance()->CreateEditorObjects(map, EditorPrefabSelectionManager::GetInstance());

    // mWorld->mOnMapObjectRemovedCallbackSystem.Register(
    //    [this](Object *object) { mWorld->mOnMapObjectRemovedCallbackSystem.ExecuteCallbacks(object); });

    // create ui
    InitUI(canvas);
}

void PrefabWorkSpaceManager::Update(float DeltaTime)
{

    mPrefabComponentPanel.Update(DeltaTime);
}

Core::WorkSpace *PrefabWorkSpaceManager::GetWorkSpace() const
{
    return mWorkSpace.get();
}

void PrefabWorkSpaceManager::SetPrefab(Prefab *prefab)
{
    mDestPrefab = prefab;

    if (mDestPrefab)
    {
        //   mComponentScrollPanel.SetObject(prefab->mDefaultObject);
        Object *prefabDestObject = prefab->mDefaultObject;

        mPrefabComponentPanel.SetObject(prefabDestObject);
        mPrefabPropertyPanel.SetObject(prefabDestObject);
    }
}

void PrefabWorkSpaceManager::OnPrefabEditActive()
{

    if (mWorld)
    {
        mWorld->SetActiveState(true);

        Entity *entity = dynamic_cast<Entity *>(mDestPrefab->mDefaultObject);

        if (entity)
        {
            entity->SetPositionLocal(0, 0, 0);
        }

        mWorld->AddPrefabObject(mDestPrefab->mDefaultObject);
    }
}

void PrefabWorkSpaceManager::OnPrefabEditInActive()
{

    if (mWorld)
    {
        mWorld->SetActiveState(false);
        if (mDestPrefab)
        {
            mDestPrefab->SetPositionWorld({0, 0, 0});
        }

        mWorld->RemovePrefabObject(mDestPrefab->mDefaultObject);
    }
}

void PrefabWorkSpaceManager::SyncPrefabInstances()
{

    Map *map = Quad::EditorSceneManager::GetInstance()->GetUserWorld()->GetCurrentMap();

    auto reflectionSystem = Quad::ReflectionSystem::GetInstance();
    Quad::ClassInfo *lastClassInfo =
        reflectionSystem->FindClassInfo(mDestPrefab->mDefaultObject->GetRunTimeClassName());

    Object *prefabObject = mDestPrefab->mDefaultObject;
    const std::vector<Component *> &prefabComList = prefabObject->GetComponentList();

    for (auto entity : map->GetEntityList())
    {

        Quad::ClassInfo *csinfo = lastClassInfo;

        if (entity->GetPrefabID() != mDestPrefab->GetID())
        {
            continue;
        }

        // 멤버변수들
        while (1)
        {
            if ((std::strcmp(csinfo->GetTypeName(), "Entity") == 0) ||
                (std::strcmp(csinfo->GetTypeName(), "Object") == 0))
                break;

            for (auto propinfo : csinfo->GetDeclaredPropertyVector())
            {
                if (entity->IsOverridenProperty(propinfo->mPropertyName))
                    continue;

                if (propinfo->mIsPointerType)
                {
                    auto csinfo = reflectionSystem->FindClassInfo(propinfo->mOriginType);
                    if (csinfo->IsAncestorClass("Component"))
                    {
                        // com pointer
                        ReflectComponentToInstance(
                            *reinterpret_cast<Component **>(propinfo->GetMemoryStart(prefabObject)),
                            *reinterpret_cast<Component **>(propinfo->GetMemoryStart(entity)));
                    }
                    else if (csinfo->IsAncestorClass("Object"))
                    {
                        // object pointer
                    }

                    continue;
                }

                if (propinfo->mIsTemplateType)
                {
                    // 일단 무시
                    continue;
                }

                if (propinfo->mIsBuiltinType)
                {
                    const std::string type = propinfo->mType;

                    ITypeDescriptor_BuiltIn *builtInDescriptor =
                        dynamic_cast<ITypeDescriptor_BuiltIn *>(propinfo->mTypeDescriptor);

                    builtInDescriptor->Copy(propinfo->GetMemoryStart(prefabObject), propinfo->GetMemoryStart(entity));
                }
            }

            std::string parentClassName = csinfo->GetParentClassList()[0];
            csinfo = reflectionSystem->FindClassInfo(parentClassName.c_str());
        }
        // 엔진에서 추가한 컴포넌트들

        std::vector<Component *> mDestoryComponents;
        for (auto com : entity->GetComponentList())
        {
            // 엔진에서 추가한것
            if (com->GetComponentFlag() == Core::EComponentFlag::eEngineAdded)
            {
                // 프리팹의 컴포넌트와 일치하는것
                auto prefabComIt =
                    std::find_if(prefabComList.begin(), prefabComList.end(),
                                 [com](Component *prefabCom)
                                 {
                                     if (prefabCom->GetPrefabComponenetKey() == com->GetPrefabComponenetKey())
                                         return true;
                                     return false;
                                 });

                if (prefabComIt != prefabComList.end())
                {
                    ReflectEngineComponentToInstance(*prefabComIt, com,
                                                     reflectionSystem->FindClassInfo(com->GetRunTimeClassName()));
                }
                else
                {

                    // prefab에없는 component라면 prefab이 해당 component를 제거했으니 인스턴스에서도 지워야하는것?
                    // prefab에없는 component라면 프리팹이 제거한 컴포넌트이거나, 혹은 각인스턴스별로 추가한
                    // 컴포넌트일수있다.

                    // 프리팹 컴포넌트였다면 제거
                    if (com->IsPrefabInheritedComponent(mDestPrefab->GetID()))
                    {
                        mDestoryComponents.push_back(com);
                    }
                }
            }
        }

        for (auto com : mDestoryComponents)
        {
            com->Destory();
        }

        // 프리팹에서 새로 추가된 컴포넌트들 반영
        SyncPrefabAddedComponents(mDestPrefab->mDefaultObject, entity);

        // 계층구조를 반영한다 .
        SyncPrefabComponentHierachy(mDestPrefab->mDefaultObject, entity);
    }
}

void PrefabWorkSpaceManager::InitLogicalWindow(UI::UICanvas *canvas)
{
    mLogicalWindow = std::make_unique<Core::LogicalWindow>();
    mLogicalWindow->mViewportController.SetViewportMode(Core::EViewportMode::eAnchored);

    mLogicalWindow->mViewportController.SetAnchorLeftState(true);
    mLogicalWindow->mViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
    mLogicalWindow->mViewportController.SetAnchorLeftRelValue(0.0f);

    mLogicalWindow->mViewportController.SetAnchorRightState(true);
    mLogicalWindow->mViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
    mLogicalWindow->mViewportController.SetAnchorRightRelValue(0.0f);

    mLogicalWindow->mViewportController.SetAnchorTopState(true);
    mLogicalWindow->mViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
    mLogicalWindow->mViewportController.SetAnchorTopRelValue(0.0f);
    //    mLogicalWindow->mViewportController.SetAnchorTopPixelValue(200.0f);

    mLogicalWindow->mViewportController.SetAnchorBottomState(true);
    mLogicalWindow->mViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mLogicalWindow->mViewportController.SetAnchorBottomRelValue(0.0f);

    mLogicalWindow->m3DWorldViewportController.SetViewportMode(Core::EViewportMode::eAnchored);
    mLogicalWindow->m3DWorldViewportController.SetAnchorLeftState(true);
    mLogicalWindow->m3DWorldViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::ePixel);
    mLogicalWindow->m3DWorldViewportController.SetAnchorLeftPixelValue(500.0f);

    mLogicalWindow->m3DWorldViewportController.SetAnchorRightState(true);
    mLogicalWindow->m3DWorldViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::ePixel);
    mLogicalWindow->m3DWorldViewportController.SetAnchorRightPixelValue(600.0f);

    mLogicalWindow->m3DWorldViewportController.SetAnchorTopState(true);
    mLogicalWindow->m3DWorldViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
    mLogicalWindow->m3DWorldViewportController.SetAnchorTopRelValue(0.0f);
    //    mLogicalWindow->mViewportController.SetAnchorTopPixelValue(0.0f);

    mLogicalWindow->m3DWorldViewportController.SetAnchorBottomState(true);
    mLogicalWindow->m3DWorldViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mLogicalWindow->m3DWorldViewportController.SetAnchorBottomRelValue(0.35f);

    mLogicalWindow->SetDebugGridRender(true);
    mLogicalWindow->SetActiveCanvas(canvas);
    // mLogicalWindow->SetWorld(EditorSceneManager::GetInstance()->GetUserWorld());
    mWorkSpace->AddLogicalWindow(mLogicalWindow.get());
}

void PrefabWorkSpaceManager::InitUI(UI::UICanvas *canvas)
{

    CreateToolbar(canvas);

    mPrefabComponentPanel.Initialize(canvas, mSelectionManager);
    mPrefabComponentPanel.SetPositionLocal(0, mToolbarHeight);
    mPrefabComponentPanel.mOnRequestAddComponentCallbackSystem.Register(
        [this](const std::string &componentClassName) { AddComponentToCurrentPrefab(componentClassName); });

    mPrefabPropertyPanel.Initialize(canvas);
    //   mPrefabPropertyPanel.SetPositionLocal(1000.0f, mToolbarHeight);
}

void PrefabWorkSpaceManager::CreateToolbar(UI::UICanvas *canvas)
{

    auto toolbar = canvas->CreateUIElement<UI::UIImage>("Toolbar");
    toolbar->SetSize(3000, mToolbarHeight);
    toolbar->SetColor(0.4f, 0.4f, 0.4f);
    toolbar->SetPositionLocal(0, 0);

    float marginX = 5.0f;
    float marginY = 5.0f;

    float posX = 0.0f;
    float posY = 0.0f;

    // ToDefaultEditButton
    auto toDefaultEditButton = toolbar->CreateChildUIElement<UI::UIButton>("toDefaultEditButton");
    toDefaultEditButton->SetSize(80, 40);
    posX += marginX;
    posY += marginY;
    toDefaultEditButton->SetPositionLocal(posX, posY);
    toDefaultEditButton->mUIImageComponent->UseTexture();
    toDefaultEditButton->mUIImageComponent->SetTexture("Engine/ArrowLeft");
    toDefaultEditButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [](float, float) { Quad::EditorDirector::GetInstance()->ChangeToDefaultEditWorkSpace(); });

    ShiftPosX(posX, toDefaultEditButton, marginX);

    auto SaveButton = toolbar->CreateChildUIElement<UI::UIButton>("SaveButton");
    SaveButton->SetSize(60, 40);
    SaveButton->SetPositionLocal(posX, posY);
    SaveButton->mUIImageComponent->UseTexture();
    SaveButton->mUIImageComponent->SetTexture("Engine/Save");
    SaveButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float) { SavePrefab(); });
}

inline void PrefabWorkSpaceManager::ShiftPosX(float &posX, UI::UIElement *element, float margin)
{
    posX += element->mTransform.GetSize().x + margin;
}

void PrefabWorkSpaceManager::SavePrefab()
{

    Quad::EditorProjectManager::GetInstance()->SaveAsset(mDestPrefab);

    // prefab instance들에 적용

    SyncPrefabInstances();
}

void PrefabWorkSpaceManager::ReflectComponentToInstance(Component *prefabComponent, Component *component)
{
    auto reflectionSystem = Quad::ReflectionSystem::GetInstance();
    const char *comName = prefabComponent->GetRunTimeClassName();

    auto classInfo = reflectionSystem->FindClassInfo(comName);

    if (classInfo->IsEngineClass())
    {
        ReflectEngineComponentToInstance(prefabComponent, component, classInfo);
    }
}

void PrefabWorkSpaceManager::ReflectEngineComponentToInstance(Component *prefabComponent, Component *component,
                                                              Quad::ClassInfo *classInfo)
{

    auto reflectionSystem = Quad::ReflectionSystem::GetInstance();
    std::string comName = classInfo->GetTypeName();

    component->SyncPrefabComponentFrom(prefabComponent);
}

void PrefabWorkSpaceManager::OnSelectedComponent(Component *com)
{

    mPrefabComponentPanel.OnSelectedComponent(com);
    mPrefabPropertyPanel.OnSelectedComponent(com);
}

void PrefabWorkSpaceManager::SyncPrefabAddedComponents(Object *prefabObject, Object *instance)
{

    if (prefabObject == nullptr || instance == nullptr)
        return;

    auto instanceComponentKeyTable = BuildPrefabComponentContextKeyTable(instance);

    for (auto prefabCom : prefabObject->GetComponentList())
    {

        if (prefabCom == nullptr || prefabCom->GetDeadState() ||
            !prefabCom->HasComponentFlag(Core::EComponentFlag::eEngineAdded))
            continue;

        const std::string prefabComponentKey = prefabCom->GetPrefabComponenetKey();
        auto keyIt = instanceComponentKeyTable.find(prefabComponentKey);
        if (keyIt != instanceComponentKeyTable.end())
            continue;

        // instance 에 해당 컴포넌트를 생성한다.

        auto newCom = instance->CreateComponent(prefabCom->GetRunTimeClassName(), prefabCom->GetInstanceName());

        if (newCom == nullptr)
            continue;

        newCom->SetComponentFlag(Core::EComponentFlag::eEngineAdded);
        newCom->SetPrefabInheritedComponent(mDestPrefab->GetID(), prefabComponentKey);

        ReflectComponentToInstance(prefabCom, newCom);
    }
}

void PrefabWorkSpaceManager::SyncPrefabComponentHierachy(Object *prefabObject, Object *instance)
{

    if (prefabObject == nullptr || instance == nullptr)
        return;

    auto prefabComponentKeyTable = BuildPrefabComponentContextKeyTable(prefabObject);
    auto instanceComponentKeyTable = BuildPrefabComponentContextKeyTable(instance);

    for (const auto &e : prefabComponentKeyTable)
    {
        std::string prefabComponentKey = e.first;
        Component *prefabComponent = e.second;

        SceneComponent *prefabSceneComponent = dynamic_cast<SceneComponent *>(prefabComponent);

        if (prefabSceneComponent == nullptr)
            continue;

        auto instanceComponentIt = instanceComponentKeyTable.find(prefabComponentKey);
        if (instanceComponentIt == instanceComponentKeyTable.end())
            continue;

        SceneComponent *instanceSceneComponent = dynamic_cast<SceneComponent *>(instanceComponentIt->second);

        if (instanceSceneComponent == nullptr)
            continue;

        SceneComponent *prefabParentSceneComponent = prefabSceneComponent->GetParent();
        if (prefabParentSceneComponent == nullptr)
        {
            instanceSceneComponent->SetParent(nullptr, false);
            continue;
        }

        const std::string &prefabParentKey = prefabParentSceneComponent->GetPrefabComponenetKey();
        auto instanceParentComponentIt = instanceComponentKeyTable.find(prefabParentKey);
        if (instanceParentComponentIt == instanceComponentKeyTable.end())
            continue;

        SceneComponent *instanceParentSceneComponent =
            dynamic_cast<SceneComponent *>(instanceParentComponentIt->second);

        if (instanceParentSceneComponent == nullptr)
            continue;

        instanceSceneComponent->SetParent(instanceParentSceneComponent, false);
    }
}

std::unordered_map<std::string, Component *> PrefabWorkSpaceManager::BuildPrefabComponentContextKeyTable(
    Object *instance)
{
    if (instance == nullptr)
        return {};

    std::unordered_map<std::string, Component *> table;

    for (auto com : instance->GetComponentList())
    {
        if (com == nullptr || com->GetDeadState())
            continue;

        if (!com->IsPrefabInheritedComponent(mDestPrefab->GetID()))
            continue;

        if (com->GetPrefabComponenetKey().empty())
            continue;

        table.insert({com->GetPrefabComponenetKey(), com});
    }

    return table;
}

void PrefabWorkSpaceManager::AddComponentToCurrentPrefab(const std::string &componentClassName)
{

    if (mDestPrefab == nullptr)
        return;

    mDestPrefab->AddComponent(componentClassName);
}

#pragma once
#include <EditorDirector/PrefabWorkSpaceData.h>
#include <memory>
#include <unordered_map>

namespace Quad
{
class ClassInfo;
}

namespace UI
{
class UICanvas;
}

namespace Core
{
class LogicalWindow;
class WorkSpace;
} // namespace Core

class Prefab;
class World;
class Component;
class BaseSelectionManager;
class PrefabWorkSpaceManager
{
  public:
    static PrefabWorkSpaceManager *GetInstance();
    PrefabWorkSpaceManager();
    ~PrefabWorkSpaceManager();

    void Initialize(UI::UICanvas *canvas, Core::LogicalWindow *globalLogicalWindow,
                    BaseSelectionManager *selectionManager);

    void Update(float DeltaTime);

    Core::WorkSpace *GetWorkSpace() const;

    void SetPrefab(Prefab *prefab);

    // 프리팹workspace가 활성화되면호출할것
    void OnPrefabEditActive();

    // 비활성화 ...
    void OnPrefabEditInActive();

    void SyncPrefabInstances();

  private:
    void InitLogicalWindow(UI::UICanvas *canvas);

    void InitUI(UI::UICanvas *canvas);
    void CreateToolbar(UI::UICanvas *canvas);

    inline void ShiftPosX(float &posX, UI::UIElement *element, float margin);
    void SavePrefab();

#pragma region Reflect

    void ReflectComponentToInstance(Component *prefabComponent, Component *component);
    void ReflectEngineComponentToInstance(Component *prefabComponent, Component *component, Quad::ClassInfo *classInfo);

    //  void ReflectStaticMeshComponentToInstance(Component *prefabComponent, Component *component);
    //   void ReflectCameraComponentToInstance(Component *prefabComponent, Component *component);

#pragma endregion

    void OnSelectedComponent(Component *com);

    // 프리팹에 새로추가된 컴포넌트들을에대해 인스턴스도 동일하게 만든다.
    void SyncPrefabAddedComponents(Object *prefabObject, Object *instance);

    // 인스턴스의 컴포넌트들 계층을 프리팹과 동기화
    void SyncPrefabComponentHierachy(Object *prefabObject, Object *instance);

    // key - component
    std::unordered_map<std::string, Component *> BuildPrefabComponentContextKeyTable(Object *instance);

    void AddComponentToCurrentPrefab(const std::string &componentClassName);

  private:
    Prefab *mDestPrefab = nullptr;
    std::unique_ptr<Core::WorkSpace> mWorkSpace;
    std::unique_ptr<Core::LogicalWindow> mLogicalWindow;

    Quad::PrefabComponentPanel mPrefabComponentPanel;
    Quad::PrefabPropertyPanel mPrefabPropertyPanel;

    float mToolbarHeight;

    std::unique_ptr<World> mWorld;

    BaseSelectionManager *mSelectionManager = nullptr;
};

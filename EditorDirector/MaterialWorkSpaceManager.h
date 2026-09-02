#pragma once

#include <EditorDirector/MaterialEditUIController.h>
#include <memory>

namespace Core
{
class WorkSpace;
class LogicalWindow;
class StaticMeshObject;
} // namespace Core

namespace UI
{
class UICanvas;

}

namespace CoreAsset
{
class Material;
}
class BaseSelectionManager;
class World;

class MaterialWorkSpaceManager
{
  public:
    static MaterialWorkSpaceManager *GetInstance();
    MaterialWorkSpaceManager();
    ~MaterialWorkSpaceManager();

    void Initialize(Core::LogicalWindow *globalLogicalWindow, BaseSelectionManager *selectionManager);
    void Update(float DeltaTime);

    void SetMaterial(CoreAsset::Material *material);

    Core::WorkSpace *GetWorkSpace() const;
    // 프리팹workspace가 활성화되면호출할것
    void OnWorkSpaceActive();

    // 비활성화 ...
    void OnWorkSpaceInActive();

  private:
    void InitLogicalWindow(UI::UICanvas *canvas);
    void InitWorld();
    void InitUI(UI::UICanvas *canvas);

#pragma region UI
    void CreateToolbar(UI::UICanvas *canvas);

#pragma endregion

    void ApplyPreviewMaterialToTargetMaterial();
    void CopyMaterial(CoreAsset::Material *sourceMaterial, CoreAsset::Material *targetMaterial);

  private:
    CoreAsset::Material *mTargetMaterial = nullptr;
    CoreAsset::Material *mPreviewMaterial;

    Core::StaticMeshObject *mSphereElement = nullptr;
    std::unique_ptr<Core::WorkSpace> mWorkSpace;
    std::unique_ptr<Core::LogicalWindow> mLogicalWindow;
    std::unique_ptr<World> mWorld;
    BaseSelectionManager *mSelectionManager = nullptr;

    float mToolbarHeight = 50.0f;
    MaterialEditUIController mMaterialEditUIController;
};

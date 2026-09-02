#pragma once
#include <Core/CoreType.h>
#include <CoreBase/PrograssContext.h>
#include <EditorDirector/GlobalOverlayType.h>
#include <functional>
#include <memory>
#include <string>

namespace UI
{
class UICanvas;
class UIElement;
class UIImage;
class UITextComponent;
class UITextButton;
struct UIColor;
} // namespace UI

class Map;
class Object;
namespace Core
{

class LogicalWindow;
} // namespace Core
namespace CoreAsset
{
class Asset;
}

enum class EScenePlayState
{
    eNone = 0,
    ePlay,
    ePause
};

class ITaskUIController;
class ClassListUIScrollPanel;
class Prefab;
class UIEditorDebugHUD;
class MapPlaySettingPanel;

class DefaultEditUIContext
{
    UI::UIImage *mToolbar;
};

class PrefabEditUIContext
{
  public:
    PrefabEditUIContext();
    ~PrefabEditUIContext();

    void SetActive(bool flag);

    UI::UIImage *mComPanel = nullptr;

  private:
};

struct DragPayloadContext
{
    EDragDropType mType;
    CoreAsset::Asset *mAsset = nullptr;
    Object *mObject = nullptr;
    UI::UIElement *mUIElement = nullptr;
    // AssetBrower에서 사용하는 ICon 이미지를위한 필드가 있어야할듯
};

enum class EDefault3DObjectType
{
    eCube = 0,
    eCylinder,
    eCone,
    eArrow,
    eSphere
};

class GlobalOverlayManager
{
  public:
    static GlobalOverlayManager *GetInstance();
    GlobalOverlayManager();
    ~GlobalOverlayManager();

    void Update(float deltaTime);

    void ChangeToDefaultEdit();
    void ChangeToPrefabEdit();
    void ChangeToMaterialEdit();

    void Initialize(UI::UICanvas *overlayCanvas, Core::LogicalWindow *overlayWindow);

    void ShowMessageBox(const std::string &str);
    void CloseMessageBox();
    void CloseCurrentContextMenu(UI::UIImage *selectedPanel, UI::UITextButton *parentButton);
    void CloseCurrentContextMenuAll();

    void ShowSaveMapBox(Map *map, const std::function<void()> &onDecisionCallback);

    void StartDragDrop(const DragPayload &payload);
    void ClearDragDrop();
    void TryDropCurrentPayload();
    bool GetDragDropState() const;
    const DragPayload &GetCurrentDragPayload() const;

    EScenePlayState GetScenePlayState() const;
    void SetScenePlayState(EScenePlayState state);

    void OnScenePlayButtonDown();
    void OnScenePlayEndButtonDown();

    void ShowPrefabEditWindow(Prefab *prefab);

  private:
    void CreateMessageBox();
    void CreateToobar();

    void CreateProjectBar();
    void CreateSceneBar();
    void CreatePlayBar();

    void CreatePrefabEditPanel();

    UI::UITextButton *AddMenuButton(UI::UIElement *parent, const std::string &name, const std::string &objectName,
                                    float width, const UI::UIColor &baseColor);

    // dir base판넬이 부모기준 배치방향  0 : 아래, 1 : 우
    UI::UIImage *CreateBaseContextPanel(UI::UITextButton *button, const std::string &objectName, float width,
                                        int dir = 0);

    void CreateFileContextPanel(UI::UITextButton *fileButton);
    void CreateEditContextPanel(UI::UITextButton *editButton);
    void CreateAssetContextPanel(UI::UITextButton *editButton);
    void CreateObjectContextPanel(UI::UITextButton *editButton);

    void CreateGameObjectContextPanel(UI::UITextButton *gameObjectButton);
    void Create3DObjectContextPanel(UI::UITextButton *gameObjectButton);
    void CreateLightObjectContextPanel(UI::UITextButton *button);
    void CreateSceneSettingContextPanel(UI::UITextButton *settingButton);
    void CreatePlayerStartAtEditorView();

    void CreateSaveMapPanel();
    void CreateDragDropImage();

    void CreateGenerationObjectClassPanel();
    void CreateGenerationPrefabPanel();

    void ShowContextMenu(UI::UIImage *panel, UI::UITextButton *parentButton = nullptr);
    bool CheckCurrentOpenedMenu(UI::UIImage *panel);

    struct MenuData
    {
        std::string name;
        std::string text;
        float width;
    };

    void SetCurrentTaskUIController(ITaskUIController *controller);

    void OnDragDropMouseEnterMainWindow();
    void OnDragDropMouseLeaveMainWindow();
    void UpdatePrefabPosIfDragDrop(int clinetPosX, int clientPosY);

    void OpenGameModeSettingPanel();
    void OpenMapRenderInspector();
    std::vector<Core::ObjectSourceCandidate> BuildObjectSourceCandiateList();
    void AppendObjectClassCandidate(std::vector<Core::ObjectSourceCandidate> &oList);
    void AppendPrefabCandidate(std::vector<Core::ObjectSourceCandidate> &oList);

    std::vector<Core::ObjectSourceCandidate> BuildObjectControllerSourceCandidateList();

    void StartImport();

    void UpdateDragDropMainWindowState(Core::LogicalWindow *hitWindow);

    void Create3DObjectToWorld(EDefault3DObjectType objectType);

  private:
    Core::LogicalWindow *mOverlayWindow = nullptr;
    UI::UICanvas *mOverlayCanvas = nullptr;
    Core::LogicalWindow *mMainWindow = nullptr;

    UIEditorDebugHUD *mDebugHUD = nullptr;
    UI::UIImage *mMessageBox;
    UI::UITextComponent *mMessageBoxTextCom;

    UI::UIImage *mToolbar;

    UI::UIImage *mCurrentMenuContextPanel;

    // 활성화된 판넬 리스트
    std::vector<UI::UIImage *> mCurrentMenuContextPanelList;

    UI::UIElement *mSaveMapPanel;
    // 맵 저장에 실행여부 이후 호출될 콜백
    std::function<void()> mOnSaveMapDicisionCallback;

    bool mIsOpeningMenuPanel;

    Map *mToSaveMap;

    UI::UIImage *mDragDropImage;
    UI::UIImage *mGenerationObjectClassPanel;
    UI::UIImage *mGenerationPrefabPanel;

    bool mIsMouseOnMainWindow = false;
    bool mIsDragDrop;
    DragPayload mCurrentDragDropPayload;               // 원본
    DragPayloadContext mCurrentDragDropPayloadContext; // 수정된 복사본
    UI::UIElement *mCurrentHoverDropTarget = nullptr;  // 현재 hover상태인 drop target com가 존재하는 ui element

    /*  UI::UIColor mHoverColor = {0.7f, 0.7f, 0.7f, 1.0f};
      UI::UIColor mHoverReleaseColor = {0.5f, 0.5f, 0.5f};*/

    EScenePlayState mScenePlayState;

    // 어떤 작업수행상태 , true: 작업진행중, false: 작업x
    bool mIsWorking = false;
    // 현재 진행중인 작업에대한 context
    std::shared_ptr<TaskContext> mWorkingTaskContext;

    ITaskUIController *mCurrentTaskUIController = nullptr;
    std::unique_ptr<ITaskUIController> mClassGenerationTaskUIController = nullptr;

    ClassListUIScrollPanel *mClassListUIScrollPanel = nullptr;
    MapPlaySettingPanel *mMapPlaySettingPanel = nullptr;

    PrefabEditUIContext mPrefabEditUIContext;
};

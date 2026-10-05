#pragma once

#include <CoreBase/CallbackSystem.h>
#include <EditorDirector/EditorPlayMode.h>
#include <string>
#include <unordered_map>
#include <vector>

class World;
class Map;
class CameraComponent;
class ObjectController;

namespace UI
{
class UICanvas;
}
enum class EPlaySessionState
{
    eStopped,
    ePlaying,
    ePaused
};

using OnUserMapAddedCallbackSystem = Core::MultiCallbackSystem<Map *>;
using OnUserMapRemovedCdallbackSystem = Core::MultiCallbackSystem<Map *>;

namespace Quad
{

class EditorSceneManager
{
  public:
    static EditorSceneManager *GetInstance();
    EditorSceneManager();
    ~EditorSceneManager();

    void RegisterWorld(const std::string &name, World *world);
    void UnRegisterWorld(World *world);
    World *GetWorld(const std::string &name) const;

    void Update(float deltaTime);
    void EndUpdate(float detlaTime);
    void CleanUp();
    void EndFrame();

    void AddUserMap(Map *map);

    World *GetUserWorld() const;

    void SetEditorMap(Map *map);

    // 월드에 등록된 맵 중 에디터 보조 맵을 제외한 목록을 제공한다. 반환 포인터의 소유권은 이전하지 않는다.
    std::vector<Map *> GetUserMapList() const;
    std::vector<std::string> GetUserMapNameList() const;

    void PlayUserWorld();
    void PauseUserWorld();
    void ReleaseUserWorldPause();
    void EndUserWorld();

    void SetUserCanvas(UI::UICanvas *canvas);

    OnUserMapAddedCallbackSystem mOnUserMapAddedCallbackSystem;
    OnUserMapRemovedCdallbackSystem mOnUserMapRemovedCallbackSystem;

  private:
    void OnUserPlayMapChanged(Map *map);

  private:
    std::unordered_map<std::string, World *> mWorldTable;
    std::unordered_map<std::string, UI::UICanvas *> mUICanvasTable;
    const std::string mUserWorldName = "UserWorld";

    // 사용자 맵 목록에서 제외할 비소유 참조다. 생성과 수명 관리는 기존 EditorEditMode 경로를 유지한다.
    Map *mEditorMap = nullptr;

    World *mUserPlayWorld;
    std::vector<Map *> mUserPlayMapList;

    CameraComponent *mUserMapCameraComponent = nullptr;
    ObjectController *mUserMapObjectController = nullptr;

    EditorPlayMode mEditorPlayMode;

    EPlaySessionState mPlaySessionState = EPlaySessionState::eStopped;
};

} // namespace Quad

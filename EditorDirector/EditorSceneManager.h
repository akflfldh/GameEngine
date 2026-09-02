#pragma once

#include <EditorDirector/EditorPlayMode.h>
#include <string>
#include <unordered_map>

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

    void AddUserMap(Map *map);

    World *GetUserWorld() const;

    void PlayUserWorld();
    void PauseUserWorld();
    void ReleaseUserWorldPause();
    void EndUserWorld();

  private:
    std::unordered_map<std::string, World *> mWorldTable;
    std::unordered_map<std::string, UI::UICanvas *> mUICanvasTable;
    const std::string mUserWorldName = "UserWorld";

    World *mUserPlayWorld;
    std::vector<Map *> mUserPlayMapList;

    CameraComponent *mUserMapCameraComponent = nullptr;
    ObjectController *mUserMapObjectController = nullptr;

    EditorPlayMode mEditorPlayMode;

    EPlaySessionState mPlaySessionState = EPlaySessionState::eStopped   ;
};

} // namespace Quad
#pragma once

#include <Core/CoreDllExport.h>

class World;
namespace Core
{

class CORE_API_LIB SceneManager
{
  public:
    static SceneManager *GetInstance();
    ~SceneManager();

    void Update(float deltaTime);
    void Render();

    World *GetWorld() const;

  private:
    SceneManager();

    World *mUserWorld;
};

} // namespace Core
#pragma once
#include <string>

class Prefab;

class PrefabGenerationManager
{

  public:
    static PrefabGenerationManager *GetInstance();

    PrefabGenerationManager();
    ~PrefabGenerationManager();

    Prefab *CreatePrefab(const std::string &parentClassName, const std::string &prefabName);

  private:
};

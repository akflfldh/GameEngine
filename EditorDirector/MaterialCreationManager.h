#pragma once

#include <string>

namespace UI
{
class UICanvas;
}

class MaterialCreationUIController;

class MaterialCreationManager
{

  public:
    static MaterialCreationManager *GetInstance();
    MaterialCreationManager();
    ~MaterialCreationManager();

    void Initialize(UI::UICanvas *canvas);

    void RequestCreatingMaterial();

  private:
    void OnMaterialCreationRequested(const std::string &mateiralName);

  private:
    MaterialCreationUIController *mMaterialCreationUIController;
};

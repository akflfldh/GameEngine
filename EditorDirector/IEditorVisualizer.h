#pragma once

class Map;
class CameraComponent;

class IEditorVisualizer
{
  public:
    IEditorVisualizer();
    virtual ~IEditorVisualizer() = 0;

    void Initialize(Map *editorMap);
    void BindMap(Map *sourceMap);
    virtual void UnBindMap();
    virtual void Update(CameraComponent *editorCamera) = 0;

    Map *GetEditorMap() const;
    Map *GetSourceMap() const;

  protected:
    virtual void OnBeforeBindingMap();
    virtual void OnBindMap(Map *sourceMap);

  private:
    Map *mEditorMap = nullptr;
    Map *mSourceMap = nullptr;
};

#pragma once

class BaseSelectionManager;

class IEditorVisualizerObject
{
  public:
    IEditorVisualizerObject();
    virtual ~IEditorVisualizerObject() = 0;

    virtual void SelectSource(BaseSelectionManager *selectionManager) = 0;

  private:
};

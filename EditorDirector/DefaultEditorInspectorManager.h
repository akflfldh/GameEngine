#pragma once

#include <unordered_map>

class IInspector;

enum class EDefaultEditorInspectorType : uint8_t
{
    eNone = 0,
    eProprety,
    eMapSetting
};

struct InspectorContext;

class DefaultEditorInspectorManager
{

  public:
    static DefaultEditorInspectorManager *GetInstance();

    DefaultEditorInspectorManager();
    ~DefaultEditorInspectorManager();

    // inspector들의 beginUI호출
    void BeginInspectorUI();

    void ActivateInsepctor(EDefaultEditorInspectorType type, const InspectorContext &inspectorContext);

    void RegisterInspector(EDefaultEditorInspectorType type, IInspector *Inspector);

  private:
    EDefaultEditorInspectorType mCurrentInspectorType = EDefaultEditorInspectorType::eNone;
    std::unordered_map<EDefaultEditorInspectorType, IInspector *> mInsepctorTable;
};

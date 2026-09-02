#pragma once

class Map;

struct InspectorContext
{
    Map *mTargetMap = nullptr;
};

class IInspector
{
  public:
    IInspector();
    virtual ~IInspector() = 0;

    virtual void BeginUI() = 0;

    virtual void ActivateInspector() = 0;
    virtual void DeActivateInspector() = 0;

    virtual void SetInspectorContext(const InspectorContext &inspectorContext) = 0;

  private:
};

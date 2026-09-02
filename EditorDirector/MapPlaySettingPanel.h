#pragma once

#include <Core/CoreType.h>
#include <UiSystem/UIImage.h>

#include "MapPlaySettingPanel.generated.h"

/*
역할
  - 현재 map bind
      - Default Player Source 표시,
      [source 상태에 따라 비주얼업데이트,Class설정박스, Prefab설정박스]


      - Default Player Class 설정
      - Default Player Prefab 설정
      - Clear 버튼

*/

class Map;
class UISearchSelectBox;
class REFLECT_CLASS(EngineClass) MapPlaySettingPanel : public UI::UIImage
{
    GENERATED_BODY(MapPlaySettingPanel)
  public:
    MapPlaySettingPanel();
    virtual ~MapPlaySettingPanel();

    void SetObjectSourceList(const std::vector<Core::ObjectSourceCandidate> &list);
    void SetObjectControllerSourceList(const std::vector<Core::ObjectSourceCandidate> &list);

    void BindMap(Map *map);

  protected:
    virtual void OnBegin() override;
    void CreateTitle(float posY);
    void CreatePlayerObjectUI(float posY);

    void CreatePlayerControllerUI(float posY);

    // void RebuildObjectSourceCandidates(const std::string &subStr);
    void SetSelectedObjectSource(uint32_t itemID);
    void SetSelectedControllerSource(uint32_t itemID);

    // map bind시 현재 map의 설정값으로 ui를 설정
    void RefreshSelectedObjectSource();
    void RefreshSelectedObjectControllerSource();

  private:
    Map *mTargetMap = nullptr;
    UISearchSelectBox *mSearchObjectSelectBox = nullptr;
    UISearchSelectBox *mSearchControllerSelectBox = nullptr;
    std::vector<Core::ObjectSourceCandidate> mObjectSourceList;
    std::vector<Core::ObjectSourceCandidate> mObjectControllerSourceList;
    uint32_t mSelectedObjectItemID = 0;
    uint32_t mSelectedControllerItemID = 0;
};
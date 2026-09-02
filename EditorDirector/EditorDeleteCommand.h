#pragma once

/*

선택된 오브젝트 ,컴포넌트를 삭제하는곳을 EditorDeleteCommand에서만 수행하도록하고
다른 모든 시스템들이 이곳을 통하도록하자.

컴포넌트의 경우 현재 에디터를 통해서 생성한 컴포넌트만 제거할수있다.



*/

class BaseSelectionManager;
class Component;
class Object;
class SceneComponent;

class EditorDeleteCommand
{
  public:
    EditorDeleteCommand();
    ~EditorDeleteCommand();

    // 현재 선택된 오브젝트,컴포넌트를 제거한다
    // 컴포넌트 우선(만약 선택된 컴포넌트가있다면 컴포넌트제거, 없다면 선택된 오브젝트 제거)
    static bool Execute(BaseSelectionManager *selectionMangaer);

  private:
    static bool DeleteSelectedComponent(BaseSelectionManager *selectionManager);
    static bool DeleteSelectedObject(BaseSelectionManager *selectionManager);

    // 루트씬 컴포넌트는 제거를 하지않는다.등등
    static bool CanDeleteComponenet(Component *com);
    static bool CanDeleteObject(Object *object);

    static bool CanDeleteSingleComponent(Component *com);

    // 엔진에서 추가하지않은 Native 컴포넌트가 있는경우 제거할수없다.
    static bool CanDeleteSceneComponentSubtree(SceneComponent *com);
};

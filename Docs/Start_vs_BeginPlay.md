# Start vs BeginPlay 정리

## 결론

현재 엔진 단계에서는 `BeginPlay`를 바로 추가하지 않아도 된다.

대신 `Start`와 `GameMode::SetupPlay`의 역할을 명확히 나누는 것이 우선이다.

```text
Start
  Object / Component의 1회 초기화
  Editor map과 Play map 양쪽에서 호출될 수 있음
  gameplay-only 로직을 넣지 않는 것이 안전함

GameMode::SetupPlay
  Play 시작 직전에 gameplay 연결을 수행
  controller 생성
  player object resolve/spawn
  possess
  active camera 설정
```

즉 현재 추천 흐름은 다음과 같다.

```text
EditorPlayMode::Start(map)
  -> GameMode::SetupPlay(map)
  -> map->Start()
  -> InitializeGizmo(map)
```

`BeginPlay`는 나중에 gameplay-only lifecycle이 필요해질 때 추가한다.

---

## 왜 Start와 BeginPlay를 나누자는 이야기가 나왔는가

엔진이 커지면 보통 두 시점이 분리된다.

```text
Object가 map에 붙고 사용할 준비가 되는 시점
  !=
게임 simulation이 실제로 시작되는 시점
```

에디터에서는 play 전에도 object/component가 존재한다.

```text
Map load
  -> Object 생성
  -> Component 생성
  -> Render proxy 등록
  -> Inspector 편집 가능
```

이 시점에 필요한 초기화는 있다.

```text
component 내부 포인터 연결
render proxy 생성/등록
asset reference resolve
editor 표시용 초기화
```

하지만 이것은 gameplay 시작은 아니다.

반대로 Play를 누르면 필요한 초기화가 있다.

```text
player controller 생성
player object possess
AI 시작
quest 시작
physics simulation 시작
runtime input 활성화
```

이런 것들이 `BeginPlay`의 역할이 될 수 있다.

---

## 현재 엔진의 실제 구조

현재 `Map::Start()`는 object list를 순회한다.

```cpp
void Map::Start()
{
    for (auto element : mEntityList)
    {
        element->Start();
    }
}
```

`Object::Start()`는 `OnBegin()`과 component start를 호출한다.

```cpp
void Object::Start()
{
    OnBegin();

    for (auto com : mComList)
    {
        com->Start();
    }
}
```

`Component::Start()`는 `OnBegin()`을 호출한다.

```cpp
void Component::Start()
{
    OnBegin();
}
```

따라서 현재 `Start`의 실제 의미는 다음에 가깝다.

```text
Object::OnBegin()
Component::OnBegin()
을 호출하는 1회 초기화 단계
```

---

## 중요한 주의점

현재 `Start`는 에디터 맵에서도 호출될 수 있다.

예를 들어 editor object를 만든 뒤 다음 흐름이 있다.

```text
map->GetWorld()->StartMap()
  -> world->StartMap()
  -> engineMode->Start(map)
  -> map->Start()
```

그러므로 `Start`에 gameplay-only 코드를 넣으면 문제가 생길 수 있다.

```text
Map을 에디터에서 열었을 뿐인데
AI가 시작되거나
player controller가 possess하거나
quest가 시작되는 상황
```

따라서 현재 정책은 이렇게 두는 것이 안전하다.

```text
Start
  editor/play 공통 초기화만 수행

GameMode::SetupPlay
  play 전용 연결만 수행
```

---

## 시각적 흐름

### Editor Map Load / Edit 흐름

```text
EditorProjectManager
  -> Map load
  -> Editor objects 생성
  -> World::StartMap()
  -> EditorEditMode::Start(map)
  -> map->Start()
  -> Object::Start()
  -> Component::Start()
```

이 흐름에서는 gameplay가 시작되면 안 된다.

```text
허용되는 일
  render proxy 준비
  editor camera 준비
  gizmo 준비
  component 기본 초기화

피해야 할 일
  player possess
  AI 시작
  quest 시작
  runtime input rule 적용
```

### Editor Play 흐름

```text
EditorSceneManager::PlayUserWorld()
  -> user map 복제
  -> play map 생성
  -> editor play camera/controller 생성
  -> UserPlayWorld 활성화
  -> World::StartMap()
  -> EditorPlayMode::Start(playMap)
  -> GameMode::SetupPlay(playMap)
  -> playMap->Start()
```

현재는 이 구조가 가장 적절하다.

```text
GameMode::SetupPlay
  -> player controller 생성
  -> player object 찾기 또는 생성
  -> controller->Possess(player)
  -> active camera 설정

Map::Start
  -> object/component 1회 초기화
```

---

## 선택지

### 선택지 A: Start만 유지

현재 추천하는 단기 정책이다.

```text
Start 하나만 사용
GameMode::SetupPlay가 gameplay 시작 연결 담당
BeginPlay는 아직 만들지 않음
```

장점:

```text
구조가 단순함
현재 코드 변경이 적음
지금 단계에서 중복 lifecycle을 만들지 않음
```

단점:

```text
나중에 gameplay-only 시작 시점이 필요해지면 BeginPlay를 추가해야 함
Start에 gameplay 로직이 섞이지 않도록 계속 주의해야 함
```

### 선택지 B: Start와 BeginPlay 분리

장기적으로는 이 구조가 더 명확할 수 있다.

```text
Start
  editor/play 공통 초기화

BeginPlay
  runtime gameplay 시작
```

흐름:

```text
Editor map load
  -> map->Start()

Play start
  -> GameMode::SetupPlay()
  -> map->Start()
  -> map->BeginPlay()
```

장점:

```text
editor 초기화와 gameplay 시작이 명확히 분리됨
AI, physics, quest, runtime-only state를 넣기 쉬움
```

단점:

```text
API가 늘어남
현재 단계에서는 Start와 역할이 중복처럼 보일 수 있음
```

---

## 현재 권장 정책

지금은 `BeginPlay`를 추가하지 않는다.

대신 다음 규칙을 지킨다.

```text
1. Start는 object/component의 1회 초기화로 유지한다.
2. Start는 editor map에서도 호출될 수 있다고 가정한다.
3. Start에는 gameplay-only logic을 넣지 않는다.
4. Play 시작 전 gameplay 연결은 GameMode::SetupPlay에서 수행한다.
5. BeginPlay는 AI, quest, physics, level script 같은 runtime-only 시작점이 필요해질 때 추가한다.
```

현재 적절한 코드 형태:

```cpp
void EditorPlayMode::Start(Map* map)
{
    if (map == nullptr)
        return;

    mGameMode.SetupPlay(map);
    map->Start();
    InitializeGizmo(map);
}
```

---

## 나중에 BeginPlay가 필요해지는 신호

다음 기능이 들어오기 시작하면 `BeginPlay / EndPlay`를 추가하는 것이 좋다.

```text
AI activation
Quest system start
Physics simulation start
Level script start
Animation runtime state start
Network session start
Save/load 이후 runtime-only state 복원
```

그때의 형태:

```cpp
void EditorPlayMode::Start(Map* map)
{
    if (map == nullptr)
        return;

    mGameMode.SetupPlay(map);
    map->Start();
    map->BeginPlay();
    InitializeGizmo(map);
}
```

종료 시:

```cpp
void EditorSceneManager::EndUserWorld()
{
    if (Map* playMap = mUserPlayWorld->GetCurrentMap())
    {
        playMap->EndPlay();
    }

    // play world cleanup
}
```

---

## 요약

```text
현재 단계
  Start + GameMode::SetupPlay 조합으로 충분함

Start
  editor/play 공통 1회 초기화

GameMode::SetupPlay
  play 전용 controller/player/camera 연결

BeginPlay
  지금은 보류
  runtime-only lifecycle이 필요해질 때 추가
```


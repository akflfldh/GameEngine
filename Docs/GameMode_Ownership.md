# GameMode Ownership

## Context

현재 엔진은 최소 플레이 가능한 런타임 루프를 구축하는 단계다.

현재 필요한 핵심 문제는 다음과 같다.

- 이 map을 play할 때 어떤 player object 또는 prefab을 사용할 것인가.
- 어떤 controller를 만들고 player를 possess할 것인가.
- 어떤 camera를 active camera로 사용할 것인가.

이 문제는 `GameMode`와 관련되어 있지만, 설정 데이터의 소유권을 어디에 둘지 정해야 한다.

## GameMode Setter를 Public으로 여는 방식

가장 빠른 방식은 `GameMode`의 설정 함수를 public으로 열고, 외부에서 직접 값을 넣는 것이다.

```cpp
mGameMode.SetDefaultPlayerObjectPrefab(prefabID);
mGameMode.SetupPlay(map);
```

이 방식은 빠르게 동작을 확인하기 좋다.

하지만 현재 구조에서 `GameMode`는 `EditorPlayMode` 내부의 실행용 객체에 가깝다.

```cpp
class EditorPlayMode : public EditorMode
{
private:
    GameMode mGameMode;
};
```

따라서 이 객체에 직접 설정하면 다음 문제가 있다.

- map 저장/로드에 설정이 포함되지 않는다.
- map마다 다른 default player 설정을 갖기 어렵다.
- `EditorPlayMode` 또는 `GameMode` 객체가 재생성되면 설정이 사라질 수 있다.
- 프로젝트를 다시 열었을 때 설정을 복원할 수 없다.

즉 public setter 방식은 "이번 play session에만 적용되는 임시 설정"에 가깝다.

## Map이 GameMode를 소유하는 방식

`DefaultPlayerObject` 설정은 "이 map을 플레이할 때 어떤 player를 쓸 것인가"라는 데이터다.

따라서 이 데이터는 play session 객체보다는 map asset에 가까운 성격을 가진다.

추천 구조는 다음과 같다.

```cpp
class Map : public CoreAsset::Asset
{
public:
    GameMode &GetGameMode();
    const GameMode &GetGameMode() const;

private:
    GameMode mGameMode;
};
```

play 시작 흐름은 다음처럼 바뀐다.

```cpp
void EditorPlayMode::Start(Map *map)
{
    if (map == nullptr)
        return;

    map->Start();
    map->GetGameMode().SetupPlay(map);
    map->BeginPlay();
}
```

이 구조에서 `EditorPlayMode`는 `GameMode`를 소유하지 않는다.

```cpp
// 제거 대상
GameMode mGameMode;
```

`EditorPlayMode`는 map lifecycle을 실행하는 역할만 가진다.

## Advantages

Map이 `GameMode`를 소유하면 의미가 명확해진다.

- map마다 다른 play rule을 가질 수 있다.
- editor UI는 현재 map의 `GameMode`를 수정하면 된다.
- play 시작 시 별도 settings 주입이 필요 없다.
- map 저장/로드에 `GameMode` 설정을 포함할 수 있다.
- `GameMode`가 "이 map의 play rule"이라는 의미를 가진다.

예를 들면 다음이 가능해진다.

- `Village.map`은 `VillagePlayerPrefab`을 default player로 사용한다.
- `Dungeon.map`은 `DungeonPlayerPrefab`을 default player로 사용한다.

## Serialization

현재 `GameMode`는 reflection 기반 `BaseClass` 계층이 아니다.

따라서 `Map`이 `GameMode mGameMode`를 직접 소유하더라도 자동으로 저장되지는 않는다.

`GameMode`에 명시적인 serialize 함수를 두는 방식이 가장 단순하다.

```cpp
class GameMode
{
public:
    void Serialize(Arch &arch);

    void SetDefaultPlayerObjectClass(const std::string &className);
    void SetDefaultPlayerObjectPrefab(CoreAsset::AssetID prefabID);
    void ClearDefaultPlayerObject();

private:
    Core::ESpawnObjectSourceType mDefaultPlayerObjectSource = Core::ESpawnObjectSourceType::eNone;
    std::string mDefaultPlayerObjectClassName;
    CoreAsset::AssetID mDefaultPlayerObjectPrefabID = NoneAssetID;
};
```

```cpp
void GameMode::Serialize(Arch &arch)
{
    arch << mDefaultPlayerObjectSource;
    arch << mDefaultPlayerObjectClassName;
    arch << mDefaultPlayerObjectPrefabID;
}
```

`Map::SerilaizeRawData()`에서는 object list를 저장하기 전에 `GameMode` 설정을 먼저 저장하면 된다.

```cpp
void Map::SerilaizeRawData(Arch &arch)
{
    mGameMode.Serialize(arch);

    // 이후 기존 object serialize
}
```

## Why Not Dynamic GameMode Class Yet

더 확장된 구조는 map이 `GameMode` class name을 저장하고, play 또는 load 시 reflection으로 생성하는 방식이다.

```cpp
class Map
{
private:
    std::string mGameModeClassName;
    std::unique_ptr<GameMode> mGameMode;
};
```

이 구조는 나중에 다음과 같은 사용자 정의 game mode가 필요할 때 유용하다.

```cpp
class RPGGameMode : public GameMode {};
class BattleGameMode : public GameMode {};
class VillageGameMode : public GameMode {};
```

하지만 현재 단계에서는 이 구조가 과하다.

여기서 "과하다"는 것은 개념적으로 틀렸다는 뜻이 아니다.

현재 필요한 문제보다 더 큰 시스템을 먼저 열게 된다는 뜻이다.

동적 `GameMode` class 구조를 제대로 만들려면 다음이 필요하다.

- `GameMode`를 reflection 생성 가능하게 만들어야 한다.
- `GameMode`가 `BaseClass` 계층에 들어갈지 결정해야 한다.
- 사용자 정의 `GameMode` 상속 구조를 설계해야 한다.
- `GameMode` 인스턴스 lifetime을 정해야 한다.
- map load 때 생성할지, play start 때 생성할지 결정해야 한다.
- polymorphic serialization을 설계해야 한다.
- editor class picker가 필요해진다.
- selected game mode class별 property inspector가 필요해진다.

현재 목표는 "default player를 지정하고 play 때 spawn/possess 하는 것"이다.

동적 `GameMode` class 구조는 "GameMode 자체를 사용자 정의 class/asset/plugin처럼 다루는 것"이다.

두 문제의 크기가 다르다.

## Recommended Stages

### Stage 1: Map owns concrete GameMode

현재 단계에서 추천하는 구조다.

```cpp
class Map
{
private:
    GameMode mGameMode;
};
```

이 방식의 장점은 다음과 같다.

- 빠르게 구현할 수 있다.
- map 저장/로드에 포함할 수 있다.
- editor UI를 붙이기 쉽다.
- 현재 목표에 충분하다.

### Stage 2: Split GameModeSettings if needed

설정 데이터가 커지면 실행 객체와 저장 데이터를 분리할 수 있다.

```cpp
class Map
{
private:
    GameModeSettings mGameModeSettings;
};

class GameMode
{
public:
    void SetupPlay(Map *map, const GameModeSettings &settings);
};
```

이 방식은 저장 데이터와 runtime 실행 객체의 책임이 더 명확하다.

### Stage 3: Dynamic GameMode class

실제로 사용자 정의 game mode가 필요해지면 class 기반 구조로 확장한다.

```cpp
class Map
{
private:
    std::string mGameModeClassName;
};
```

이 단계에서 reflection 생성, editor class picker, polymorphic serialization을 함께 설계한다.

## Current Recommendation

지금은 다음 순서로 진행하는 것이 적절하다.

1. `GameMode` setter들을 public으로 변경한다.
2. `GameMode::Serialize(Arch&)`를 추가한다.
3. `Map`에 `GameMode mGameMode`를 추가한다.
4. `Map::GetGameMode()`를 추가한다.
5. `Map::SerilaizeRawData()`에서 `mGameMode.Serialize(arch)`를 호출한다.
6. `EditorPlayMode`의 `GameMode mGameMode`를 제거한다.
7. `EditorPlayMode::Start()`에서 `map->GetGameMode().SetupPlay(map)`를 호출한다.
8. 이후 editor UI는 `map->GetGameMode().SetDefaultPlayerObjectPrefab(...)`처럼 현재 map의 `GameMode`를 수정한다.

이 구조는 현재 엔진의 단계에서 가장 단순하고, map 저장/로드와 editor workflow에도 자연스럽게 연결된다.

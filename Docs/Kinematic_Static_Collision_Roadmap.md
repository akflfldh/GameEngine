# Kinematic-Static Collision 처리 로드맵

## 문서 목적

이 문서는 Kinematic Body, 특히 플레이어 Character가 Static 지형과 충돌하면서 다음 동작을 수행하도록 만드는 과정을 단계별로 정리한다.

- 바닥을 통과하지 않는다.
- 벽을 통과하지 않는다.
- 바닥에 닿으면 낙하를 멈춘다.
- 벽에 비스듬히 접근하면 벽을 따라 이동한다.
- Dynamic Body와 충돌하면 Dynamic Body에는 물리 반응을 전달할 수 있다.
- 해결할 수 없는 Kinematic-Static 접촉이 일반 rigid-body solver를 방해하지 않는다.

최종 목표는 단순한 관통 보정이 아니라 `Sweep and Slide` 기반 Kinematic 이동을 구축하는 것이다. 다만 현재 엔진에 Sweep 기능이 없으므로, 먼저 이산적인 관통 보정 방식으로 동작을 안정화한 뒤 단계적으로 교체한다.

---

## 전체 구조 요약

```text
CharacterMovementComponent
    -> 입력으로 원하는 속도와 이동량 계산
    -> PhysicsBridgeSystem에 Kinematic 이동 요청

PhysicsBridgeSystem
    -> KinematicMoveCommand를 PhysicsScene에 전달

PhysicsScene
    -> Kinematic 이동 후보 위치 계산
    -> Static Shape와 충돌 검사
    -> 이동 허용 범위 계산
    -> Ground / Wall 판정
    -> 최종 위치와 속도 결정

PhysicsBridgeSystem
    -> 최종 Transform 및 KinematicMoveResult를 외부에 전달

CharacterMovementComponent
    -> Grounded, GroundNormal, Hit 결과를 다음 프레임 이동 계산에 사용
```

Rigid-body 충돌 solver와 Kinematic 이동 solver의 역할은 분리한다.

```text
Dynamic-Dynamic      -> 일반 rigid-body solver
Dynamic-Static       -> 일반 rigid-body solver
Dynamic-Kinematic    -> 일반 solver에서 Dynamic만 반응
Kinematic-Static     -> Kinematic 이동 solver
Kinematic-Kinematic  -> 기본적으로 이벤트 또는 별도 정책
Static-Static        -> 처리하지 않음
```

---

# 1단계. 충돌 조합 분류 확정

## 목표

해결할 수 없는 Kinematic-Static 접촉이 기존의 반복 충돌 solver에 들어가지 않도록 한다. 동시에 Kinematic-Static 충돌 감지 자체는 이후 전용 처리에서 사용할 수 있도록 유지한다.

## 기본 개념

Static과 Kinematic은 일반 solver 관점에서 모두 역질량이 0이다.

```cpp
inverseMass == 0.0f
```

두 Body 모두 solver가 이동시킬 수 없으므로 일반 관통 보정에 넣으면 다음 문제가 생긴다.

```text
Kinematic-Static 접촉 선택
    -> totalUnitMove == 0
    -> 관통 보정 실패
    -> penetration이 그대로 유지
    -> 같은 접촉이 반복 선택
    -> 다른 Dynamic 접촉이 처리되지 않음
```

따라서 충돌 감지와 충돌 반응 대상을 분리해야 한다.

## 세부 작업

1. Body 조합을 분류하는 함수를 정의한다.
2. Dynamic Body가 하나 이상 있는 접촉만 기존 rigid-body solver에 전달한다.
3. Kinematic-Static 접촉은 별도의 Kinematic 접촉 목록에 전달한다.
4. Static-Static 접촉은 조기에 제외한다.
5. Kinematic-Kinematic은 일단 이벤트만 생성하거나 제외한다.

```cpp
enum class ECollisionResponsePath
{
    eNone,
    eRigidBodySolver,
    eKinematicSolver
};
```

```cpp
ECollisionResponsePath ClassifyCollisionPair(
    EPhysicsBodyType bodyA,
    EPhysicsBodyType bodyB);
```

## 완료 조건

- Kinematic이 Static 바닥과 겹쳐도 Dynamic Body의 기존 바닥 충돌이 정상적으로 처리된다.
- Kinematic-Static 접촉은 일반 `CollisionResolution()`의 `contactList`에 들어가지 않는다.
- Kinematic-Static 충돌 정보는 별도 경로에서 확인할 수 있다.

---

# 2단계. Kinematic 전용 접촉 데이터 정의

## 목표

Kinematic 이동 보정에 필요한 최소 접촉 정보를 일반 `PhysicsContact`와 분리하여 표현한다.

## 기본 개념

일반 `PhysicsContact`는 질량, 관성, 반발, 마찰 임펄스 계산을 위한 데이터이다. Kinematic-Static 처리에서는 임펄스보다 다음 정보가 중요하다.

- 어떤 Kinematic Body가 충돌했는가.
- 어떤 Static Body 또는 Shape와 충돌했는가.
- Kinematic을 바깥으로 밀어낼 법선은 무엇인가.
- 관통 깊이는 얼마인가.
- 접촉 지점은 어디인가.
- 바닥인지 벽인지 판정할 수 있는가.

## 세부 작업

```cpp
struct KinematicContact
{
    PhysicsBodyHandle mKinematicBodyHandle;
    PhysicsShapeHandle mKinematicShapeHandle;
    PhysicsBodyHandle mOtherBodyHandle;
    PhysicsShapeHandle mOtherShapeHandle;

    CoreMath::Vector3 mPoint;
    CoreMath::Vector3 mNormalTowardKinematic;
    float mPenetration = 0.0f;
};
```

접촉 법선은 항상 Kinematic을 향하도록 통일한다.

현재 일반 접촉 법선이 `BodyB -> BodyA` 방향이라면 다음처럼 변환할 수 있다.

```cpp
if (contact.mBodyA == kinematicBody)
{
    normalTowardKinematic = contact.mNormal;
}
else
{
    normalTowardKinematic = -contact.mNormal;
}
```

PhysicsScene에는 최소한 프레임 단위 목록이 필요하다.

```cpp
std::vector<KinematicContact> mKinematicContactList;
```

이 목록은 프레임 시작 시 비우고 해당 프레임의 Kinematic 충돌 감지 결과만 수집한다.

## 완료 조건

- 모든 Kinematic 접촉 법선이 Kinematic 바깥 방향으로 일관되게 정렬된다.
- Kinematic Body Handle을 기준으로 해당 Body의 접촉만 찾을 수 있다.
- 일반 rigid-body manifold와 데이터가 섞이지 않는다.

---

# 3단계. 1차 Kinematic 이동 후보 계산

## 목표

외부에서 설정된 Kinematic 속도를 이용해 이번 프레임의 원하는 이동량과 후보 위치를 계산한다.

## 기본 개념

Kinematic은 힘으로 가속되지 않는다. MovementComponent가 계산한 속도를 물리엔진이 위치 변화량으로 변환한다.

```text
desiredDisplacement = velocity * deltaTime
candidatePosition = currentPosition + desiredDisplacement
```

후보 위치는 곧바로 최종 위치로 확정하지 않는다. Static 충돌을 검사하고 보정한 뒤 최종 위치를 결정해야 한다.

## 세부 작업

1. 현재 위치를 저장한다.
2. 속도로 원하는 이동량을 계산한다.
3. 후보 위치에 Body와 Shape를 배치한다.
4. 후보 위치에서 Kinematic-Static 충돌을 검사한다.

```cpp
struct KinematicMoveContext
{
    PhysicsBody* mBody = nullptr;
    CoreMath::Vector3 mStartPosition;
    CoreMath::Vector3 mDesiredDisplacement;
    CoreMath::Vector3 mCandidatePosition;
};
```

```cpp
context.mStartPosition = body.mPosition;
context.mDesiredDisplacement = body.mVelocity * deltaTime;
context.mCandidatePosition =
    context.mStartPosition + context.mDesiredDisplacement;
```

현재 단계에서는 이산 충돌 감지를 사용하므로 후보 위치에 실제 Body를 임시 배치한 뒤 Shape 월드 트랜스폼을 갱신할 수 있다.

```cpp
body.mPosition = context.mCandidatePosition;
RebuildShapesWorldTransformFromBody(body);
```

## 완료 조건

- Kinematic Body가 충돌하지 않을 때 기존 속도로 정상 이동한다.
- 후보 위치와 프레임 시작 위치를 모두 확인할 수 있다.
- 충돌 보정 전에는 외부 Transform 결과를 확정하지 않는다.

---

# 4단계. 이산 관통 보정 구현

## 목표

후보 위치에서 Static과 겹친 Kinematic을 접촉 법선 방향으로 밀어내어 바닥과 벽을 통과하지 않도록 한다.

## 기본 개념

Kinematic은 일반 solver가 이동시키지 않지만, Kinematic 전용 solver에서는 이동 명령을 충돌 가능한 범위로 제한해야 한다.

```text
correctedPosition = candidatePosition
                  + normalTowardKinematic * penetration
```

표면에 정확히 붙을 때 반복 관통이 발생할 수 있으므로 작은 `skinWidth`를 추가한다.

```cpp
constexpr float skinWidth = 0.001f;
```

## 세부 작업

```cpp
void ResolveKinematicPenetration(
    PhysicsBody& body,
    const KinematicContact& contact)
{
    body.mPosition +=
        contact.mNormalTowardKinematic *
        (contact.mPenetration + skinWidth);
}
```

여러 면에 동시에 접촉할 수 있으므로 한 번만 처리하지 않고 제한된 횟수만큼 반복한다.

```cpp
for (int iteration = 0; iteration < 4; ++iteration)
{
    RebuildShapesWorldTransformFromBody(body);

    std::vector<KinematicContact> contacts =
        DetectKinematicStaticContacts(body);

    if (contacts.empty())
        break;

    for (const KinematicContact& contact : contacts)
    {
        ResolveKinematicPenetration(body, contact);
    }
}
```

## 완료 조건

- 낮은 속도로 떨어지는 Kinematic이 Static 바닥 아래로 계속 내려가지 않는다.
- 벽과 바닥에 동시에 접촉해도 제한된 반복 횟수 안에 관통이 감소한다.
- 해결 실패 접촉이 무한 반복되지 않는다.
- 최종 위치가 변경되면 `mTransformSyncDirty`가 설정된다.

## 현재 단계의 한계

이 방식은 이동 후 겹침을 검사한다. 한 프레임에 얇은 벽을 완전히 지나가면 최종 위치에서 겹치지 않으므로 충돌을 놓칠 수 있다.

---

# 5단계. 충돌면 안쪽 속도 제거와 Slide

## 목표

관통 보정 후에도 다음 프레임에 같은 벽이나 바닥 안으로 계속 이동하지 않도록 속도를 수정한다. 벽에 비스듬히 접근하면 벽을 따라 이동하도록 한다.

## 기본 개념

속도는 접촉 법선 성분과 접선 성분으로 분해할 수 있다.

```text
velocity = normalVelocity + tangentVelocity
```

표면 안쪽을 향하는 법선 성분만 제거하면 접선 성분은 남기 때문에 Slide가 발생한다.

```cpp
float intoSurface =
    body.mVelocity.Dot(contact.mNormalTowardKinematic);

if (intoSurface < 0.0f)
{
    body.mVelocity -=
        contact.mNormalTowardKinematic * intoSurface;
}
```

## 세부 작업

1. 관통 보정 후 각 접촉의 법선 안쪽 속도를 제거한다.
2. 여러 접촉면이 있으면 반복적으로 각 법선 성분을 제거한다.
3. 최종 속도를 다음 프레임 MovementComponent가 조회할 수 있게 유지한다.
4. 벽에서 수직 속도까지 잘못 제거되지 않는지 확인한다.

예시:

```text
낙하 속도: (0, -5, 0)
바닥 법선: (0, 1, 0)
법선 안쪽 성분 제거 결과: (0, 0, 0)
```

```text
대각선 이동: (3, 0, 3)
벽 법선: (-1, 0, 0)
벽 안쪽 성분 제거 결과: (0, 0, 3)
```

## 완료 조건

- 바닥에 닿은 뒤 수직 속도가 매 프레임 계속 누적되지 않는다.
- 벽에 대각선으로 이동하면 완전히 정지하지 않고 벽을 따라 움직인다.
- 벽에서 멀어지는 속도 성분은 제거하지 않는다.

---

# 6단계. Grounded 판정과 Movement 결과 반환

## 목표

CharacterMovementComponent가 바닥 접촉 여부를 알고 중력, 점프, 공중 제어 정책을 결정할 수 있게 한다.

## 기본 개념

접촉 법선이 월드 Up 방향에 충분히 가까우면 이동 가능한 바닥으로 판정한다.

```text
dot(contactNormal, worldUp) >= cos(maxSlopeAngle)
```

벽 법선은 Up과 거의 수직이므로 Ground로 판정되지 않는다.

## 세부 작업

```cpp
struct KinematicMoveResult
{
    PhysicsBodyHandle mBodyHandle;
    CoreMath::Vector3 mPosition;
    CoreMath::Vector3 mVelocity;

    bool mGrounded = false;
    CoreMath::Vector3 mGroundNormal = {0, 1, 0};

    bool mHitWall = false;
    CoreMath::Vector3 mWallNormal;
};
```

```cpp
const CoreMath::Vector3 worldUp{0, 1, 0};
const float minGroundDot = cos(maxSlopeRadians);

float upAlignment =
    contact.mNormalTowardKinematic.Dot(worldUp);

if (upAlignment >= minGroundDot)
{
    result.mGrounded = true;
    result.mGroundNormal =
        contact.mNormalTowardKinematic;
}
```

MovementComponent는 결과를 이용해 수직 속도를 관리한다.

```cpp
if (moveResult.mGrounded && mVerticalVelocity < 0.0f)
{
    mVerticalVelocity = 0.0f;
}
```

## 완료 조건

- 평평한 바닥에서 `Grounded == true`가 된다.
- 벽에만 접촉했을 때는 `Grounded == false`이다.
- 지정한 최대 경사각보다 급한 면은 바닥으로 판정하지 않는다.
- 낙하 후 Grounded가 되면 수직 속도가 0으로 정리된다.

---

# 7단계. PhysicsBridge 명령과 결과 경로 정리

## 목표

Core의 CharacterMovementComponent가 Physics 내부 자료구조를 직접 알지 않도록 하고, 요청과 결과를 Bridge를 통해 전달한다.

## 기본 개념

현재 `SetKinematicVelocity()`는 속도만 전달한다. Kinematic 이동 처리 결과까지 필요해지므로 이동 요청과 결과를 명시적으로 표현하는 것이 좋다.

## 세부 작업

```cpp
struct KinematicMoveCommand
{
    PhysicsBodyHandle mBodyHandle;
    CoreMath::Vector3 mDesiredVelocity;
    float mDeltaTime = 0.0f;
};
```

또는 물리 Step이 공통 `deltaTime`을 사용한다면 명령에는 속도만 유지하고 PhysicsScene이 이동량을 계산할 수 있다.

Bridge가 유지해야 할 흐름:

```text
PrePhysicsUpdate
    -> KinematicMoveCommand enqueue

PhysicsScene::Step
    -> Kinematic 이동 처리
    -> KinematicMoveResult 생성

PostPhysicsUpdate
    -> Transform 결과를 SceneComponent에 반영
    -> Grounded 결과를 MovementComponent에 전달 또는 캐싱
```

MovementComponent 포인터를 Physics 모듈에 전달하지 않는다. Bridge가 `SceneComponent* <-> PhysicsBodyHandle` 매핑과 함께 최신 Kinematic 결과를 캐싱할 수 있다.

```cpp
const KinematicMoveResult* GetKinematicMoveResult(
    SceneComponent* component) const;
```

## 완료 조건

- Physics 모듈이 CharacterMovementComponent를 include하지 않는다.
- MovementComponent가 PhysicsBody 포인터에 직접 접근하지 않는다.
- 같은 프레임 또는 정책상 명확한 다음 프레임에 Grounded 결과를 읽을 수 있다.
- Body 삭제 시 결과 캐시와 Handle 매핑도 함께 제거된다.

---

# 8단계. Dynamic Body 밀기 정책

## 목표

Kinematic Character가 Dynamic Box와 충돌했을 때 Box가 움직이되, Character 이동이 무제한의 힘을 전달하지 않도록 정책을 정한다.

## 기본 개념

일반적인 Kinematic은 무한 질량처럼 작동한다. 따라서 Dynamic-Kinematic 접촉에서는 Dynamic Body만 임펄스를 받는다.

현재 rigid-body solver에 Dynamic-Kinematic 접촉을 유지하면 기본적인 밀기 동작은 가능하다. 다만 Kinematic 속도가 높으면 과도한 임펄스를 전달할 수 있다.

## 세부 작업

1. Dynamic-Kinematic 접촉은 일반 solver에 유지한다.
2. 유효질량 계산에서 Kinematic의 역질량과 역관성은 반드시 0으로 취급한다.
3. Kinematic의 속도는 상대속도 계산에는 포함한다.
4. Kinematic 자체에는 계산된 임펄스를 적용하지 않는다.
5. 필요하면 Character가 전달할 수 있는 최대 Push Impulse를 제한한다.

Kinematic과 Static의 관성 관련 값은 모두 0이어야 한다.

```cpp
body.mInertia = CoreMath::Matrix4X4::Zero;
body.mInertiaInverse = CoreMath::Matrix4X4::Zero;
body.mInertiaWorld = CoreMath::Matrix4X4::Zero;
body.mInertiaInverseWorld = CoreMath::Matrix4X4::Zero;
```

## 완료 조건

- Character가 Dynamic Box를 밀 수 있다.
- Dynamic Box가 Character를 충돌 임펄스로 밀거나 회전시키지 않는다.
- Kinematic의 관성값이 Dynamic 접촉 유효질량 계산에 섞이지 않는다.
- 빠른 Character 이동으로 Dynamic Body가 비정상적으로 발사되지 않는다.

---

# 9단계. Sweep and Slide로 교체

## 목표

이동 후 관통을 수정하는 방식에서 이동 경로의 최초 충돌 시점을 찾는 연속적인 이동 방식으로 교체한다.

## 기본 개념

이산 관통 보정은 시작 위치와 최종 위치 사이의 충돌을 놓칠 수 있다. Sweep은 Shape를 이동 경로 전체에 걸쳐 검사하고 최초 충돌 시간 `TOI(Time Of Impact)`를 구한다.

```text
TOI = 0.0 -> 시작하자마자 충돌
TOI = 0.5 -> 전체 이동량의 절반에서 충돌
TOI = 1.0 -> 이동 끝까지 충돌 없음
```

## 세부 작업

1. 현재 위치에서 원하는 이동량 방향으로 Shape Sweep을 수행한다.
2. 가장 작은 TOI를 가진 충돌을 선택한다.
3. 충돌 직전까지 이동한다.
4. Skin Width만큼 표면에서 떨어뜨린다.
5. 남은 이동량에서 표면 안쪽 법선 성분을 제거한다.
6. 남은 이동량으로 다시 Sweep한다.
7. 최대 반복 횟수에 도달하면 남은 이동을 버린다.

```cpp
CoreMath::Vector3 remainingDisplacement =
    desiredVelocity * deltaTime;

for (int iteration = 0; iteration < 4; ++iteration)
{
    KinematicSweepHit hit;

    if (!SweepKinematicBody(
            body,
            remainingDisplacement,
            hit))
    {
        body.mPosition += remainingDisplacement;
        break;
    }

    float safeTime = std::max(
        0.0f,
        hit.mTimeOfImpact - hit.mSkinTime);

    body.mPosition +=
        remainingDisplacement * safeTime;

    CoreMath::Vector3 remaining =
        remainingDisplacement *
        (1.0f - hit.mTimeOfImpact);

    float intoSurface =
        remaining.Dot(hit.mNormalTowardKinematic);

    if (intoSurface < 0.0f)
    {
        remaining -=
            hit.mNormalTowardKinematic * intoSurface;
    }

    remainingDisplacement = remaining;
}
```

초기 구현은 Box 또는 Capsule 한 종류부터 시작한다. Character 용도라면 Capsule Sweep을 최종 목표로 두는 편이 유리하다.

## 완료 조건

- 빠른 이동에서도 얇은 벽을 통과하지 않는다.
- 바닥과 벽의 모서리에서 안정적으로 Slide한다.
- Sweep 반복 횟수가 상한을 가진다.
- 시작부터 겹친 상태는 별도의 depenetration 경로로 복구한다.

---

# 10단계. Character 품질 기능 확장

## 목표

기본 충돌 이동이 안정화된 후 실제 플레이 가능한 Character 이동 기능을 추가한다.

## 세부 작업

### Ground Snap

작은 단차나 내리막에서 캐릭터가 매 프레임 공중 상태로 전환되지 않도록 아래 방향으로 짧은 Sweep을 수행한다.

### Step Up

낮은 장애물과 충돌했을 때 다음 세 이동을 시도한다.

```text
위로 이동
앞으로 이동
아래로 이동하여 바닥 찾기
```

### Slope Limit

최대 경사각보다 급한 면에서는 위로 이동하지 못하게 하고 아래 방향으로 미끄러지게 한다.

### Jump

Grounded 상태에서만 수직 속도를 점프 속도로 설정한다.

```cpp
if (mGrounded && jumpRequested)
{
    mVerticalVelocity = mJumpSpeed;
    mGrounded = false;
}
```

### Coyote Time 및 Jump Buffer

입력 품질 개선 단계에서 추가하며 기본 물리 충돌 구현과는 분리한다.

## 완료 조건

- 작은 단차와 내리막에서 Grounded가 불필요하게 깜빡이지 않는다.
- 설정한 높이 이하의 계단을 오를 수 있다.
- 급경사를 벽처럼 처리하거나 아래로 미끄러진다.
- 점프와 착지가 일관되게 동작한다.

---

# 권장 구현 우선순위

## 단기 안정화

```text
1. 충돌 조합 분류
2. Kinematic 접촉 데이터 분리
3. 이동 후보 위치 계산
4. 이산 관통 보정
5. 법선 안쪽 속도 제거
6. Grounded 결과 반환
```

이 단계까지 구현하면 낮은 속도의 Kinematic Character가 바닥과 벽을 통과하지 않고 기본 이동할 수 있다.

## 중기 구조 완성

```text
7. Bridge 명령/결과 구조 정리
8. Dynamic Body 밀기 정책 안정화
9. Sweep and Slide 구현
```

## 장기 플레이 품질

```text
10. Ground Snap
11. Step Up
12. Slope Limit
13. Jump 및 입력 보조 기능
```

---

# 테스트 체크리스트

## Kinematic-Static 기본 테스트

- Kinematic Box가 중력 속도로 Static 바닥에 떨어진다.
- Kinematic Box가 바닥 아래로 통과하지 않는다.
- Grounded가 true가 된다.
- 수직 속도가 0이 된다.
- Static 바닥의 Transform은 변하지 않는다.

## 벽 Slide 테스트

- Kinematic Box가 벽에 정면으로 이동하면 멈춘다.
- 벽에 대각선으로 이동하면 벽을 따라 움직인다.
- 벽에서 멀어지는 입력은 정상적으로 처리된다.

## Solver 격리 테스트

- Kinematic-Static 접촉 중에도 별도의 Dynamic Box가 Static 바닥 위에서 정상적으로 유지된다.
- Kinematic-Static 접촉이 일반 rigid-body solver 반복 횟수를 소비하지 않는다.
- Kinematic-Static 접촉에서 NaN 또는 Inf가 발생하지 않는다.

## Dynamic 밀기 테스트

- Kinematic Character가 Dynamic Box를 수평으로 민다.
- Dynamic Box만 임펄스 영향을 받는다.
- Character는 충돌로 회전하거나 밀리지 않는다.
- Dynamic Box가 Static 바닥 아래로 압착되어 통과하지 않는다.

## Sweep 전환 후 테스트

- 높은 속도에서도 얇은 Static 벽을 통과하지 않는다.
- 큰 `deltaTime`에서도 이동 결과가 finite하다.
- 모서리와 두 벽 사이에서 반복 횟수가 무한히 증가하지 않는다.

---

# 최종 설계 원칙

1. Kinematic은 힘과 충돌 임펄스로 이동하지 않는다.
2. Kinematic의 이동은 외부 요청으로 결정되지만, 실제 허용 이동량은 Kinematic 이동 solver가 계산한다.
3. Kinematic-Static 접촉은 일반 rigid-body solver에 넣지 않는다.
4. Dynamic-Kinematic 접촉에서는 Dynamic만 물리 반응을 받는다.
5. CharacterMovementComponent는 이동 의도를 계산하고 PhysicsScene은 충돌 가능한 최종 이동을 계산한다.
6. Physics 모듈은 Character 또는 MovementComponent를 직접 알지 않는다.
7. 초기 이산 관통 보정은 임시 단계이며 최종적으로 Sweep and Slide로 교체한다.

# DebugDraw를 직접 렌더 커맨드로 처리하는 이유

## 개요

에디터에서 사용하는 조명 범위, 카메라 프러스텀, 방향 화살표, 선택 보조선 같은 시각 요소는 일반적인 게임 오브젝트나 컴포넌트와 성격이 다르다.

이들은 씬에 존재하는 실제 객체가 아니라, 현재 프레임에서 에디터가 사용자에게 보여주기 위해 임시로 요청하는 시각 정보다. 따라서 `Object`, `Component`, `RenderProxy`를 영구적으로 생성하는 방식보다, 매 프레임 필요한 렌더 정보를 직접 구축해서 렌더 시스템에 전달하는 `DebugDraw` 방식이 더 적절하다.

## 일반 오브젝트/컴포넌트 방식의 문제

일반 오브젝트/컴포넌트 방식으로 라이트 반경, 카메라 프러스텀, 방향 화살표를 표현하면 다음 문제가 생긴다.

- 실제 씬 객체가 아닌데도 `Object`와 `Component` 생명주기에 들어간다.
- hierarchy, selection, serialization, prefab, map dirty 처리 대상인지 계속 구분해야 한다.
- 단순한 선 하나를 그리기 위해 컴포넌트 생성, 렌더 프록시 등록, 해제 과정이 필요해진다.
- 매 프레임 달라지는 임시 시각 정보를 영구 객체처럼 관리하게 된다.
- 에디터 전용 표시물이 런타임 게임 오브젝트 구조와 섞인다.
- 라이트 영역처럼 선택 대상이 아닌 정보까지 hit 대상, inspector 대상, 저장 대상에서 제외하는 추가 정책이 필요해진다.

즉, 표현하려는 정보는 가벼운 에디터 보조선인데, 처리 구조는 일반 렌더 오브젝트처럼 무거워진다.

## 직접 렌더 커맨드 방식의 핵심

`DebugDraw` 방식은 다음과 같이 동작한다.

```text
Editor / Component / Tool
    -> DrawLine(), DrawWireSphere(), DrawArrow() 요청
    -> RenderProxyManager 또는 DebugDrawManager가 임시 커맨드 수집
    -> RenderPipelineManager가 DebugLineRenderCommand로 변환
    -> DebugLineRenderPass가 한 번에 렌더링
    -> EndFrame에서 커맨드 제거
```

이 구조에서는 debug draw 요청이 오브젝트를 생성하지 않는다. 단지 이번 프레임에 그릴 선, 색상, depth test 여부 같은 렌더 정보만 남긴다.

## 이점

### 1. 생명주기가 단순하다

DebugDraw 데이터는 기본적으로 1프레임짜리 임시 데이터다.

```cpp
DrawLine(start, end, color);
```

이 호출은 다음 프레임까지 유지되는 오브젝트를 만들지 않는다. 렌더링이 끝나면 자동으로 사라진다. 따라서 `Destroy`, `UnRegisterRenderProxy`, `SetMap(nullptr)` 같은 생명주기 처리가 필요 없다.

### 2. 저장/로드와 분리된다

라이트 반경, 카메라 프러스텀, 방향 화살표는 에디터 표시물이지 씬 데이터가 아니다.

직접 렌더 커맨드 방식은 애초에 `Map`, `Object`, `Component`에 등록되지 않으므로 serialization, prefab, dirty/save 흐름과 자연스럽게 분리된다.

### 3. 에디터 기능과 런타임 객체가 섞이지 않는다

아이콘처럼 선택 가능한 대상은 `EditorVisualizerObject`로 두는 것이 맞다. 하지만 조명 반경이나 방향선은 선택 대상이 아니라 보조 시각 정보다.

따라서 다음처럼 역할을 나누는 것이 좋다.

```text
EditorVisualizerObject
    -> billboard icon
    -> EditorHitProxyComponent
    -> source component binding

DebugDraw
    -> light radius
    -> spot cone
    -> directional arrow
    -> camera frustum
    -> player start direction
```

이렇게 하면 클릭 가능한 대표 객체와 단순 시각 보조 정보가 명확히 분리된다.

### 4. 렌더링 비용을 줄이기 쉽다

선 기반 debug draw는 많은 선을 하나의 커맨드 리스트 또는 하나의 structured buffer로 모아 처리할 수 있다.

예를 들어 `DrawWireSphere()`가 내부적으로 64개의 선을 만들더라도, 최종적으로는 `DebugLineRenderPass`에서 한 번에 업로드하고 렌더링할 수 있다.

```text
DrawLine x N
    -> DebugLineRenderCommand list
    -> StructuredBuffer<DebugLineData>
    -> DrawInstanced(lineCount * 2)
```

반대로 컴포넌트/메시 방식은 각 표시물마다 렌더 프록시, mesh, material, object buffer 처리가 들어가기 쉽다.

### 5. 기능 확장이 쉽다

`DrawLine`만 안정적으로 만들면 그 위에 많은 기능을 올릴 수 있다.

```text
DrawLine
    -> DrawArrow
    -> DrawWireBox
    -> DrawWireSphere
    -> DrawWireCone
    -> DrawFrustum
    -> DrawCapsule
```

새로운 에디터 표시 기능을 추가할 때마다 새 컴포넌트나 새 메시를 만들 필요가 없다. 필요한 수학 계산으로 line segment만 생성하면 된다.

### 6. 선택 정책과 렌더 표시 정책을 분리할 수 있다

라이트 아이콘은 클릭 가능해야 하지만, 라이트 반경 sphere는 클릭 대상이 아닐 수 있다.

직접 렌더 커맨드 방식에서는 선택은 `EditorHitProxyComponent`가 담당하고, 표시는 `DebugDraw`가 담당한다.

```text
Hit
    -> EditorHitProxyComponent

Visual Guide
    -> DebugDraw
```

이 분리는 에디터 입력 시스템을 단순하게 유지하는 데 중요하다.

### 7. 플레이/에디터 모드 정책을 적용하기 쉽다

DebugDraw 요청은 에디터 모드에서만 호출하거나, 렌더 패스 등록 자체를 에디터 윈도우에서만 수행하도록 제한할 수 있다.

일반 오브젝트로 만들면 플레이 모드 전환 시 어떤 객체를 제거하고 유지할지 관리해야 하지만, DebugDraw 방식은 요청하지 않으면 그려지지 않는다.

## 현재 엔진에서의 권장 구조

현재 엔진에는 이미 다음 구조가 있다.

```text
IRenderProxyManager
ObjectRenderItemBuilder
RenderPipelineManager
RenderPassExecuteContext
RenderEditOverlayPass
```

따라서 DebugDraw는 다음 흐름으로 확장하는 것이 적절하다.

```text
IRenderProxyManager::DrawLine()
    -> ObjectRenderItemBuilder가 renderID별 DebugLineRenderCommand 수집
    -> RenderPipelineManager::CreateRenderCommands()에서 ExecuteContext로 복사
    -> DebugLineRenderPass에서 렌더링
    -> ObjectRenderItemBuilder::EndFrame()에서 임시 라인 제거
```

## 컴포넌트 방식이 적절한 경우

모든 표시를 DebugDraw로 처리해야 하는 것은 아니다. 다음 경우에는 오브젝트/컴포넌트 방식이 적절하다.

- 클릭 가능한 에디터 아이콘
- transform gizmo처럼 hover, drag, axis hit가 필요한 조작 도구
- 씬에 실제로 존재해야 하는 runtime object
- mesh/material/component 상태를 inspector에서 편집해야 하는 객체
- 저장되어야 하는 객체

따라서 카메라/라이트/플레이어스타트의 대표 아이콘은 `EditorVisualizerObject + BillboardComponent + EditorHitProxyComponent`로 두고, 방향선/범위/프러스텀은 `DebugDraw`로 그리는 구성이 가장 적절하다.

## 결론

DebugDraw는 일반 오브젝트를 대체하는 시스템이 아니라, 씬에 저장되지 않는 임시 시각 정보를 빠르고 명확하게 렌더링하기 위한 별도 경로다.

현재 엔진에서는 다음 원칙을 유지하는 것이 좋다.

- 클릭 가능한 대표 표시는 `EditorVisualizerObject`로 처리한다.
- 선택 판정은 `EditorHitProxyComponent`가 담당한다.
- 조명 범위, 방향 화살표, 카메라 프러스텀 같은 보조선은 `DebugDraw`로 처리한다.
- `DrawLine`을 가장 기본 primitive로 만들고, 나머지 도형은 `DrawLine` 조합으로 구현한다.
- DebugDraw 데이터는 매 프레임 임시 커맨드로 수집하고 `EndFrame`에서 제거한다.


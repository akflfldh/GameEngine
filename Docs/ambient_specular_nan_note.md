# Shader Specular NaN 문제 정리

## 문제 상황

`DefaultStaticMeshHLSL`의 specular 계산 중 아래 코드에서 `NaN`이 발생했다.

```hlsl
float m = (1.0f - gRoughness) * 255.0f;

float3 sr = ((m + 8) / 8) * pow(max(dot(normal, halfwayVector), 0.0f), m);
```

PIX 디버깅 결과 `m == 0`일 때 `sr` 값이 다음처럼 깨졌다.

```text
sr = { x = -nan(ind), y = -nan(ind), z = -nan(ind) }
```

## 원인

`gRoughness == 1.0f`이면 다음과 같이 된다.

```hlsl
float m = (1.0f - 1.0f) * 255.0f;
// m == 0
```

이 상태에서 `dot(normal, halfwayVector)`가 `0` 이하이면:

```hlsl
pow(0, 0)
```

이 발생한다.

`pow(0, 0)`은 수학적으로 정의가 애매하고, GPU/HLSL 환경에서는 `NaN`이 나올 수 있다.

## 핵심 문제

문제가 되는 조합은 다음이다.

```text
roughness == 1.0
shininess == 0
NdotH == 0
pow(0, 0)
```

즉 roughness가 최대값일 때 specular exponent가 0이 되고, 표면과 half vector가 수직 또는 반대 방향이면 `pow(0, 0)`이 된다.

## 권장 수정안

`m` 값을 최소 `1.0f` 이상으로 clamp한다.

```hlsl
float roughness = saturate(gRoughness);
float shininess = max((1.0f - roughness) * 255.0f, 1.0f);

float ndoth = saturate(dot(normal, halfwayVector));
float3 sr = ((shininess + 8.0f) / 8.0f) * pow(ndoth, shininess);
```

## 왜 이 방식이 적절한가

`ndoth`를 `0.0001f` 같은 값으로 올리는 방식도 가능하다.

```hlsl
float ndoth = max(dot(normal, halfwayVector), 0.0001f);
```

하지만 이 방식은 원래 specular가 `0`이어야 하는 각도에서도 아주 작은 specular가 생긴다.

반면 `shininess`만 최소값으로 보정하면:

```text
ndoth == 0 -> pow(0, 1) == 0
```

이 되어 물리적으로도 자연스럽고 `NaN`도 방지할 수 있다.

## 최종 권장 코드

```hlsl
float roughness = saturate(gRoughness);
float shininess = max((1.0f - roughness) * 255.0f, 1.0f);

float ndoth = saturate(dot(normal, halfwayVector));
float3 sr = ((shininess + 8.0f) / 8.0f) * pow(ndoth, shininess);
```

## 추가 권장 사항

`gRoughness`는 CPU에서 `0.0f ~ 1.0f`로 clamp하더라도 shader에서도 한 번 더 `saturate()`를 적용하는 것이 안전하다.

```hlsl
float roughness = saturate(gRoughness);
```

Shader는 잘못된 material 값, 미초기화 값, import된 값의 예외 상황까지 방어해야 하므로 GPU 쪽에서도 최소한의 안정성 처리를 넣는 것이 좋다.

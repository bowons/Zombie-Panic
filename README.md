# Zombie-Panic

Unreal Engine 5 기반의 간단한 FPS 게임 프로젝트입니다.  
플레이어는 제한된 탄창과 주무기와 보조무기를 활용해 지정된 라운드만큼의 좀비 웨이브를 버텨야 합니다.

본 프로젝트는 FPS 클라이언트의 시스템 전반을 직접 설계·구현하며, 객체지향적 코드 구조를 생각하면서 **확장성과 유지보수성**을 공부하는 데 중점을 두었습니다.

---

## ZombiePanic 동영상
[![Zombie-Panic](https://img.youtube.com/vi/md8x9uarLVs/0.jpg)](https://youtu.be/md8x9uarLVs)
---

## 주요 구현 기능

### FPS 플레이어 시스템
- `FPSCharacterBase` 기반의 **플레이어/좀비 상속 구조**
  - Character 객체 처리의 유연성을 위해 상속 구조 구현
  - 코드 중복 최소화 및 유지보수성 강화를 위해 노력
- 사격, 조준(ADS), 재장전, 무기 교체 등 FPS 핵심 동작 구현
- **탄퍼짐(Spread)**: `FMath::VRandCone()`을 이용해 총구 방향에 원뿔 범위 랜덤성 부여
- **반동(Recoil)**: `FRandRange()` 기반의 상하/좌우 반동 계산 후 카메라에 적용

### 무기 시스템
- `AWeaponBase` -> `Rifle`, `Pistol` 등으로 확장되는 구조
- 탄환 발사, 피격 판정, 데미지 전달 로직을 모듈화하여 독립성 확보
- 캐릭터는 Fire만 누르면 처리할 수 있도록 구현하여 캡슐화

### 좀비 AI
- `ZombieAIController` + Behavior Tree를 활용하여 Unreal 엔진의 기능을 유기적으로 사용하여 구현 부담을 최소화
  - Patrol, Attack, CanAttack 등 개별 Task/Service로 분리
  - AI 행동 알고리즘 교체·확장이 용이
- 추적 및 근접 공격 동작 구현, 웨이브 단위 스폰 및 난이도 조정 가능

### UI / UX
- `PlayerHUDInterface`를 통해 이벤트 발생시 HUD에 적용
  - 체력/탄약 변화가 HUD에 실시간 반영
  - 게임 로직과 UI 분리를 통해 유지보수성 강화 - 인터페이스를 이용한 느슨한 결합 유지
- **Fab SmoothDynamicCrossHair 플러그인** 적용하여 개발 시간 단축
  - 사격/이동 상황에 따라 크로스헤어가 동적으로 변화
 
### 에셋 및 애니메이션
- 외부 에셋(캐릭터/무기)을 Blender로 직접 수정, 구매 에셋은 리타게팅하여 프로젝트에 최적화
- State Machine & Montage 기반의 자연스러운 이동/공격/재장전 애니메이션 블렌딩
- 스켈레톤 호환성 문제 및 블렌드 공간 조정 문제를 직접 해결

---

## 기술 스택
- **Engine**: Unreal Engine 5 (C++ & Blueprint 혼합)
- **Language**: C++
- **Plugins**: Fab SmoothDynamicCrossHair
- **Tools**: Blender (구매 에셋 수정)

---

## 느낀 점
Zombie-Panic 프로젝트를 통해, 코드 구현 경험을 넘어 실제 개발 환경에서 마주칠 수도 있는 문제들을 경험해본 것 같습니다.
- 외부 에셋을 프로젝트에 맞게 변형하거나, Blender를 통해 직접 수정하고, 또 Fab에서 구매한 Plugin을 이용해 개발 시간을 단축해보았습니다. 이를 통해, 제가 개발하지 않은 리소스들을 활용하는 경험을 얻었습니다.
- 최대한 객체지향을 활용해 확장성과 유지보수를 염두에 두고 개발하려 노력했지만 원하는 대로 되지는 않았던 것 같습니다.


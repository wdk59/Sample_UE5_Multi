# UE 5.8 멀티플레이 샘플 사용 설명서

## 1. 목적

이 프로젝트는 언리얼 네트워크 개발의 다음 기본 흐름을 작은 예제로 비교하기 위한 샘플이다.

- 로컬에서만 실행되는 효과
- 클라이언트가 소유한 Actor를 통한 Server RPC
- 서버에서 접속자들에게 전달하는 NetMulticast RPC
- GameState의 프로퍼티 복제와 RepNotify
- 로컬 UI와 서버 권위 상태의 연결

이 샘플은 개념 입문용이다. 아래의 "본 프로젝트 적용 전 보완 사항"을 처리하지 않은 채 실제 게임 구조로 그대로 복사하지 않는다.

## 2. 주요 파일과 역할

| 파일 | 역할 |
|---|---|
| `Sample_MultiCharacter` | 클라이언트가 소유하는 Server RPC 진입점 |
| `InteractionActorBase` | Overlap 감지와 로컬/서버 경로 분기 |
| `LocalInteractionActor` | 감지한 로컬 플레이어에게만 VFX 재생 |
| `GolobalInteractionActor` | 서버 확인 후 Multicast로 접속자에게 VFX 재생 |
| `Sample_MultiGameState` | 모든 플레이어가 관찰하는 `GlobalText` 복제 |
| `Sample_MultiPlayerController` | 로컬 플레이어에게만 샘플 UI 생성 |
| `Sample_MultiWidget` | 버튼 입력을 Character RPC로 전달하고 복제 결과 표시 |

관련 Blueprint는 `Content/MyContents` 아래에 있다.

- `InteractionObjects/Blueprints/BP_LocalInteractionActor`
- `InteractionObjects/Blueprints/BP_GlobalInterationActor`
- `Widgets/BP_Sample_MultiWidget`

## 3. 실행 준비

1. Unreal Editor에서 `Sample_Multi.uproject`를 연다.
2. `BP_ThirdPersonGameMode`에 `Sample_MultiGameState` 계열 클래스가 Game State Class로 지정되어 있는지 확인한다.
3. 사용 중인 PlayerController Blueprint에 `BP_Sample_MultiWidget`이 `Sample Multi Widget Class`로 지정되어 있는지 확인한다.
4. `Lvl_ThirdPerson`에 로컬/전역 상호작용 Blueprint가 배치되어 있는지 확인한다.
5. 각 상호작용 Blueprint의 `VFX` 속성에 원하는 Niagara System이 지정되어 있는지 확인한다.

## 4. PIE 멀티플레이 테스트

에디터의 Play 설정을 다음처럼 구성한다.

- Number of Players: 2 이상
- Net Mode: Play As Listen Server
- Run Under One Process: 한 Output Log에서 비교하려면 활성화

각 창에서 캐릭터를 움직여 상호작용 Actor의 충돌 범위에 진입한다.

### 로컬 상호작용 예상 결과

- 진입한 플레이어의 창에서만 VFX가 보인다.
- 다른 플레이어 창에는 같은 VFX가 나타나지 않는다.
- 서버 상태를 변경하지 않는 순수 로컬 연출의 예다.

### 전역 상호작용 예상 결과

- 소유 Character가 Server RPC를 전송한다.
- 서버의 전역 Actor가 Authority를 확인한다.
- 서버가 NetMulticast RPC를 호출한다.
- 서버와 관련성 있는 클라이언트가 VFX를 재생한다.

### 공유 텍스트 예상 결과

- 어느 플레이어가 버튼을 눌러도 요청은 Character의 Server RPC로 이동한다.
- 서버가 GameState의 `GlobalText`를 변경한다.
- 각 클라이언트의 RepNotify가 델리게이트를 Broadcast한다.
- 로컬 Widget이 최신 텍스트로 갱신된다.

## 5. 로그 읽는 법

VFX 로그에는 다음 값이 출력된다.

- `PlayerID`: 서버가 현재 게임 안에서 플레이어에 할당한 식별자
- `PIEInstanceID`: PIE 창을 구분하기 위한 에디터 전용 번호

`PIEInstanceID`는 패키징 게임의 계정 또는 플레이어 ID가 아니다. 영구 사용자 식별에는 Online Subsystem의 Unique Net ID 같은 별도 체계가 필요하다.

테스트할 때 함께 확인하면 좋은 값은 다음과 같다.

- `GetNetMode()`: Standalone, Client, Listen Server, Dedicated Server 구분
- `HasAuthority()`: 해당 Actor 인스턴스에 서버 권위가 있는지
- `IsLocallyControlled()`: 해당 Pawn을 현재 프로세스의 로컬 플레이어가 조종하는지
- `GetLocalRole()`: Actor의 로컬 네트워크 역할

Listen Server의 호스트 Character는 `HasAuthority()`와 `IsLocallyControlled()`가 동시에 참일 수 있다.

## 6. 코드 흐름 따라 읽기

전역 VFX는 다음 순서로 읽는다.

```text
InteractionActorBase::OnInteractionBeginOverlap
  -> Character::TryInteract
  -> Character::Server_Interact_Implementation
  -> GolobalInteractionActor::Interact
  -> Multicast_PlayVFX_Implementation
  -> InteractionActorBase::PlayVFX
```

복제 텍스트는 다음 순서로 읽는다.

```text
Widget::OnChangeTextButtonClicked
  -> Character::Server_SetGlobalText_Implementation
  -> GameState::SetGlobalText
  -> GlobalText replication
  -> GameState::OnRep_GlobalText
  -> Widget::OnGlobalTextChanged
```

## 7. 현재 샘플의 평가

RPC 방향, Actor 소유권, 로컬 실행, Multicast와 RepNotify의 차이를 처음 배우기에는 유용하다. 하지만 이 샘플만 보고 실제 멀티플레이 전체를 구현하기에는 부족하다.

본 프로젝트 적용 전에 최소한 다음 항목을 별도 예제로 추가하거나 팀 규칙으로 정해야 한다.

1. 서버 검증: RPC 대상 거리, Overlap, 쿨다운, 권한, 현재 상태를 서버에서 재검증한다.
2. 상태 설계: 지속 상태는 replicated property, 순간 알림은 RPC로 구분한다.
3. PlayerState 수명 주기: 클라이언트에서는 BeginPlay보다 늦게 도착할 수 있어 `OnRep_PlayerState`를 고려한다.
4. Client RPC: 특정 소유 클라이언트 한 명에게 보내는 응답 예제가 없다.
5. 접속 흐름: 로그인, PlayerState 생성, Possess, 재접속, Seamless Travel 예제가 없다.
6. 최적화: Relevancy, Cull Distance, Dormancy, Update Frequency, replication condition 예제가 없다.
7. 이동/예측: CharacterMovement의 예측과 보정, 커스텀 이동 복제 예제가 없다.
8. 세션: 호스트 생성, 검색, 참가, 종료 및 Online Subsystem 연동이 없다.
9. Dedicated Server: Listen Server와 전용 서버의 차이를 검증하는 테스트가 없다.
10. 자동 테스트: 다중 클라이언트에서 결과를 검증하는 네트워크 테스트가 없다.

## 8. 현재 코드에서 알아둘 사항

- `InteractionActorBase::Server_Interact`는 소유권 실패를 설명하기 위한 비교용 함수이며 정상 경로에서 사용하지 않는다.
- `Character::Server_Interact`는 TargetActor의 형식과 서버의 실제 Overlap 상태를 다시 검증한다. 실제 게임에서는 여기에 쿨다운, 캐릭터 상태, 팀, 사용 횟수 같은 규칙도 추가한다.
- 클래스/에셋 이름의 `Golobal`, `Interation` 오타는 참조를 안전하게 이전한 뒤 정리하는 것이 좋다.
- 일회성 VFX Multicast는 `Unreliable`이다. 반드시 보여야 하거나 게임 상태를 결정하는 정보는 프로퍼티 복제로 표현한다.

## 9. 팀 권장 학습 순서

1. 로컬 Actor에 들어가 한 창에만 VFX가 보이는지 확인한다.
2. 전역 Actor에 들어가 RPC 호출 순서와 각 창의 로그를 비교한다.
3. UI 버튼으로 RepNotify가 지속 상태를 동기화하는 과정을 확인한다.
4. 서버 검증 코드를 추가해 잘못된 거리의 요청을 거부해 본다.
5. 특정 플레이어에게만 결과를 보내는 Client RPC 예제를 추가한다.
6. Dedicated Server PIE 또는 별도 서버 프로세스로 같은 테스트를 반복한다.

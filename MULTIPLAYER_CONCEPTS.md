# 멀티플레이 보충 개념

## 서버 권위

게임 결과를 결정하는 코드는 서버에서 실행한다. 클라이언트는 이동, 공격, 상호작용의 의도를 요청할 수 있지만 성공 여부를 확정해서는 안 된다. 서버는 요청을 받으면 거리, 상태, 비용, 쿨다운과 대상 유효성을 다시 검사한다.

`HasAuthority()`는 "이 코드가 서버 프로세스인가"보다 "이 Actor 인스턴스에 권위가 있는가"를 묻는 표현이다. Listen Server의 로컬 플레이어는 서버 권위와 로컬 제어를 동시에 가질 수 있다.

## Actor 소유권과 RPC

- Server RPC: 해당 Actor를 소유한 클라이언트가 서버로 호출한다.
- Client RPC: 서버가 해당 Actor의 소유 클라이언트 한 명에게 호출한다.
- NetMulticast RPC: 서버가 호출하면 현재 관련성 있는 클라이언트들에게 전달된다.

레벨에 배치된 일반 Actor는 보통 서버 소유다. 클라이언트 요청은 PlayerController, 소유 Pawn/Character처럼 그 클라이언트가 소유한 Actor를 통해 서버로 보내는 것이 기본이다.

RPC 매개변수로 Actor 포인터를 보낼 수 있다고 해서 그 요청을 신뢰해도 된다는 뜻은 아니다. 서버가 대상 Actor와 호출자 사이의 관계를 검증해야 한다.

## RPC와 프로퍼티 복제 선택

RPC는 사건을 전달하고, 프로퍼티 복제는 최신 상태를 전달한다.

- 총구 섬광, 짧은 사운드: RPC 후보
- 문 개폐 상태, 체력, 점수, 팀: replicated property 후보
- 늦게 참가한 플레이어도 알아야 하는 값: replicated property
- 특정 소유자에게만 알려야 하는 결과: Client RPC 또는 owner-only property

Multicast는 과거 기록이 아니다. 호출 이후 접속한 클라이언트는 이전 Multicast를 받지 못한다.

## RepNotify

`ReplicatedUsing=OnRep_X`는 클라이언트가 새 값을 받았을 때 표현 계층을 갱신하기 좋다. 서버에서는 OnRep가 자동 호출되지 않으므로 Listen Server UI에도 같은 갱신 함수가 필요하면 Setter에서 공통 함수를 호출한다.

복제 프로퍼티는 `GetLifetimeReplicatedProps`와 `DOREPLIFETIME` 또는 적절한 조건 매크로에 등록해야 한다.

## GameMode, GameState, PlayerState

- GameMode: 서버에만 존재하며 규칙, 스폰, 승패를 결정한다.
- GameState: 서버가 관리하고 모든 클라이언트가 관찰할 경기 전체 상태를 둔다.
- PlayerState: 플레이어 이름, 점수, 팀처럼 Pawn이 바뀌어도 유지할 플레이어 상태를 둔다.
- PlayerController: 서버와 소유 클라이언트에 존재한다. 다른 클라이언트의 PlayerController는 일반적으로 보이지 않는다.
- Pawn/Character: 월드에서 조종되는 실체다. 사망과 재스폰으로 교체될 수 있다.

영구적인 플레이어 식별을 `PlayerId`에 의존하지 않는다. `PlayerId`는 현재 게임 세션의 임시 식별에 적합하다.

## 복제 시점

클라이언트에서 Actor의 BeginPlay가 호출됐다고 해서 모든 참조가 이미 복제됐다는 보장은 없다. PlayerState가 필요한 초기화는 `OnRep_PlayerState`, GameState 값은 해당 RepNotify처럼 데이터 도착 이벤트에 맞추는 편이 안전하다. 모든 프레임에서 유효할 때까지 검색하는 방식은 피한다.

## 관련성과 성능

서버의 모든 Actor를 모든 클라이언트에 항상 복제하면 확장되지 않는다. 다음 개념을 함께 설계한다.

- Net Relevancy와 Net Cull Distance
- Net Update Frequency
- Dormancy
- Owner Only 및 Skip Owner 조건
- RPC 호출 빈도와 Reliable 큐
- Fast Array Serializer 같은 대량 목록 복제

`Reliable`은 중요하다는 의미이지 빠르다는 의미가 아니다. 손실 시 재전송되고 순서를 보장하므로 빈번한 연출 이벤트에 남용하면 뒤의 중요한 RPC까지 지연시킬 수 있다.

## 이동 예측과 보정

CharacterMovementComponent는 클라이언트 예측과 서버 보정을 이미 제공한다. 일반 위치를 매 Tick Server RPC로 보내는 방식으로 대체하지 않는다. 대시나 특수 이동을 추가할 때는 이동 모드, 저장 이동 데이터, 서버 검증 및 보정 영향을 함께 검토한다.

## UI 원칙

Widget은 로컬 표현 객체이며 자체적으로 네트워크 복제되지 않는다. UI 입력은 소유 PlayerController나 Pawn을 통해 서버에 요청하고, UI 출력은 GameState/PlayerState 등 복제된 모델을 관찰해 갱신한다.

```text
Widget input -> owned Actor -> Server RPC -> authoritative state
authoritative state -> property replication -> OnRep/delegate -> Widget output
```

## 테스트 체크리스트

- Listen Server 호스트와 원격 클라이언트에서 결과가 같은가?
- Dedicated Server에서도 UI 없는 서버 코드가 정상인가?
- 지연과 패킷 손실을 시뮬레이션해도 상태가 회복되는가?
- 늦게 참가한 클라이언트가 최신 상태를 받는가?
- 연결 종료와 재접속 후 PlayerState와 Possess가 정상인가?
- 악의적인 거리, 대상, RPC 빈도 요청을 서버가 거부하는가?
- 두 플레이어가 동시에 같은 대상을 조작해도 결과가 결정적인가?

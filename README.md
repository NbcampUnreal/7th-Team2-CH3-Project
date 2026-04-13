# Exodus

> 3인칭 소울 액션 슈팅 게임 | Unreal Engine 5 / C++

##프로젝트 개요
> 장르 : 3인칭 액션 슈팅
> 플랫폼 : PC
> 팀 규모 : 5명
> 기간 : 4주
> 담당 : 몬스터 AI 시스템

## 담당 역할 - Monster AI System
### Behavior Tree 기반 4단계 AI 설계
> 순찰 -> 탐자 -> 전투 -> 사망 상태 흐름 구현
> BT Service 노드로 탐지 로직 구현 -> 매 프레임 체크 대신 일정 간격으로만 거리 계산 (Tick 최소화)
> Simple Parallel 활용 -> 이동과 감지 태스크 병렬 실행으로 AI 움직임 자연스럽게 개선
> Blackboard 기반 상태값 중앙 관리 -> AI <> GamePlay 시스템 결합도 최소화

### 팀 협업
> 데미지 인터페이스 설계 협의
> 사망 처리 , 이펙트 트리거 연동
> 파트별 독립 개발 가능한 구조로 분리


## 기술 스택
'Unreal Engine 5', 'C++', 'Behavior Tree', 'Blackboard', 'Simple Parallel'



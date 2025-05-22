# CPU 스케줄링 시뮬레이터

다양한 CPU 스케줄링 알고리즘을 시뮬레이션하고 성능을 비교하는 프로젝트입니다.

## 기능

- 구현된 CPU 스케줄링 알고리즘
    - FCFS
    - SJF
    - Preemptive SJF
    - Priority
    - Preemptive Priority
    - RR
- 프로세스 정보를 TSV 파일에서 로드하거나 시뮬레이터 실행 중 실시간으로 추가 가능
- I/O 큐 구현
- 각 스케줄링 알고리즘의 평균 Turnaround Time 및 Waiting Time 측정
- Gantt 차트 및 실행 히스토리 시각화
- 벤치마킹 도구를 사용하여 여러 시나리오에서 스케줄러 성능 비교

## 빌드

- 개발 빌드
    ```sh
    make dev
    ```
- 릴리스 빌드
    ```sh
    make build
    ```

바이너리는 `bin/sim` 경로에 생성됩니다.

## 실행

```sh
bin/sim [옵션]
```

### 옵션

- `--scheduler=<scheduler_name>`: 사용할 스케줄러를 미리 지정합니다.
    - `fcfs`, `sjf`, `psjf`, `priority`, `ppriority`, `rr`
- `--tsv=<file_path>`: 프로세스 정보가 담긴 TSV 파일 경로를 지정합니다. (기본값: `data/processes.tsv`)
- `--non-interactive`: 모든 프로세스가 완료될 때까지 자동으로 시뮬레이션하고 종료됩니다.
- `--rr-time-quantum=<time>`: Round Robin 스케줄러가 사용되는 경우 Time Quantum 값을 미리 지정합니다.

### 대화형 모드

`--non-interactive` 플래그 없이 실행하면 대화형 모드로 시뮬레이터를 실행합니다. 대화형 모드에서는 각 틱에 다음 명령어를 사용할 수 있습니다.

- `a` (add): 현재 틱을 `arrived_at`으로 한 새 프로세스를 추가합니다.
- `t` (tick): 시뮬레이션을 한 틱 진행합니다.
- `f` (finish): 모든 프로세스가 완료될 때까지 시뮬레이션을 진행합니다.
- `s` (status): 현재 실행 중인 프로세스, 스케줄러 상태, I/O 큐 상태를 출력합니다.
- `h` (history): 현재 틱까지의 시뮬레이션 히스토리를 출력합니다.
- `g` (gantt): Gantt 차트를 출력합니다.
- `q` (quit): 시뮬레이터를 종료하고 통계를 출력합니다.

## 벤치마킹 도구

`tools/benchmark`에 스케줄링 알고리즘 간 성능을 비교하기 위한 Node.js 스크립트가 포함되어 있습니다.

### 결과 예시

```js
{
  fcfs: { turnaround: 4683.78, waiting: 4153.06 },
  sjf: { turnaround: 2811.79, waiting: 2280.27 },
  psjf: { turnaround: 2735.04, waiting: 2204.4 },
  priority: { turnaround: 3993.06, waiting: 3463.24 },
  ppriority: { turnaround: 3964.46, waiting: 3434.48 },
  'rr (rr-time-quantum=10)': { turnaround: 5076.53, waiting: 4547.25 },
  'rr (rr-time-quantum=20)': { turnaround: 5093.89, waiting: 4564.65 },
  'rr (rr-time-quantum=30)': { turnaround: 5100.89, waiting: 4571.26 },
  'rr (rr-time-quantum=40)': { turnaround: 5122.45, waiting: 4593.06 },
  'rr (rr-time-quantum=50)': { turnaround: 5122.74, waiting: 4593.03 }
}
```

### 사용법

1. `cd tools/benchmark`
2. `yarn install`
3. `yarn generate`
  - `../../data/__generated__/`에 100개의 TSV 파일 (100개의 '세션')을 생성합니다.
  - 한 개의 세션에서는 30개의 프로세스를 스케줄링합니다.
  - 각 프로세스는 임의의 `arrived_at`, `priority`, `cpu_burst`, `io_burst`, `io_request_time`을 가집니다.
4. `yarn benchmark`
  - 미리 정의된 스케줄링 옵션으로, 각 스케줄러별로 동일한 100개의 세션을 실행합니다.
  - 스케줄러별 평균 Turnaround Time과 Waiting Time을 계산합니다.

## 프로젝트 구조

- `src/`: C 소스 코드
  - `main.c`: 프로그램 진입점, 사용자 입력 처리
  - `scheduler_*.c`: 각 스케줄링 알고리즘 구현
  - `executor.c`: 시뮬레이션의 핵심 로직 (틱 실행, 프로세스 상태 변경 등)
  - `controller.c`: 사용자 명령어 처리 및 시뮬레이터 제어
  - `process.c`, `process_queue.c`: 프로세스 및 프로세스 큐 관련 자료구조 및 함수
  - `tsv.c`: TSV 파일 파싱
  - `stats.c`: 통계 정보 관리
  - `logger.c`: 로그 출력
  - `history.c`: 실행 히스토리 및 Gantt 차트 데이터 관리
  - `utils.c`: 사용자 입력 핸들링 등 유틸리티 함수
- `include/`: C 헤더 파일
- `data/`: TSV 파일 (프로세스 데이터)
  - `processes.tsv.template`: TSV 파일 형식 예시
- `tools/benchmark/`: Node.js 벤치마킹 도구

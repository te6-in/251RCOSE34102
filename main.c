#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int pid;
  // int priority;
  int arrival;
  int cpu_burst;
  // int io_burst;
  // int io_request;

  int remaining_cpu_burst;
} Process;

typedef struct ProcessNode {
  Process p;
  struct ProcessNode *next;
} ProcessNode;

static ProcessNode *head = NULL;
static ProcessNode *tail = NULL;

static void enqueue(Process p) {
  ProcessNode *node = (ProcessNode *)malloc(sizeof(ProcessNode));

  if (!node) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  node->p = p;
  node->next = NULL;

  if (!tail) {
    head = tail = node;

    return;
  }

  tail->next = node;
  tail = node;
}

static int dequeue(Process *out) {
  if (!head) // queue is empty
    return 0;

  ProcessNode *tmp = head;
  *out = head->p;
  head = head->next;

  if (!head)
    tail = NULL; // queue is now empty

  free(tmp);

  return 1; // success
}

static int is_queue_empty(void) { return head == NULL; }

static int get_positive_int(const char *prompt) {
  char buffer[16];
  int value;

  while (1) {
    printf("%s", prompt);

    if (!fgets(buffer, sizeof(buffer), stdin)) {
      perror("fgets");
      exit(EXIT_FAILURE);
    }

    if (sscanf(buffer, "%d", &value) == 1 && value > 0)
      return value;

    puts("양의 정수를 입력해 주세요.");
  }
}

static void end_simulator(int current_time) {
  printf("\n시뮬레이터 종료 시각: %d\n", current_time);

  exit(0);
}

int main(void) {
  int current_time = 0;
  int pid_counter = 1;
  Process running;

  printf("FCFS\n");

  while (1) {
    char input[8];
    char choice;

    while (1) {
      printf("\nTime %d   — 프로세스를 추가할까요? (y/n/q): ", current_time);

      if (!fgets(input, sizeof(input), stdin)) // EOF
        end_simulator(current_time);

      choice = input[0];

      if (choice == 'y' || choice == 'Y' || choice == 'n' || choice == 'N' ||
          choice == 'q' || choice == 'Q')
        break;

      puts("    y, n, q 중 하나를 입력해 주세요.");
    }

    if (choice == 'q' || choice == 'Q')
      end_simulator(current_time);

    if (choice == 'y' || choice == 'Y') {
      int burst = get_positive_int("    CPU burst: ");

      Process p = {pid_counter++, current_time, burst, burst};
      enqueue(p);

      printf("    ⇒ 프로세스 %d 추가됨 (burst %d)\n", p.pid, burst);
    }

    // 현재 실행 중인 프로세스가 없음
    if (running.remaining_cpu_burst <= 0) {
      // 대기 큐 없음
      if (is_queue_empty()) {
        printf("Time %d~%d — CPU idle\n", current_time, current_time + 1);
        current_time++;

        continue;
      }

      dequeue(&running);
    }

    // 실행할 프로세스 있음

    if (running.remaining_cpu_burst == running.cpu_burst) {
      printf("Time %d   — 프로세스 %d 실행 시작 (burst %d 예정)\n",
             current_time, running.pid, running.remaining_cpu_burst);
    }

    running.remaining_cpu_burst--;

    printf("Time %d~%d — 프로세스 %d 실행  (burst %d 남음)\n", current_time,
           current_time + 1, running.pid, running.remaining_cpu_burst);

    if (running.remaining_cpu_burst == 0) {
      printf("Time   %d — 프로세스 %d 종료 (burst %d 완료)\n", current_time + 1,
             running.pid, running.cpu_burst);
    }

    current_time++;
  }

  end_simulator(current_time);
}

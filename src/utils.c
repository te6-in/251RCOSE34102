#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int get_positive_int(const char *prompt) {
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

    printf("양의 정수를 입력해 주세요.");
  }
}

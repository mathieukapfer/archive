#include "JitterMonitor.h"
#include <unistd.h>

int main(int argc, char *argv[]) {

  JitterMonitor jitter("test_jitter", 100);

  for (int i = 0; i < 100; i++) {
    jitter.ComputeJitterStart();
    usleep(i * 1000);
    jitter.ComputeJitter();
  }

  for (int i = 100; i > 0; i--) {
    jitter.ComputeJitterStart();
    usleep(i * 1000);
    jitter.ComputeJitter();
  }

  jitter.DisplayJitter();

  return 0;
}

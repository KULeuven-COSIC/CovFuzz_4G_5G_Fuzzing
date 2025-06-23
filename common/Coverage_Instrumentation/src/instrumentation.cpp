#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "instrumentation.h"

struct feedback_t feedback = {
    .guard_count = 1,
    .triggered_edges = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER
};

#ifdef __cplusplus 
extern "C" {
#endif
void __sanitizer_cov_trace_pc_guard_init(uint32_t *start, uint32_t *stop) {
  printf("__sanitizer_cov_trace_pc_guard_init\n");
  memset(feedback.guard_map, 0, sizeof(feedback.guard_map)/sizeof(feedback.guard_map[0]));
  static uint64_t N;  // Counter for the guards.
  if (start == stop || *start) return;  // Initialize only once.
  printf("INIT: %p %p\n", start, stop);
  for (uint32_t *x = start; x < stop; x++) {
    *x = ++N;  // Guards should start from 1.
    feedback.guard_count++;
  }
  printf("Total edges: %" PRIu64 "\n", feedback.guard_count);
}

void __sanitizer_cov_trace_pc_guard(uint32_t *guard) {
  if (!*guard) return;  // Duplicate the guard check.
  // If you set *guard to 0 this code will not be called again for this edge.
  // Now you can get the PC and do whatever you want:
  //   store it somewhere or symbolize it and print right away.
  // The values of `*guard` are as you set them in
  // __sanitizer_cov_trace_pc_guard_init and so you can make them consecutive
  // and use them to dereference an array or a bit vector.
  // void *PC = __builtin_return_address(0);
  // char PcDescr[1024];
  // This function is a part of the sanitizer run-time.
  // To use it, link with AddressSanitizer or other sanitizer.
  // __sanitizer_symbolize_pc(PC, "%p %F %L", PcDescr, sizeof(PcDescr));
  // printf("guard: %p %x PC %s\n", guard, *guard, PcDescr);
  pthread_mutex_lock(&feedback.mutex);
  feedback.guard_map[*guard]++;
  feedback.triggered_edges++;
  pthread_mutex_unlock(&feedback.mutex);
}

#ifdef __cplusplus 
}
#endif

void print_total_triggered_edges() {
  uint64_t counter = 0;
  for (uint64_t i = 0; i < feedback.guard_count; i++) {
    if (feedback.guard_map[i]) counter++;
  }
  printf("%" PRIu64 " edges were executed from %" PRIu64 "\n", counter, feedback.guard_count);
}

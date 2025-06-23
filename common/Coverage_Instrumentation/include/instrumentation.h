#ifndef _INSTR_H_
#define _INSTR_H_

#include <stdint.h>
#include <pthread.h>

#define MAX_EDGES 200000

struct feedback_t {
    uint8_t guard_map[MAX_EDGES];
    uint64_t guard_count ;
    uint64_t triggered_edges;
    pthread_mutex_t mutex;
};

#ifdef __cplusplus 
extern "C" {
#endif

void __sanitizer_cov_trace_pc_guard_init(uint32_t *start, uint32_t *stop);
void __sanitizer_cov_trace_pc_guard(uint32_t *guard);

#ifdef __cplusplus 
}
#endif

void print_total_triggered_edges(void);

#endif //_INSTR_H_
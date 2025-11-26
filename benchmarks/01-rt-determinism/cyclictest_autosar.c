/*
 * Real-Time Determinism Test for AUTOSAR Classic
 * Uses OS Alarm callbacks (EB tresos style)
 */

#include <stdio.h>
#include <stdint.h>
#include "Os.h"
#include "EcuM.h"

#define TEST_ITERATIONS 1000000
#define INTERVAL_TICKS 1000  // Assuming 1µs tick

static uint64_t latencies[TEST_ITERATIONS];
static int iteration = 0;
static TickType last_tick = 0;

/* AUTOSAR Alarm Callback (ISR Category 2) */
ALARMCALLBACK(RtTestAlarm) {
    TickType current_tick;
    GetAlarm(RtTestAlarm, &current_tick);
    
    if (last_tick > 0) {
        TickType delta = current_tick - last_tick;
        int64_t jitter = (int64_t)delta - INTERVAL_TICKS;
        latencies[iteration++] = (jitter > 0) ? jitter : 0;
    }
    
    last_tick = current_tick;
    
    if (iteration >= TEST_ITERATIONS) {
        TerminateApplication(0, NO_RESTART);
    }
}

TASK(RtTestTask) {
    printf("=== AUTOSAR RT Determinism Test ===\n");
    
    /* Set relative alarm with 1kHz cycle */
    SetRelAlarm(RtTestAlarm, 1, INTERVAL_TICKS);
    
    /* Wait for completion (sleep task) */
    while (iteration < TEST_ITERATIONS) {
        WaitEvent(RtTestEvent);
        ClearEvent(RtTestEvent);
    }
    
    /* Calculate stats */
    uint64_t min = UINT64_MAX, max = 0, sum = 0;
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        if (latencies[i] < min) min = latencies[i];
        if (latencies[i] > max) max = latencies[i];
        sum += latencies[i];
    }
    uint64_t avg = sum / TEST_ITERATIONS;
    
    printf("Results:\n");
    printf("  Min:    %lu µs\n", min);
    printf("  Avg:    %lu µs\n", avg);
    printf("  Max:    %lu µs\n", max);
    
    /* Save to file (using stdio on AUTOSAR Adaptive) */
    FILE *fp = fopen("/data/rt_autosar.csv", "w");
    if (fp) {
        fprintf(fp, "iteration,latency_us\n");
        for (int i = 0; i < TEST_ITERATIONS; i++) {
            fprintf(fp, "%d,%lu\n", i, latencies[i]);
        }
        fclose(fp);
    }
    
    TerminateTask();
}

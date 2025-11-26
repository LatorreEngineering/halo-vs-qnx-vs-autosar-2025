/*
 * Real-Time Determinism Test for QNX 8.0
 * Uses QNX timer_create() with SIGEV_PULSE
 */

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <sched.h>

#define TEST_ITERATIONS 1000000
#define INTERVAL_NS 1000000  // 1ms = 1,000,000ns

static uint64_t latencies[TEST_ITERATIONS];
static int iteration = 0;

static inline uint64_t get_cycles(void) {
    uint64_t cycles;
    asm volatile("mrs %0, cntvct_el0" : "=r" (cycles));
    return cycles;
}

int main(void) {
    printf("=== QNX RT Determinism Test ===\n");
    
    /* Set real-time priority (requires root) */
    struct sched_param param;
    param.sched_priority = 255;  // QNX max priority
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
    
    /* Create channel for timer pulses */
    int chid = ChannelCreate(0);
    int coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, 0);
    
    /* Create timer */
    struct sigevent event;
    SIGEV_PULSE_INIT(&event, coid, SIGEV_PULSE_PRIO_INHERIT, 1, 0);
    
    timer_t timer_id;
    timer_create(CLOCK_MONOTONIC, &event, &timer_id);
    
    /* Arm timer (periodic 1kHz) */
    struct itimerspec itime;
    itime.it_value.tv_sec = 0;
    itime.it_value.tv_nsec = INTERVAL_NS;
    itime.it_interval.tv_sec = 0;
    itime.it_interval.tv_nsec = INTERVAL_NS;
    timer_settime(timer_id, 0, &itime, NULL);
    
    /* Measure */
    uint64_t last_cycles = get_cycles();
    struct _pulse pulse;
    
    while (iteration < TEST_ITERATIONS) {
        MsgReceive(chid, &pulse, sizeof(pulse), NULL);
        
        uint64_t now_cycles = get_cycles();
        uint64_t delta_cycles = now_cycles - last_cycles;
        
        /* Convert to µs (assuming 1GHz counter) */
        uint64_t latency_us = (delta_cycles / 1000) - 1000;
        latencies[iteration++] = latency_us;
        
        last_cycles = now_cycles;
    }
    
    /* Cleanup */
    timer_delete(timer_id);
    ConnectDetach(coid);
    ChannelDestroy(chid);
    
    /* Stats */
    uint64_t min = UINT64_MAX, max = 0, sum = 0;
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        if (latencies[i] < min) min = latencies[i];
        if (latencies[i] > max) max = latencies[i];
        sum += latencies[i];
    }
    
    printf("Results:\n");
    printf("  Min:    %lu µs\n", min);
    printf("  Avg:    %lu µs\n", sum / TEST_ITERATIONS);
    printf("  Max:    %lu µs\n", max);
    
    /* Save CSV */
    FILE *fp = fopen("/tmp/rt_qnx.csv", "w");
    if (fp) {
        fprintf(fp, "iteration,latency_us\n");
        for (int i = 0; i < TEST_ITERATIONS; i++) {
            fprintf(fp, "%d,%lu\n", i, latencies[i]);
        }
        fclose(fp);
        printf("✓ Saved to /tmp/rt_qnx.csv\n");
    }
    
    /* Verdict */
    if (max < 10) {
        printf("✓ EXCELLENT: Suitable for ASIL-D (max < 10µs)\n");
    } else if (max < 50) {
        printf("✓ GOOD: Suitable for ADAS\n");
    } else {
        printf("⚠ Unexpected jitter for QNX: %luµs\n", max);
    }
    
    return 0;
}

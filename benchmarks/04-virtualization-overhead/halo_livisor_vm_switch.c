```c
/*
 * Halo OS LiVisor VM Switch Benchmark
 * Measures context switch time between RT and Linux VMs
 */

#include 
#include 
#include 
#include <livisor/livisor.h>

#define TEST_ITERATIONS 100000

static inline uint64_t rdtsc(void) {
    uint64_t cycles;
    asm volatile("mrs %0, cntvct_el0" : "=r" (cycles));
    return cycles;
}

int main(void) {
    printf("=== Halo OS LiVisor VM Switch Benchmark ===\n");
    
    /* Initialize LiVisor */
    livisor_init();
    
    /* Create two VMs: RTOS (vCPU 0) and Linux (vCPU 1) */
    livisor_vm_t *vm_rtos = livisor_create_vm(LIVISOR_VM_TYPE_RTOS, 0);
    livisor_vm_t *vm_linux = livisor_create_vm(LIVISOR_VM_TYPE_LINUX, 1);
    
    uint64_t total_cycles = 0;
    
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        uint64_t start = rdtsc();
        
        /* Switch from RTOS to Linux VM */
        livisor_switch_vm(vm_rtos, vm_linux);
        
        /* Switch back */
        livisor_switch_vm(vm_linux, vm_rtos);
        
        uint64_t end = rdtsc();
        total_cycles += (end - start);
    }
    
    /* Convert to microseconds (assume 2 GHz CPU) */
    uint64_t avg_cycles = total_cycles / TEST_ITERATIONS;
    double avg_us = (double)avg_cycles / 2000.0;
    
    printf("Results:\n");
    printf("  Iterations: %d\n", TEST_ITERATIONS);
    printf("  Avg cycles: %lu\n", avg_cycles);
    printf("  Avg time:   %.2f µs\n", avg_us);
    
    if (avg_us < 15) {
        printf("✓ PASS: Low overhead (< 15µs)\n");
    } else {
        printf("⚠ Higher than expected: %.2fµs\n", avg_us);
    }
    
    return 0;
}
```

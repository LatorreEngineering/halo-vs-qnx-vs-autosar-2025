# Benchmark Methodology

## Overview
This document describes the rigorous methodology used to ensure fair, reproducible comparisons between Halo OS, QNX+Linux, and AUTOSAR.

## Test Environment Standardization

### Hardware Consistency
- **Single board per test:** Infineon AURIX TC397 r1.2
- **Identical peripherals:** Same Ethernet PHY, same CAN transceiver
- **Controlled temperature:** 25°C ± 2°C (lab environment)
- **Stable power:** Linear bench supply (not switching)

### Software Configuration
- **Minimal builds:** Only benchmark code + minimal OS
- **No debug symbols:** Release builds with optimization (-O2)
- **Same compiler family where possible:** GCC 12.x

### Load Conditions
- **Idle baseline:** Measure with no other tasks running
- **Synthetic load:** Add 50% CPU load for stress tests
- **Consistent I/O:** Same data rates across stacks

## Measurement Techniques

### Timing
- **Hardware timestamping:** Use CPU cycle counter (CCNT) when available
- **External verification:** Logic analyzer for sub-microsecond accuracy
- **Statistics:** Min/Avg/P99/Max over 1M+ samples

### Data Collection
- **Raw samples:** Store all measurements, not just aggregates
- **Outlier handling:** Flag but don't discard >3σ events
- **Timestamps:** Include wall-clock time for correlation

## Reproducibility

Every test can be reproduced using:
1. Scripts in `benchmarks/` directory
2. Hardware described in `HARDWARE_TEST_GUIDE.md`
3. Exact firmware versions logged in `results/metadata.json`

## Bias Mitigation

- **Randomized test order:** Run OSes in random sequence
- **Multiple runs:** Average across 3+ trials
- **Third-party verification:** Welcome community reproductions

## Limitations

- **Single hardware platform:** Results may vary on other SoCs
- **Lab conditions:** Production vehicles face more variability
- **Synthetic workloads:** Real-world apps may behave differently

See individual benchmark READMEs for test-specific details.

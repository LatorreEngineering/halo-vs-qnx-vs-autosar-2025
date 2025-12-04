```bash
#!/bin/bash
# Run all RT determinism tests

set -e

echo "=== RT Determinism Benchmark ==="
echo ""

# Compile all tests
make clean
make all

# Run on QEMU or hardware (detect from env)
if [ "$RUN_MODE" = "hardware" ]; then
    echo "Running on hardware..."
    # Flash and run via JTAG
    t32marm -c ../../../boards/infineon-tc397/halo/config.t32 -s run_tests.cmm
else
    echo "Running in QEMU..."
    qemu-system-aarch64 -machine virt -cpu cortex-a72 -m 512M \
        -kernel cyclictest_halo -nographic
fi

echo ""
echo "✓ Tests complete. Results in ../../results/2025-11-benchmarks/"
```

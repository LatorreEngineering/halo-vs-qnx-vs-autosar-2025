#!/bin/bash
# Run all benchmarks in QEMU (no hardware needed)
# Estimated time: 15-20 minutes

set -e

echo "========================================="
echo "Halo OS vs QNX vs AUTOSAR Benchmark Suite"
echo "Running in QEMU (aarch64-rt)"
echo "========================================="
echo ""

# Create results directory
mkdir -p results/2025-11-benchmarks
timestamp=$(date +%Y%m%d_%H%M%S)
logfile="results/2025-11-benchmarks/run_${timestamp}.log"

echo "Log file: $logfile"
echo ""

# 1. RT Determinism
echo "[1/5] Running RT Determinism Tests..."
cd benchmarks/01-rt-determinism
./run_all.sh | tee -a ../../$logfile
cd ../..

# 2. Communication Latency
echo "[2/5] Running Communication Latency Tests..."
cd benchmarks/02-comms-latency
python3 measure_e2e.py | tee -a ../../$logfile
cd ../..

# 3. Memory Footprint
echo "[3/5] Running Memory Footprint Analysis..."
cd benchmarks/03-memory-footprint
./build_all.sh | tee -a ../../$logfile
python3 compare_sizes.py | tee -a ../../$logfile
cd ../..

# 4. Virtualization Overhead
echo "[4/5] Running Virtualization Tests..."
cd benchmarks/04-virtualization-overhead
./run_vm_bench.sh | tee -a ../../$logfile
cd ../..

# 5. Crypto Performance
echo "[5/5] Running Crypto Performance Tests..."
cd benchmarks/05-crypto-performance
python3 plot_crypto.py | tee -a ../../$logfile
cd ../..

echo ""
echo "========================================="
echo "✓ All benchmarks complete!"
echo "Results saved to: results/2025-11-benchmarks/"
echo "========================================="

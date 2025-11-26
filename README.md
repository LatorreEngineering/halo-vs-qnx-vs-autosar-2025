# Halo OS vs QNX+Linux vs AUTOSAR — Independent 2025 Benchmarks

> **The first fully independent, reproducible, head-to-head comparison** of Li Auto's open-source Halo OS (StarRing OS) against the two dominant automotive stacks: **QNX+Linux** and **AUTOSAR Classic/Adaptive**.

**Published:** November 2025  
**Author:** Raul Latorre Fortes
**License:** Apache 2.0  
**GitHub:** https://github.com/LatorreEngineering/halo-os-vs-qnx-vs-autosar-2025  
**Previous Article:** [LinkedIn Analysis](https://www.linkedin.com/feed/update/urn:li:activity:7397228392372256768/)
**Article for this repo:** [LinkedIn Analysis](I need to include the link when ready)
---

## Why This Exists

Li Auto claims Halo OS delivers:
- **2× faster response, 5× better stability** than AUTOSAR
- **<1 ms cross-domain sensor sharing**
- **80% less virtualization overhead**
- **4–8× faster cryptography**
- **4-week SoC adaptation** (vs 3–6 months)

**No one has independently verified this — until now.**

This repository contains **complete, runnable benchmarks** on real hardware (Infineon AURIX TC397, STM32H7, QEMU) and the raw data behind our comprehensive analysis.

---

## Current Results (Month year; I will include results when ready,WiP)
 
| Metric                          | Halo OS       | QNX+Linux     | AUTOSAR       | Winner        |
|---------------------------------|---------------|---------------|---------------|---------------|
| **Interrupt latency (worst)**   | tbm µs        | tbm µs        | tbm µs        | tbd           |   
| **E2E sensor→actuator**         | tbm ms        | tbm ms        | tbm ms        |  tbd          |
| **Memory footprint (full)**     | tbm MB        | tbm MB        | tbm MB        |  tbd          |
| **Crypto (AES-256-GCM)**        | tbm GB/s      | tbm GB/s      | tbm GB/s      |  tbd          |
| **VM switch overhead**          | tbm µs        | tbm µs        | N/A           |  tbd          |
| **Cost/License**                | Free          | $100K+        | $50K+         |  tbd          |
| **Safety Certification**        | ASIL-B (WIP)  | ASIL-D        | ASIL-D        | ✓ QNX/AUTOSAR |
| **Ecosystem Maturity**          | Nascent       | Mature        | Mature        | ✓ QNX/AUTOSAR |

**Full results:** [/results/2025-11-benchmarks/](results/2025-11-benchmarks/)

---

## Quick Start (No Hardware Needed)

```bash
# Clone and run all benchmarks in QEMU
git clone https://github.com/LatorreEngineering/halo-os-vs-qnx-vs-autosar-2025.git
cd halo-os-vs-qnx-vs-autosar-2025
chmod +x run_all_qemu.sh
./run_all_qemu.sh

# Results will be generated in results/2025-11-benchmarks/
```

**Estimated time:** 15–20 minutes on modern hardware

---

## Benchmark Suites

### 1. Real-Time Determinism (`01-rt-determinism`)
- **cyclictest** on all three stacks
- Measures worst-case interrupt latency and jitter
- **Key finding:** tbd

### 2. Communication Latency (`02-comms-latency`)
- End-to-end sensor→actuator using:
  - Halo: **VBSLite** (Fast DDS + MVBS)
  - AUTOSAR: **SOME/IP** (Vector stack)
  - QNX: **PPS** (Persistent Publish/Subscribe)
- **Key finding:** tbd

### 3. Memory Footprint (`03-memory-footprint`)
- Boot-to-idle RAM/Flash consumption
- Linker map analysis
- **Key finding:** tbd

### 4. Virtualization Overhead (`04-virtualization-overhead`)
- Mixed-criticality VM switching
- Halo LiVisor vs QNX Hypervisor
- **Key finding:** tbd

### 5. Crypto Performance (`05-crypto-performance`)
- AES-256-GCM throughput using HW accelerators
- **Key finding:** tbd

---

## Hardware Test Guide

See **[docs/HARDWARE_TEST_GUIDE.md](docs/HARDWARE_TEST_GUIDE.md)** for detailed instructions using:
- **Infineon AURIX TC397** Development Kit
- **Lauterbach TRACE32** debugger
- **Saleae Logic Pro 16** logic analyzer

---

## Repository Structure

```
.
halo-os-vs-qnx-vs-autosar-2025/
├── README.md
├── LICENSE
├── .gitignore
├── run_all_qemu.sh
├── .github/workflows/
│   ├── ci-benchmark.yml
│   └── mirror-to-gitee.yml
├── boards/              # Hardware configs (TC397, STM32H7, QEMU)
│   ├── infineon-tc397/
│   │   ├── halo/
│   │   ├── autosar/
│   │   └── qnx/
│   ├── qemu-aarch64-rt/
│   └── stm32h7-mcu/
├── benchmarks/
│   ├── 01-rt-determinism/
│   ├── 02-comms-latency/
│   ├── 03-memory-footprint/
│   ├── 04-virtualization-overhead/
│   └── 05-crypto-performance/
├── benchmarks/          # 5 benchmark suites (C code + Python analysis)         
├── docs/                # Detailed guides and methodology
├── integrations/        # Eclipse SCORE + VBSLite transport
├── results/             # Raw CSVs and plots
└── tools/               # Trace viewer, manifest fixes
```

---

## Contributing

We welcome contributions! To add new benchmarks or platforms:

1. Fork this repository
2. Create a feature branch: `git checkout -b my-new-benchmark`
3. Add your code under `benchmarks/06-your-test/`
4. Run CI: `git push origin my-new-benchmark`
5. Open a Pull Request

---

## Citation

If you use this work in research or publications, please cite:

```bibtex
@misc{latorre2025halo,
  title={Halo OS vs QNX vs AUTOSAR: Independent 2025 Benchmarks},
  author={Latorre, Raúl},
  year={2025},
  month={November},
  url={https://github.com/LatorreEngineering/halo-os-vs-qnx-vs-autosar-2025}
}
```

---

## License

Apache License 2.0 — See [LICENSE](LICENSE) for details.

**You are free to:** fork, modify, and publish your own benchmarks.

---

## Acknowledgments

- **Li Auto** for open-sourcing Halo OS
- **BlackBerry QNX** and **AUTOSAR consortium** for reference documentation
- **Eclipse SCORE** community for SDV architecture guidance

---

## Contact

- **Issues:** [GitHub Issues](https://github.com/LatorreEngineering/halo-os-vs-qnx-vs-autosar-2025/issues)
- **LinkedIn:** [Raúl Latorre](https://www.linkedin.com/in/raul-latorre-fortes-631b7130/)
- **Mirror (China):** [Gitee](https://gitee.com/latorre-engineering/halo-os-vs-qnx-vs-autosar-2025)

---

**Last Updated:** November 26, 2025
```

---

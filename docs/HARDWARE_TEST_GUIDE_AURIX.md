# Detailed Hardware Test Guide — Infineon AURIX TC397

**Last Updated:** November 26, 2025  
**Platform:** Infineon AURIX TC397 Development Kit (KIT_AURIX_TC397_TFT)  
**Target Stacks:** Halo OS (VCOS + VBSLite), AUTOSAR (EB tresos), QNX 8.0 SDP

---

## 📋 Table of Contents

1. [Required Hardware](#required-hardware)
2. [Software Prerequisites](#software-prerequisites)
3. [Initial Board Setup](#initial-board-setup)
4. [Flashing Instructions](#flashing-instructions)
5. [Measurement Setup](#measurement-setup)
6. [Running Benchmarks](#running-benchmarks)
7. [Data Collection](#data-collection)
8. [Expected Results](#expected-results)
9. [Troubleshooting](#troubleshooting)

---

## 1. Required Hardware

### Primary Board
- **Infineon AURIX TC397 Development Kit** (KIT_AURIX_TC397_TFT)
  - TriCore 1.8 GHz CPU with 6 cores
  - 8 MB Program Flash, 1.5 MB Data Flash
  - 6 MB SRAM
  - Ethernet 100/1000 Mbps
  - CAN FD, FlexRay, LIN interfaces
  - **Purchase:** ~$500 from Mouser/Digi-Key
  - **Datasheet:** [TC397 User Manual](https://www.infineon.com/cms/en/product/microcontroller/32-bit-tricore-tm-microcontroller/)

### Debug & Measurement Tools
- **Lauterbach TRACE32 PowerDebug X50** ($3K-$5K)
  - With LA-7765 license for AURIX TC3xx
  - JTAG cable included
  - **Alternative (budget):** Infineon DAP mini JTAG (~$150, limited features)

- **Saleae Logic Pro 16** Logic Analyzer ($1,500)
  - 100 MHz sample rate required for <1ms measurements
  - 16 digital channels
  - **Alternative:** Analog Discovery 2 ($300, 100 MS/s)

- **Oscilloscope** (optional but recommended)
  - Rigol DS1054Z or better (100 MHz, 4 channels)
  - For verifying GPIO toggle timing

### Additional Hardware
- **2× Automotive Ethernet PHY Boards**
  - Marvell 88Q2112 (100Base-T1 / 1000Base-T1)
  - For VBSLite/SOME/IP testing
  - **Example:** OPEN Alliance EVK (~$200 each)

- **CAN Transceiver Module**
  - TJA1050 or MCP2551
  - For legacy ECU integration tests

- **12V Bench Power Supply**
  - Min 3A output
  - Automotive voltage range (9-16V)

- **USB-to-Serial Adapter** (FTDI FT232RL)
  - For UART debug console

---

## 2. Software Prerequisites

### Development Tools

#### For Halo OS
```bash
# Install Halo OS SDK (from Gitee)
git clone https://gitee.com/li-auto/halos.git
cd halos
./install_sdk.sh

# Dependencies (Ubuntu 22.04)
sudo apt install gcc-arm-none-eabi gcc-12 cmake ninja-build
pip3 install west pyyaml
```

#### For AUTOSAR
- **EB tresos Studio 2024.2** (Elektrobit)
  - Requires license (~$10K/year enterprise, free 30-day trial)
  - Download: https://www.elektrobit.com/tresos
- **HighTec TriCore GCC 12.1.1** (free edition)
  - Download: https://free-entry-toolchain.hightec-rt.com

#### For QNX
- **QNX SDP 8.0** (BlackBerry)
  - Requires license (~$5K perpetual, 30-day eval available)
  - Download: https://www.qnx.com/download
- **QNX Momentics IDE** (included in SDP)

### Debugger Software
- **Lauterbach TRACE32 R.2024.11** (latest)
  - Install from CD/DVD or download portal
  - Ensure TC3xx family pack is installed

### Measurement Software
- **Saleae Logic 2** (free)
  - Download: https://www.saleae.com/downloads
- **Python 3.10+** with libraries:
```bash
pip3 install pandas matplotlib numpy scipy
```

---

## 3. Initial Board Setup

### Physical Connections
1. **Power**
   - Connect 12V to X1 power jack (center-positive)
   - Verify D1 LED (green) lights up

2. **JTAG**
   - Connect Lauterbach debugger to X2 JTAG header (20-pin)
   - Pinout: Standard ARM Cortex debug (JTAG/SWD)

3. **UART Console**
   - Connect FTDI to X5 UART header
   - Pins: GND, RX (P14.0), TX (P14.1)
   - Baud rate: 115200 8N1

4. **Ethernet**
   - Connect PHY board to X7 RGMII pins
   - See `boards/infineon-tc397/hardware/ethernet_wiring.pdf`

5. **GPIO Measurement**
   - Logic analyzer CH0 → P20.0 (publisher trigger)
   - Logic analyzer CH1 → P20.1 (subscriber actuator)
   - Common ground to X8-GND

### Software Setup
```bash
# Clone this benchmark repository
git clone https://github.com/LatorreEngineering/halo-os-vs-qnx-vs-autosar-2025.git
cd halo-os-vs-qnx-vs-autosar-2025

# Verify toolchains
arm-none-eabi-gcc --version  # Should be ≥12.0
tricore-gcc --version        # HighTec 12.1.1
qcc --version                # QNX 8.0

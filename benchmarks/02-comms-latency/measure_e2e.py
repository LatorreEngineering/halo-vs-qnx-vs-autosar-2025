#!/usr/bin/env python3
"""
End-to-End Latency Measurement
Analyzes Logic Analyzer CSV captures to validate <1ms claim
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import argparse
from pathlib import Path

def parse_logic_csv(filepath, ch0_name='Channel 0', ch1_name='Channel 1'):
    """Parse Saleae Logic CSV export"""
    df = pd.read_csv(filepath)
    
    # Find rising edges on CH0 (publisher) and CH1 (subscriber)
    ch0_edges = df[df[ch0_name] == 1]['Time [s]'].values
    ch1_edges = df[df[ch1_name] == 1]['Time [s]'].values
    
    # Match edges (find closest CH1 edge for each CH0 edge)
    latencies = []
    for t0 in ch0_edges:
        # Find first CH1 edge after this CH0 edge
        later_edges = ch1_edges[ch1_edges > t0]
        if len(later_edges) > 0:
            t1 = later_edges[0]
            latency_us = (t1 - t0) * 1e6  # Convert to microseconds
            latencies.append(latency_us)
    
    return np.array(latencies)

def analyze_latency(latencies, platform_name, threshold_us=1000):
    """Analyze and visualize latency data"""
    
    print(f"\n=== {platform_name} E2E Latency Analysis ===")
    print(f"Samples: {len(latencies)}")
    print(f"Min:     {latencies.min():.2f} µs")
    print(f"Avg:     {latencies.mean():.2f} µs")
    print(f"Median:  {np.median(latencies):.2f} µs")
    print(f"P99:     {np.percentile(latencies, 99):.2f} µs")
    print(f"Max:     {latencies.max():.2f} µs")
    print(f"StdDev:  {latencies.std():.2f} µs")
    
    # Verdict on <1ms claim
    avg_latency = latencies.mean()
    if avg_latency < threshold_us:
        print(f"\n✓ PASS: Validates <{threshold_us}µs claim (avg = {avg_latency:.0f} µs)")
    else:
        print(f"\n✗ FAIL: Does not meet <{threshold_us}µs (avg = {avg_latency:.0f} µs)")
    
    # Plot
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    
    # Histogram
    axes[0].hist(latencies, bins=50, color='steelblue', alpha=0.7, edgecolor='black')
    axes[0].axvline(avg_latency, color='r', linestyle='--', label=f'Avg: {avg_latency:.0f}µs')
    axes[0].axvline(threshold_us, color='g', linestyle='--', label=f'Target: {threshold_us}µs')
    axes[0].set_xlabel('Latency (µs)')
    axes[0].set_ylabel('Frequency')
    axes[0].set_title(f'{platform_name} E2E Latency Distribution')
    axes[0].legend()
    axes[0].grid(True, alpha=0.3)
    
    # CDF
    sorted_latencies = np.sort(latencies)
    cdf = np.arange(1, len(sorted_latencies) + 1) / len(sorted_latencies)
    axes[1].plot(sorted_latencies, cdf, linewidth=2)
    axes[1].axvline(threshold_us, color='g', linestyle='--', label=f'Target: {threshold_us}µs')
    axes[1].axhline(0.99, color='r', linestyle='--', label='P99', alpha=0.5)
    axes[1].set_xlabel('Latency (µs)')
    axes[1].set_ylabel('Cumulative Probability')
    axes[1].set_title(f'{platform_name} Latency CDF')
    axes[1].legend()
    axes[1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    output_path = f'../../results/2025-11-benchmarks/e2e_{platform_name.lower().replace(" ", "_")}.png'
    plt.savefig(output_path, dpi=300)
    print(f"✓ Plot saved to {output_path}")
    
    return {
        'min': latencies.min(),
        'avg': latencies.mean(),
        'p99': np.percentile(latencies, 99),
        'max': latencies.max(),
        'pass': avg_latency < threshold_us
    }

def main():
    parser = argparse.ArgumentParser(description='Analyze E2E latency from logic analyzer captures')
    parser.add_argument('--halo', help='Halo OS capture CSV', default='captures/halo_e2e.csv')
    parser.add_argument('--qnx', help='QNX capture CSV', default='captures/qnx_e2e.csv')
    parser.add_argument('--autosar', help='AUTOSAR capture CSV', default='captures/autosar_e2e.csv')
    parser.add_argument('--threshold', type=int, default=1000, help='Target latency (µs)')
    
    args = parser.parse_args()
    
    results = {}
    
    for platform, filepath in [('Halo OS', args.halo), ('QNX', args.qnx), ('AUTOSAR', args.autosar)]:
        if Path(filepath).exists():
            latencies = parse_logic_csv(filepath)
            results[platform] = analyze_latency(latencies, platform, args.threshold)
        else:
            print(f"⚠ Warning: {filepath} not found, skipping {platform}")
    
    # Comparison table
    print("\n=== Comparison Summary ===")
    print(f"{'Platform':<15} {'Min (µs)':<12} {'Avg (µs)':<12} {'P99 (µs)':<12} {'Max (µs)':<12} {'<{args.threshold}µs?'}")
    print("-" * 80)
    for platform, stats in results.items():
        pass_mark = "✓" if stats['pass'] else "✗"
        print(f"{platform:<15} {stats['min']:<12.0f} {stats['avg']:<12.0f} "
              f"{stats['p99']:<12.0f} {stats['max']:<12.0f} {pass_mark}")

if __name__ == '__main__':
    main()

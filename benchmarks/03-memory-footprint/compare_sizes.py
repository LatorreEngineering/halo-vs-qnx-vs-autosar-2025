#!/usr/bin/env python3
"""
Memory Footprint Comparison
Analyzes linker map files to compare TEXT/DATA/BSS sizes
"""

import re
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

def parse_map_file(filepath):
    """Extract memory sections from GCC map file"""
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Find memory configuration section
    text_match = re.search(r'\.text\s+0x[0-9a-f]+\s+(0x[0-9a-f]+)', content)
    data_match = re.search(r'\.data\s+0x[0-9a-f]+\s+(0x[0-9a-f]+)', content)
    bss_match = re.search(r'\.bss\s+0x[0-9a-f]+\s+(0x[0-9a-f]+)', content)
    
    def hex_to_kb(hex_str):
        return int(hex_str, 16) / 1024
    
    return {
        'text': hex_to_kb(text_match.group(1)) if text_match else 0,
        'data': hex_to_kb(data_match.group(1)) if data_match else 0,
        'bss': hex_to_kb(bss_match.group(1)) if bss_match else 0
    }

def main():
    # Parse map files
    map_files = {
        'Halo OS': 'halo_minimal.elf.map',
        'QNX': 'qnx_minimal.elf.map',
        'AUTOSAR': 'autosar_minimal.elf.map'
    }
    
    data = []
    for platform, mapfile in map_files.items():
        if Path(mapfile).exists():
            sizes = parse_map_file(mapfile)
            sizes['platform'] = platform
            sizes['total'] = sizes['text'] + sizes['data'] + sizes['bss']
            data.append(sizes)
    
    df = pd.DataFrame(data)
    
    # Print table
    print("\n=== Memory Footprint Comparison ===")
    print(f"{'Platform':<15} {'TEXT (KB)':<12} {'DATA (KB)':<12} {'BSS (KB)':<12} {'TOTAL (MB)':<12}")
    print("-" * 70)
    for _, row in df.iterrows():
        print(f"{row['platform']:<15} {row['text']:<12.0f} {row['data']:<12.0f} "
              f"{row['bss']:<12.0f} {row['total']/1024:<12.2f}")
    
    # Calculate savings
    autosar_total = df[df['platform'] == 'AUTOSAR']['total'].values[0]
    for _, row in df.iterrows():
        if row['platform'] != 'AUTOSAR':
            savings = (1 - row['total'] / autosar_total) * 100
            print(f"\n{row['platform']} is {savings:.1f}% smaller than AUTOSAR")
    
    # Plot
    fig, ax = plt.subplots(figsize=(10, 6))
    
    x = range(len(df))
    width = 0.25
    
    ax.bar([i - width for i in x], df['text'], width, label='TEXT', color='#3498db')
    ax.bar(x, df['data'], width, label='DATA', color='#2ecc71')
    ax.bar([i + width for i in x], df['bss'], width, label='BSS', color='#e74c3c')
    
    ax.set_xlabel('Platform')
    ax.set_ylabel('Size (KB)')
    ax.set_title('Memory Footprint Comparison (Minimal Build)')
    ax.set_xticks(x)
    ax.set_xticklabels(df['platform'])
    ax.legend()
    ax.grid(True, alpha=0.3, axis='y')
    
    plt.tight_layout()
    plt.savefig('../../results/2025-11-benchmarks/memory_footprint.png', dpi=300)
    print("\n✓ Plot saved to memory_footprint.png")
    
    # Save CSV
    df.to_csv('../../results/2025-11-benchmarks/memory_footprint.csv', index=False)
    print("✓ Data saved to memory_footprint.csv")

if __name__ == '__main__':
    main()

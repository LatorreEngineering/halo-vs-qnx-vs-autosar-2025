import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

def plot_jitter_comparison():
    """Generate comprehensive jitter comparison plots"""
    
    # Load data
    results_dir = Path("../../results/2025-11-benchmarks")
    
    halo_df = pd.read_csv(results_dir / "rt_halo.csv")
    qnx_df = pd.read_csv(results_dir / "rt_qnx.csv")
    autosar_df = pd.read_csv(results_dir / "rt_autosar.csv")
    
    # Create figure with subplots
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Real-Time Determinism: Halo OS vs QNX vs AUTOSAR', 
                 fontsize=16, fontweight='bold')
    
    # 1. Time series (first 10K samples)
    ax1 = axes[0, 0]
    ax1.plot(halo_df['iteration'][:10000], halo_df['latency_us'][:10000], 
             label='Halo OS', alpha=0.6, linewidth=0.5)
    ax1.plot(qnx_df['iteration'][:10000], qnx_df['latency_us'][:10000], 
             label='QNX', alpha=0.6, linewidth=0.5)
    ax1.plot(autosar_df['iteration'][:10000], autosar_df['latency_us'][:10000], 
             label='AUTOSAR', alpha=0.6, linewidth=0.5)
    ax1.set_xlabel('Iteration')
    ax1.set_ylabel('Latency (µs)')
    ax1.set_title('Jitter Over Time (First 10K Samples)')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # 2. Box plot
    ax2 = axes[0, 1]
    data = [halo_df['latency_us'], qnx_df['latency_us'], autosar_df['latency_us']]
    bp = ax2.boxplot(data, labels=['Halo OS', 'QNX', 'AUTOSAR'], patch_artist=True)
    colors = ['#3498db', '#2ecc71', '#e74c3c']
    for patch, color in zip(bp['boxes'], colors):
        patch.set_facecolor(color)
        patch.set_alpha(0.6)
    ax2.set_ylabel('Latency (µs)')
    ax2.set_title('Distribution Comparison')
    ax2.grid(True, alpha=0.3, axis='y')
    
    # 3. Histogram
    ax3 = axes[1, 0]
    ax3.hist(halo_df['latency_us'], bins=50, alpha=0.5, label='Halo OS', color='#3498db')
    ax3.hist(qnx_df['latency_us'], bins=50, alpha=0.5, label='QNX', color='#2ecc71')
    ax3.hist(autosar_df['latency_us'], bins=50, alpha=0.5, label='AUTOSAR', color='#e74c3c')
    ax3.set_xlabel('Latency (µs)')
    ax3.set_ylabel('Frequency')
    ax3.set_title('Latency Distribution')
    ax3.legend()
    ax3.grid(True, alpha=0.3, axis='y')
    
    # 4. CDF (Cumulative Distribution Function)
    ax4 = axes[1, 1]
    for df, label, color in [(halo_df, 'Halo OS', '#3498db'),
                              (qnx_df, 'QNX', '#2ecc71'),
                              (autosar_df, 'AUTOSAR', '#e74c3c')]:
        sorted_data = np.sort(df['latency_us'])
        cdf = np.arange(1, len(sorted_data) + 1) / len(sorted_data)
        ax4.plot(sorted_data, cdf, label=label, color=color, linewidth=2)
    
    ax4.set_xlabel('Latency (µs)')
    ax4.set_ylabel('Cumulative Probability')
    ax4.set_title('CDF: Determinism Analysis')
    ax4.axhline(y=0.99, color='r', linestyle='--', label='P99', alpha=0.5)
    ax4.legend()
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig(results_dir / 'plots' / 'jitter_comparison.png', dpi=300)
    print(f"✓ Plot saved to {results_dir / 'plots' / 'jitter_comparison.png'}")
    
    # Print statistics
    print("\n=== Statistics Summary ===")
    for name, df in [('Halo OS', halo_df), ('QNX', qnx_df), ('AUTOSAR', autosar_df)]:
        print(f"\n{name}:")
        print(f"  Min:    {df['latency_us'].min():.2f} µs")
        print(f"  Avg:    {df['latency_us'].mean():.2f} µs")
        print(f"  Median: {df['latency_us'].median():.2f} µs")
        print(f"  P99:    {df['latency_us'].quantile(0.99):.2f} µs")
        print(f"  Max:    {df['latency_us'].max():.2f} µs")
        print(f"  StdDev: {df['latency_us'].std():.2f} µs")

if __name__ == '__main__':
    plot_jitter_comparison()

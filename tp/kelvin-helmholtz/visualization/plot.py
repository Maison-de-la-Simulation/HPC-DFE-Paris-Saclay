"""
plot.py  —  plot all fields of a Kelvin-Helmholtz snapshot

Usage:
    python plot.py <path/to/snapshot_XXXXXX.npz>

The .npz file must contain: 'omega', 'u', 'v', 'time', 'dx', 'dy', 'Lx', 'Ly', 'nx', 'ny'
"""

import sys
import os
import numpy as np
import matplotlib.pyplot as plt

# ---------------------------------------------------------------------------
# Argument
# ---------------------------------------------------------------------------
if len(sys.argv) != 2:
    print("Usage: python plot.py <snapshot_XXXXXX.npz>")
    sys.exit(1)

filepath = sys.argv[1]
if not os.path.isfile(filepath):
    print(f"Error: file not found: {filepath}")
    sys.exit(1)

# ---------------------------------------------------------------------------
# Load
# ---------------------------------------------------------------------------
f     = np.load(filepath)
omega = f["omega"]
ux     = f["ux"]
uy     = f["uy"]
t     = float(f["time"])
Lx    = float(f["Lx"])
Ly    = float(f["Ly"])

fields = [
    (omega, r"Vorticity $\omega$", "RdBu_r"),
    (ux,     r"Velocity $u$",       "RdBu_r"),
    (uy,     r"Velocity $v$",       "RdBu_r"),
]

# ---------------------------------------------------------------------------
# Plot
# ---------------------------------------------------------------------------
fig, axes = plt.subplots(1, 3, figsize=(15, 5 * Ly / Lx))
fig.suptitle(f"t = {t:.4f}", fontsize=13)

extent = [0, Lx, 0, Ly]

for ax, (data, label, cmap) in zip(axes, fields):
    vmax = np.percentile(np.abs(data), 99) + 1e-12
    im = ax.imshow(
        data.T,
        extent=extent,
        origin="lower",
        cmap=cmap,
        vmin=-vmax,
        vmax=vmax,
        interpolation="bilinear",
        aspect="equal",
    )
    fig.colorbar(im, ax=ax, label=label, shrink=0.8)
    ax.set_title(label)
    ax.set_xlabel("x")
    ax.set_ylabel("y")

plt.tight_layout()
plt.show()

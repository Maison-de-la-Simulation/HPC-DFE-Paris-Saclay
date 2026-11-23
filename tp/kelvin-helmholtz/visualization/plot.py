"""
plot.py  —  plot all fields of a Kelvin-Helmholtz snapshot

Usage:
    python plot.py <path/to/snapshot_XXXXXX.h5>

The HDF5 file contains the datasets 'omega', 'ux', 'uy' and the attributes
'time', 'dx', 'dy', 'Lx', 'Ly', 'nx', 'ny' (see snapshot.py).
"""

import sys
import os
import numpy as np
import matplotlib.pyplot as plt

from snapshot import load_snapshot

# ---------------------------------------------------------------------------
# Argument
# ---------------------------------------------------------------------------
if len(sys.argv) != 2:
    print("Usage: python plot.py <snapshot_XXXXXX.h5>")
    sys.exit(1)

filepath = sys.argv[1]
if not os.path.isfile(filepath):
    print(f"Error: file not found: {filepath}")
    sys.exit(1)

# ---------------------------------------------------------------------------
# Load
# ---------------------------------------------------------------------------
snap  = load_snapshot(filepath)
omega = snap["omega"]
ux    = snap["ux"]
uy    = snap["uy"]
t     = snap["t"]
Lx    = snap["Lx"]
Ly    = snap["Ly"]

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

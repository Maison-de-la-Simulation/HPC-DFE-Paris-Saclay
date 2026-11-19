"""
compare.py — compare the error between two snapshots

Usage:
    python plot.py <path/to/snapshot_XXXXXX.npz> <path/to/snapshot_XXXXXX.npz> 

The .npz file must contain: 'omega', 'ux', 'uy', 'time', 'dx', 'dy', 'Lx', 'Ly', 'nx', 'ny'
"""

import sys
import os
import numpy as np
import matplotlib.pyplot as plt

# ---------------------------------------------------------------------------
# Argument
# ---------------------------------------------------------------------------
if len(sys.argv) != 3:
    print("Usage: python plot.py <snapshot_XXXXXX.npz> <snapshot_XXXXXX.npz>")
    sys.exit(1)

filepath0 = sys.argv[1]
filepath1 = sys.argv[1]

if not os.path.isfile(filepath0):
    print(f"Error: file not found: {filepath0}")
    sys.exit(1)

if not os.path.isfile(filepath1):
    print(f"Error: file not found: {filepath1}")
    sys.exit(1)

# ---------------------------------------------------------------------------
# Load
# ---------------------------------------------------------------------------
f0     = np.load(filepath0)
omega0 = f0["omega"]
ux0     = f0["ux"]
uy0     = f0["uy"]
t0     = float(f0["time"])
Lx0    = float(f0["Lx"])
Ly0    = float(f0["Ly"])

f1     = np.load(filepath1)
omega1 = f1["omega"]
ux1     = f1["ux"]
uy1     = f1["uy"]
t1     = float(f1["time"])
Lx1    = float(f1["Lx"])
Ly1    = float(f1["Ly"])

omega_error = np.abs(omega0 - omega1)
ux_error = np.abs(ux0 - ux1)
uy_error = np.abs(uy0 - uy1)

print(" Omega total L2 error: {}".format(np.sqrt(np.sum(np.pow(omega_error,2)))))
print(" Ux total L2 error: {}".format(np.sqrt(np.sum(np.pow(ux_error,2)))))
print(" Uy total L2 error: {}".format(np.sqrt(np.sum(np.pow(uy_error,2)))))

fields = [
    (omega_error, r"Vorticity $\omega$ error", "RdBu_r"),
    (ux_error,     r"Velocity $u$ error",       "RdBu_r"),
    (uy_error,     r"Velocity $v$ error",       "RdBu_r"),
]

# ---------------------------------------------------------------------------
# Plot
# ---------------------------------------------------------------------------
fig, axes = plt.subplots(1, 3, figsize=(15, 5 * Ly0 / Lx0))
fig.suptitle(f"t = {t0:.4f}", fontsize=13)

extent = [0, Lx0, 0, Ly0]

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
"""
animate.py  —  animate all Kelvin-Helmholtz snapshots

Usage:
    python animate.py <output_dir> [--save animation.gif] [--every 2] [--dpi 80] [--scale 0.5]

Arguments:
    output_dir      directory containing snapshot_XXXXXX.npz files
    --save <file>   (optional) save the animation to a file instead of
                    displaying it (requires ffmpeg for .mp4 or pillow for .gif)
    --every N       use only every Nth snapshot to reduce file size (default: 1)
    --dpi D         output resolution for saved animation (default: 100)
    --scale S       scale the figure size by S to reduce file size (default: 1.0)
"""

import sys
import os
import glob
import argparse
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# ---------------------------------------------------------------------------
# Arguments
# ---------------------------------------------------------------------------
parser = argparse.ArgumentParser(description="Animate Kelvin-Helmholtz snapshots.")
parser.add_argument("output_dir", help="directory containing snapshot_XXXXXX.npz files")
parser.add_argument("--save", metavar="FILE", default=None,
                    help="save animation to FILE (.mp4 or .gif)")
parser.add_argument("--fps", type=int, default=20, help="frames per second (default: 20)")
parser.add_argument("--every", type=int, default=1,
                    help="use only every Nth snapshot to reduce file size (default: 1)")
parser.add_argument("--dpi", type=float, default=100.0,
                    help="output resolution for saved animation (default: 100)")
parser.add_argument("--scale", type=float, default=1.0,
                    help="scale the figure size by this factor to reduce file size (default: 1.0)")
args = parser.parse_args()

if args.every < 1:
    print("Error: --every must be >= 1")
    sys.exit(1)
if args.scale <= 0:
    print("Error: --scale must be > 0")
    sys.exit(1)

if not os.path.isdir(args.output_dir):
    print(f"Error: directory not found: {args.output_dir}")
    sys.exit(1)

# ---------------------------------------------------------------------------
# Load snapshots (sorted by step index)
# ---------------------------------------------------------------------------
pattern = os.path.join(args.output_dir, "snapshot_*.npz")
files   = sorted(glob.glob(pattern))

if not files:
    print(f"Error: no snapshot_*.npz files found in '{args.output_dir}'")
    sys.exit(1)

print(f"Found {len(files)} snapshots in '{args.output_dir}'")

snapshots = []
for path in files:
    f = np.load(path)
    snapshots.append({
        "omega": f["omega"],
        "ux":     f["ux"],
        "uy":     f["uy"],
        "t":     float(f["time"]),
        "Lx":    float(f["Lx"]),
        "Ly":    float(f["Ly"]),
    })

snapshots = snapshots[::args.every]
print(f"Using {len(snapshots)} snapshots after --every={args.every}")

Lx = snapshots[0]["Lx"]
Ly = snapshots[0]["Ly"]
extent = [0, Lx, 0, Ly]

fields_info = [
    ("omega", r"Vorticity $\omega$"),
    ("ux",     r"Velocity $u_x$"),
    ("uy",     r"Velocity $u_y$"),
]

# ---------------------------------------------------------------------------
# Figure setup
# ---------------------------------------------------------------------------
fig, axes = plt.subplots(1, 3, figsize=(15 * args.scale, 5 * Ly / Lx * args.scale))
fig.subplots_adjust(top=0.88)

def field_vmax(key):
    """Compute a global colour scale from the 99th percentile over all frames."""
    return max(np.percentile(np.abs(s[key]), 99) for s in snapshots) + 1e-12

vmaxes = {key: field_vmax(key) for key, _ in fields_info}

ims = []
for ax, (key, label) in zip(axes, fields_info):
    vmax = vmaxes[key]
    im = ax.imshow(
        snapshots[0][key].T,
        extent=extent,
        origin="lower",
        cmap="RdBu_r",
        vmin=-vmax,
        vmax=vmax,
        interpolation="bilinear",
        aspect="equal",
    )
    fig.colorbar(im, ax=ax, label=label, shrink=0.8)
    ax.set_title(label)
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ims.append(im)

title = fig.suptitle(f"t = {snapshots[0]['t']:.4f}", fontsize=13)

# ---------------------------------------------------------------------------
# Animation
# ---------------------------------------------------------------------------
def update(frame_idx):
    snap = snapshots[frame_idx]
    for im, (key, _) in zip(ims, fields_info):
        im.set_data(snap[key].T)
    title.set_text(f"t = {snap['t']:.4f}")
    return ims + [title]

ani = animation.FuncAnimation(
    fig, update, frames=len(snapshots), interval=1000 // args.fps, blit=False
)

# ---------------------------------------------------------------------------
# Output
# ---------------------------------------------------------------------------
if args.save:
    ext = os.path.splitext(args.save)[1].lower()
    if ext == ".gif":
        writer = animation.PillowWriter(fps=args.fps)
    else:
        writer = animation.FFMpegWriter(fps=args.fps, bitrate=1800)
    print(f"Saving animation to '{args.save}' ...")
    ani.save(args.save, writer=writer, dpi=args.dpi)
    print("Done.")
else:
    plt.show()

"""
stack.py  —  stack Kelvin-Helmholtz snapshots into a 3D (x, y, t) volume for ParaView

Usage:
    python stack.py <output_dir> [--format xdmf|vtk] [--output stack]
                                 [--every N] [--time-scale S]

Reads every snapshot_XXXXXX.h5 file of <output_dir> and stacks each field
(omega, ux, uy) into a 3D array whose third axis is time. Two output formats:

--format xdmf (default): two files
    <output>.h5     the 3D arrays, one dataset per field, of shape (Nt, Ny, Nx)
    <output>.xdmf   a small XML file that tells ParaView how to read <output>.h5
                    as a regular 3D grid (x, y, time)
    Open <output>.xdmf in ParaView (choose the "Xdmf Reader" if asked).

--format vtk: a single file
    <output>.vtk    legacy VTK file (binary, regular grid), opened directly by
                    ParaView. Written with NumPy only, no extra library needed.

In ParaView, the axes are X = x, Y = y and Z = time. Use for instance a Contour,
a Slice or a Volume rendering of omega.

The time axis is scaled so that the time extent of the volume equals Lx by
default; use --time-scale to change the length of one time unit along this axis.
"""

import argparse
import os
import sys

import h5py
import numpy as np

from snapshot import list_snapshots, load_snapshot

FIELDS = ("omega", "ux", "uy")

# ---------------------------------------------------------------------------
# Arguments
# ---------------------------------------------------------------------------
parser = argparse.ArgumentParser(
    description="Stack Kelvin-Helmholtz snapshots into a 3D (x, y, t) volume for ParaView.")
parser.add_argument("output_dir", help="directory containing snapshot_XXXXXX.h5 files")
parser.add_argument("--format", choices=("xdmf", "vtk"), default="xdmf",
                    help="xdmf: HDF5 data + XDMF description (default); "
                         "vtk: a single legacy VTK file")
parser.add_argument("--output", default="stack",
                    help="name of the output file(s), without extension (default: stack)")
parser.add_argument("--every", type=int, default=1,
                    help="use only every Nth snapshot (default: 1)")
parser.add_argument("--time-scale", type=float, default=None,
                    help="length of one time unit along the time axis "
                         "(default: the time extent is set equal to Lx)")
args = parser.parse_args()

if args.every < 1:
    sys.exit("Error: --every must be >= 1")

files = list_snapshots(args.output_dir)[::args.every]
if len(files) < 2:
    sys.exit(f"Error: at least 2 snapshot_*.h5 files are needed in '{args.output_dir}'")

# ---------------------------------------------------------------------------
# Grid and time axis
# ---------------------------------------------------------------------------
with h5py.File(files[0], "r") as f:
    Nx, Ny = f["omega"].shape
    dx = float(f.attrs["dx"])
    dy = float(f.attrs["dy"])
    Lx = float(f.attrs["Lx"])

times = []
for path in files:
    with h5py.File(path, "r") as f:
        times.append(float(f.attrs["time"]))
times = np.array(times)
Nt = len(files)

dt_snapshots = np.diff(times)
dt_mean = dt_snapshots.mean()
if np.max(np.abs(dt_snapshots - dt_mean)) > 1e-6 * max(dt_mean, 1e-300):
    print("Warning: snapshots are not evenly spaced in time; "
          "the time axis uses the mean spacing")

time_scale = args.time_scale if args.time_scale is not None else Lx / (times[-1] - times[0])
dt_axis = dt_mean * time_scale
t0_axis = times[0] * time_scale

print(f"Stacking {Nt} snapshots of {Nx} x {Ny} points, "
      f"t = {times[0]:.4f} to {times[-1]:.4f} (time scale {time_scale:.4g})")

# ---------------------------------------------------------------------------
# Stack the snapshots
# ---------------------------------------------------------------------------
# Snapshots are stored with shape (Nx, Ny) (indexing 'ij'). The volumes have
# shape (Nt, Ny, Nx): x varies fastest in memory, then y, then time, which is
# the order expected by both XDMF and VTK for a regular grid.
volumes = {name: np.empty((Nt, Ny, Nx), dtype=np.float64) for name in FIELDS}
for k, path in enumerate(files):
    snap = load_snapshot(path)
    for name in FIELDS:
        volumes[name][k, :, :] = snap[name].T


def write_xdmf(output):
    """Write <output>.h5 (data) and <output>.xdmf (description for ParaView)."""
    h5_path = output + ".h5"
    with h5py.File(h5_path, "w") as out:
        for name in FIELDS:
            out.create_dataset(name, data=volumes[name])
        out.create_dataset("time", data=times)
        out.attrs["dx"] = dx
        out.attrs["dy"] = dy
        out.attrs["time_scale"] = time_scale

    # For a 3DCoRectMesh, XDMF lists dimensions, origin and spacing from the
    # slowest to the fastest axis: (t, y, x).
    h5_name = os.path.basename(h5_path)
    attributes = "\n".join(
        f'''      <Attribute Name="{name}" AttributeType="Scalar" Center="Node">
        <DataItem Dimensions="{Nt} {Ny} {Nx}" NumberType="Float" Precision="8" Format="HDF">{h5_name}:/{name}</DataItem>
      </Attribute>'''
        for name in FIELDS)

    xdmf = f'''<?xml version="1.0" ?>
<!DOCTYPE Xdmf SYSTEM "Xdmf.dtd" []>
<Xdmf Version="2.0">
  <Domain>
    <Grid Name="kelvin_helmholtz_xyt" GridType="Uniform">
      <Topology TopologyType="3DCoRectMesh" Dimensions="{Nt} {Ny} {Nx}"/>
      <Geometry GeometryType="ORIGIN_DXDYDZ">
        <DataItem Dimensions="3" NumberType="Float" Precision="8" Format="XML">{t0_axis} 0.0 0.0</DataItem>
        <DataItem Dimensions="3" NumberType="Float" Precision="8" Format="XML">{dt_axis} {dy} {dx}</DataItem>
      </Geometry>
{attributes}
    </Grid>
  </Domain>
</Xdmf>
'''
    xdmf_path = output + ".xdmf"
    with open(xdmf_path, "w") as f:
        f.write(xdmf)
    return [h5_path, xdmf_path], xdmf_path


def write_vtk(output):
    """Write <output>.vtk, a legacy VTK file (binary STRUCTURED_POINTS).

    Header in text, then for each field the values in big-endian binary,
    x varying fastest. Dimensions, origin and spacing are given as (x, y, t).
    """
    vtk_path = output + ".vtk"
    with open(vtk_path, "wb") as f:
        header = (
            "# vtk DataFile Version 3.0\n"
            "Kelvin-Helmholtz instability (x, y, time)\n"
            "BINARY\n"
            "DATASET STRUCTURED_POINTS\n"
            f"DIMENSIONS {Nx} {Ny} {Nt}\n"
            f"ORIGIN 0.0 0.0 {t0_axis}\n"
            f"SPACING {dx} {dy} {dt_axis}\n"
            f"POINT_DATA {Nx * Ny * Nt}\n"
        )
        f.write(header.encode("ascii"))
        for name in FIELDS:
            f.write(f"SCALARS {name} double 1\nLOOKUP_TABLE default\n".encode("ascii"))
            f.write(volumes[name].astype(">f8").tobytes())
            f.write(b"\n")
    return [vtk_path], vtk_path


# ---------------------------------------------------------------------------
# Write
# ---------------------------------------------------------------------------
if args.format == "xdmf":
    written, to_open = write_xdmf(args.output)
else:
    written, to_open = write_vtk(args.output)

print("Written " + " and ".join(written))
print(f"Open {to_open} in ParaView: axes X = x, Y = y, Z = time")

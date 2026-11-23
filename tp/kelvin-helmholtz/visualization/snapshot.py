"""
snapshot.py  —  read a Kelvin-Helmholtz snapshot

Shared by plot.py, animate.py and compare.py.

Snapshots are HDF5 files (snapshot_XXXXXX.h5) with:
    - datasets   : 'omega', 'ux', 'uy'  (shape Nx x Ny)
    - attributes : 'time', 'dx', 'dy', 'Lx', 'Ly', 'nx', 'ny'
                   (and 'mpi_size' for the MPI version)
"""

import glob
import os

import h5py


def load_snapshot(path):
    """Return a dict with the fields and parameters of a snapshot file."""
    with h5py.File(path, "r") as f:
        return {
            "omega": f["omega"][...],
            "ux": f["ux"][...],
            "uy": f["uy"][...],
            "t": float(f.attrs["time"]),
            "Lx": float(f.attrs["Lx"]),
            "Ly": float(f.attrs["Ly"]),
        }


def list_snapshots(output_dir):
    """Return the sorted list of snapshot files in output_dir."""
    return sorted(glob.glob(os.path.join(output_dir, "snapshot_*.h5")))

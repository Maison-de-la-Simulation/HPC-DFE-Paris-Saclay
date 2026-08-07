# I. Discovery of the sequential code

**Question 1.1 - first execution:** Once you ensure that your Python environment is ready, run the sequential code with the default parameters. Check that you reach the end of simulation and that the diag folder is well gererated with snaphots.

```python
python main.py
```

**Question 2.2 - visualization:** Visualization can be performed using the scripts in the directory `visualiaztion`.

Plot some snapshots at different timesteps or create an animation to ensure that the simulation has run correctly.

```python
python visualiaztion/plot.py python plot.py diags/snapshot_000000.npz
```

**Question 2.3 - read the code:** Take the time now to read the code and to understand the structure. You can also run it again by changing the initial parameters.
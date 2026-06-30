"""
Example: Driving Daisy via the extended API interface (daisy_bmi Python module).

This shows the Daisy-specific API coupling pattern, which extends the standard
BMI interface with additional Daisy-specific methods. Use this when you need
access beyond the BMI standard, e.g. for perturbation runs or direct state access.

For a plain BMI 2.0 coupling (pymt, OpenEarth, MODFLOW6), use BMI instead of API.

Requirements:
  - install daisy_bmi Python module via 'pip install -e python' from the root dir of this project
"""

import os
import numpy as np
from daisy import API
from pathlib import Path

# daisy config file
config_file = Path(r"c:\src\daisy\sample\sample_bmi.dai")

# definitions
def compute_zh0(tops: np.ndarray, h_daisy: np.ndarray) -> float | None:
    """Depth of h=0 [m, +down] by linear interpolation. None if WT outside column."""
    h = np.array(h_daisy)
    tops_m   = np.array(tops) * 1e-2
    sat_mask = h > 0.0
    if not sat_mask.any():
        return None
    sat_node = int(np.argmax(sat_mask))
    if sat_node == 0:
        return 0.0
    i_below = sat_node - 1
    i_above = sat_node
    h_below = float(h[i_below])
    h_above = float(h[i_above])
    z_below = float(tops_m[i_below])
    z_above = float(tops_m[i_above])
    return z_below + (0.0 - h_below) / (h_above - h_below) * (z_above - z_below)

# -- Daisy simulation --

# initialise Daisy
api = API()
os.chdir(config_file.parent)
api.initialize(str(config_file))

# get static internals
area   = api.get_value("column__area")             # scalar cm²
tops   = api.get_value_array("soil_layer__top_depth")    # np.NDarray[float] cm
bots   = api.get_value_array("soil_layer__bottom_depth") # np.NDarray[float] cm
n_lay  = len(tops)

# runs simulations per daily time step
end_time = 365

print(f"BMI time units are: {api.get_time_units()}")

for itime in np.arange(end_time-1):
    # Get current time
    t = api.get_current_time()

    # Advance Daisy one day
    api.update_until(t + 1.0)

    # get pressure heads and GW table depth
    h = api.get_value_array("soil_water__pressure_head")
    gwl = compute_zh0(tops, h_daisy=h)
    if gwl is None:
        print(f'gwl at day {t:.1f} is below Daisy column')
    else:
        print(f'gwl at day {t:.1f} is {gwl:.3f} m')

api.finalize()

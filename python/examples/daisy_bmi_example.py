"""
Example: Driving Daisy via the BMI interface (daisy_bmi Python module).

This shows the standard BMI coupling pattern, and is ready to be plugged
into pymt, OpenEarth, or a custom MODFLOW6 coupling loop.

Requirements:
  - install daisy_bmi Python module via 'pip install -e python' from the root dir of this project
"""

import os
import numpy as np
from daisy import DaisyBMI
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
daisy = DaisyBMI()
os.chdir(config_file.parent)
daisy.initialize(str(config_file))

# get static internals
area   = daisy.get_value("column__area")             # scalar cm²
tops   = daisy.get_value_array("soil_layer__top_depth")    # np.NDarray[float] cm
bots   = daisy.get_value_array("soil_layer__bottom_depth") # np.NDarray[float] cm
n_lay  = len(tops)

# runs simulations per daily time step
end_time = 365

print(f"BMI time units are: {daisy.get_time_units()}")

for itime in np.arange(end_time-1):
    # Get current time
    t = daisy.get_current_time()

    # Advance Daisy one day
    daisy.update_until(t + 1.0)

    # get presure heads of column
    h = daisy.get_value_array("soil_water__pressure_head") 
    gwl = compute_zh0(tops, h_daisy=h)
    if gwl is None:
        print(f'gwl at day {t:.1f} is below Daisy column')
    else:
        print(f'gwl at day {t:.1f} is {gwl}')

daisy.finalize()

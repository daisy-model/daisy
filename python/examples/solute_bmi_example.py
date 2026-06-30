"""
Example: Solute transport coupling via the Daisy BMI interface.

Demonstrates the new solute primitives added for MODFLOW coupling.
Follows the same style as daisy_api_example.py.

Requirements:
  - install daisy_bmi Python module via 'pip install -e python' from the root dir
  - a .dai config file that traces at least one chemical (e.g. NO3)
"""

import os
import numpy as np
from daisy import API
from pathlib import Path

config_file = Path(r"c:\src\daisy\sample\sample_bmi.dai")

# Simulated groundwater concentration per BMI variable [g/cm³].
C_GW: dict[str, float] = {"soil_solute_NO3__concentration": 5e-6}

api = API()
os.chdir(config_file.parent)
api.initialize(str(config_file))

# static geometry
tops = api.get_value_array("soil_layer__top_depth")     # [cm]
bots = api.get_value_array("soil_layer__bottom_depth")  # [cm]
dz   = np.abs(bots - tops)                              # layer thicknesses [cm]

solute_vars = [n for n in api.get_output_var_names()
               if n.startswith("soil_solute_") and n.endswith("__concentration")]
print(f"Traced solute vars: {solute_vars}")

for _ in range(10):
    t = api.get_current_time()
    print(t)

    h     = api.get_value_array("soil_water__pressure_head")  # [cm]
    q     = api.get_value_array("soil_water__recharge_rate")  # [mm/day]

    # sat_node: first layer where h > 0 (exchange layer)
    sat   = np.where(h > 0.0)[0]
    sat_node = int(sat[0]) if len(sat) else None

    if sat_node is None:
        print(f"day {t:.0f}: water table below column")
        api.update_until(t + 1.0)
        continue

    q_cm_h = float(q[sat_node]) / (10.0 * 24.0)  # mm/day → cm/h

    api.update_until(t + 1.0)

    # buffer-zone sync: overwrite saturated cells with groundwater concentration.
    for var in solute_vars:
        C = api.get_value_array(var).copy()           # [g/cm³], one per layer

        c_gw = C_GW.get(var, 0.0)
        C[h >= 0.0] = c_gw                            # groundwater model owns saturated cells

        api.set_value_array(var, C)

        J_out = q_cm_h * C[sat_node]                  # [g/cm²/h]
        print(f"day {t:.0f}  [{var}]  C={C[sat_node]:.3e} g/cm³  J={J_out:.3e} g/cm²/h")

api.finalize()

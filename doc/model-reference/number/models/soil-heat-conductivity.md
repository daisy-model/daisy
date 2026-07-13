---
doc_type: model
component: number
model: soil-heat-conductivity
registered_name: soil_heat_conductivity
base_model: horizon
implementation_class: NumberSoilHeatConductivity
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_soil.C
registration_description: Find heat conductivity for a given pressure (h).
status: scaffold
---

# `number::soil_heat_conductivity`

## Summary

Find soil heat conductivity for a given pressure within a specific soil horizon.

## Exposed class

- `NumberSoilHeatConductivity`

## Construction paths

- Registration/build path: `NumberSoilHeatConductivity(const BlockModel&)`

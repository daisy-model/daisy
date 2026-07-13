---
doc_type: model
component: number
model: soil-heat-capacity
registered_name: soil_heat_capacity
base_model: horizon
implementation_class: NumberSoilHeatCapacity
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_soil.C
registration_description: Find heat capacity for a given pressure (h).
status: scaffold
---

# `number::soil_heat_capacity`

## Summary

Find soil heat capacity for a given pressure within a specific soil horizon.

## Exposed class

- `NumberSoilHeatCapacity`

## Construction paths

- Registration/build path: `NumberSoilHeatCapacity(const BlockModel&)`

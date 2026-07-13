---
doc_type: model
component: number
model: soil-theta
registered_name: soil_Theta
base_model: horizon
implementation_class: NumberSoilTheta
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_soil.C
registration_description: Find water content (Theta) for a given pressure (h).
status: scaffold
---

# `number::soil_Theta`

## Summary

Find water content for a given pressure within a specific soil horizon.

## Exposed class

- `NumberSoilTheta`

## Construction paths

- Registration/build path: `NumberSoilTheta(const BlockModel&)`

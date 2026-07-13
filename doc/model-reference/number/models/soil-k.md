---
doc_type: model
component: number
model: soil-k
registered_name: soil_K
base_model: horizon
implementation_class: NumberSoilK
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_soil.C
registration_description: Find hydraulic conductivity (K) for a given pressure (h).
status: scaffold
---

# `number::soil_K`

## Summary

Find hydraulic conductivity for a given pressure within a specific soil horizon.

## Exposed class

- `NumberSoilK`

## Construction paths

- Registration/build path: `NumberSoilK(const BlockModel&)`

## TODO

- Document the interaction between primary and secondary domain conductivity in the result.

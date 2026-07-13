---
doc_type: model
component: number
model: depth-theta
registered_name: depth_Theta
base_model: depth
implementation_class: NumberDepthTheta
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_soil.C
registration_description: Find water content (Theta) for a given pressure (h).
status: scaffold
---

# `number::depth_Theta`

## Summary

Find water content for a given pressure at a specific depth in a soil column.

## Exposed class

- `NumberDepthTheta`

## Construction paths

- Registration/build path: `NumberDepthTheta(const BlockModel&)`

## TODO

- Add an example showing how `column`, `h`, and `z` interact in a full input.

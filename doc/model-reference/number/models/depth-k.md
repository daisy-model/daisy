---
doc_type: model
component: number
model: depth-k
registered_name: depth_K
base_model: depth
implementation_class: NumberDepthK
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_soil.C
registration_description: Find water conductivity (K) for a given pressure (h).
status: scaffold
---

# `number::depth_K`

## Summary

Find hydraulic conductivity for a given pressure at a specific depth in a soil column.

## Exposed class

- `NumberDepthK`

## Construction paths

- Registration/build path: `NumberDepthK(const BlockModel&)`

## TODO

- Clarify whether the reported conductivity includes any secondary-domain contribution.

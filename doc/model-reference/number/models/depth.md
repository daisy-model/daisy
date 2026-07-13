---
doc_type: model
component: number
model: depth
registered_name: depth
base_model: component
implementation_class: NumberByDepth
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_soil.C
registration_description: Find soil value at specific depth.
status: scaffold
---

# `number::depth`

## Summary

Abstract base model for soil lookups that combine a soil column, a pressure, and a depth.

## Exposed class

- `NumberByDepth`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `column` | state singleton column object | The soil column whose properties we want to examine. |
| `h` | state singleton number object | The tension we want to compare with. |
| `z` | state singleton number object | The height we want to compare with. |

## Construction paths

- Registration/build path: `NumberByDepth(const BlockModel&)`

## TODO

- Document the coordinate conventions for `z` and how out-of-column depths are treated.

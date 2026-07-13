---
doc_type: model
component: number
model: horizon
registered_name: horizon
base_model: component
implementation_class: NumberByTension
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_soil.C
registration_description: Find soil value at specific horizon.
status: scaffold
---

# `number::horizon`

## Summary

Abstract base model for soil lookups that query a specific horizon at a supplied pressure.

## Exposed class

- `NumberByTension`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `horizon` | state singleton horizon object | The soil horizon whose properties we want to examine. |
| `h` | state singleton number object | The tension we want to compare with. |
| `top_soil` | const boolean | Set this to true for the A horizon. |

## Construction paths

- Registration/build path: `NumberByTension(const BlockModel&)`

## TODO

- Document how `top_soil` affects horizon initialization and defaults.

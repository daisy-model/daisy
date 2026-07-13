---
doc_type: model
component: number
model: daisy-soil-soil-h
registered_name: daisy/soil/soil.h
base_model: component
implementation_class: NumberTensionByTheta
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_soil.C
registration_description: Find pressure (h) for a given water content (Theta).
status: scaffold
---

# `number::daisy/soil/soil.h`

## Summary

Find pressure for a given water content within a specific soil horizon.

## Exposed class

- `NumberTensionByTheta`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `horizon` | state singleton horizon object | The soil horizon whose properties we want to examine. |
| `Theta` | state singleton number object | The water content we want to compare with. |
| `top_soil` | const boolean | Set this to true for the A horizon. |

## Construction paths

- Registration/build path: `NumberTensionByTheta(const BlockModel&)`

## TODO

- Confirm whether the registered model name should remain `daisy/soil/soil.h` or be normalized in a later compatibility-preserving cleanup.

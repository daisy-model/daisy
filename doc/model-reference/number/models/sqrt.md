---
doc_type: model
component: number
model: sqrt
registered_name: sqrt
base_model: component
implementation_class: NumberSqrt
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_arit.C
registration_description: Take the square root of its argument.
status: scaffold
---

# `number::sqrt`

## Summary

Take the square root of the operand.

## Exposed class

- `NumberSqrt`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operand` | state singleton number object | Operand for this function. |

## Construction paths

- Direct-use path: `NumberSqrt(std::unique_ptr<Number>)`
- Registration/build path: `NumberSqrt(const BlockModel&)`

## TODO

- Document domain requirements for non-negative inputs.
- Add examples showing square-root number models in Daisy inputs.

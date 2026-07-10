---
doc_type: model
component: number
model: sqr
registered_name: sqr
base_model: component
implementation_class: NumberSqr
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_arit.C
registration_description: Take the square of its argument.
status: scaffold
---

# `number::sqr`

## Summary

Take the square of the operand.

## Exposed class

- `NumberSqr`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operand` | state singleton number object | Operand for this function. |

## Construction paths

- Direct-use path: `NumberSqr(std::unique_ptr<Number>)`
- Registration/build path: `NumberSqr(const BlockModel&)`

## TODO

- Document how output dimensions are derived from the operand dimension.
- Add examples showing squared numbers in Daisy inputs.

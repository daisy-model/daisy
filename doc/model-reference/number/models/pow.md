---
doc_type: model
component: number
model: pow
registered_name: pow
base_model: component
implementation_class: NumberPow
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_arit.C
registration_description: Raise 'base' to the power of 'exponent'.
status: scaffold
---

# `number::pow`

## Summary

Raise one number to the power of another.

## Exposed class

- `NumberPow`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `base` | state singleton number object | The base operand for this function. |
| `exponent` | state singleton number object | The exponent operand for this function. |

## Construction paths

- Direct-use path: `NumberPow(std::unique_ptr<Number>, std::unique_ptr<Number>)`
- Registration/build path: `NumberPow(const BlockModel&)`

## TODO

- Document dimension expectations for exponent handling.
- Add an example showing a power expression in Daisy input.

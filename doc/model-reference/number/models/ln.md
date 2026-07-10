---
doc_type: model
component: number
model: ln
registered_name: ln
base_model: component
implementation_class: NumberLn
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_arit.C
registration_description: Take the natural logarithm of its argument.
status: scaffold
---

# `number::ln`

## Summary

Take the natural logarithm of the operand.

## Exposed class

- `NumberLn`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operand` | state singleton number object | Operand for this function. |

## Construction paths

- Direct-use path: `NumberLn(std::unique_ptr<Number>)`
- Registration/build path: `NumberLn(const BlockModel&)`

## TODO

- Document domain requirements for positive inputs.
- Add examples showing how logarithmic number models are used in Daisy inputs.

---
doc_type: model
component: number
model: log10
registered_name: log10
base_model: component
implementation_class: NumberLog10
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_arit.C
registration_description: Take the base 10 logarithm of its argument.
status: scaffold
---

# `number::log10`

## Summary

Take the base 10 logarithm of the operand.

## Exposed class

- `NumberLog10`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operand` | state singleton number object | Operand for this function. |

## Construction paths

- Direct-use path: `NumberLog10(std::unique_ptr<Number>)`
- Registration/build path: `NumberLog10(const BlockModel&)`

## TODO

- Document domain requirements for positive inputs.
- Add examples showing how logarithmic number models are used in Daisy inputs.

---
doc_type: model
component: number
model: operands
registered_name: operands
base_model: component
implementation_class: NumberOperands
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_arit.C
registration_description: Shared helper for arithmetic expressions involving a variable number of number operands.
status: scaffold
---

# `number::operands`

## Summary

Abstract helper for arithmetic number models that consume a sequence of operands.

## Exposed class

- `NumberOperands`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operands` | const variable number object sequence | The operands for this function. |

## Construction paths

- Direct-use path: derived classes accept `std::vector<std::unique_ptr<Number>>`
- Registration/build path: `NumberOperands(const BlockModel&)`

## TODO

- Document ownership and ordering expectations for the operand sequence.
- Explain which derived classes require at least one operand.

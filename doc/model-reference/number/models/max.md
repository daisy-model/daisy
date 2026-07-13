---
doc_type: model
component: number
model: max
registered_name: max
base_model: component
implementation_class: NumberMax
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_arit.C
registration_description: Use the largest value of its operands.
status: scaffold
---

# `number::max`

## Summary

Use the largest value of the operand sequence.

## Exposed class

- `NumberMax`

## Declared entries

This model relies on the inherited `operands` declaration.

## Construction paths

- Direct-use path: `NumberMax(std::vector<std::unique_ptr<Number>>)`
- Registration/build path: `NumberMax(const BlockModel&)`

## TODO

- Document dimension selection when operands disagree.
- Add an example showing a max expression in Daisy input.

---
doc_type: model
component: number
model: min
registered_name: min
base_model: component
implementation_class: NumberMin
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_arit.C
registration_description: Use the smallest value of its operands.
status: scaffold
---

# `number::min`

## Summary

Use the smallest value of the operand sequence.

## Exposed class

- `NumberMin`

## Declared entries

This model relies on the inherited `operands` declaration.

## Construction paths

- Direct-use path: `NumberMin(std::vector<std::unique_ptr<Number>>)`
- Registration/build path: `NumberMin(const BlockModel&)`

## TODO

- Document dimension selection when operands disagree.
- Add an example showing a min expression in Daisy input.

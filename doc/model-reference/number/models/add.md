---
doc_type: model
component: number
model: "+"
registered_name: "+"
base_model: component
implementation_class: NumberSum
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_arit.C
registration_description: Use the sum of its operands.
status: scaffold
---

# `number::+`

## Summary

Use the sum of the operand sequence.

## Exposed class

- `NumberSum`

## Declared entries

This model relies on the inherited `operands` declaration.

## Construction paths

- Direct-use path: `NumberSum(std::vector<std::unique_ptr<Number>>)`
- Registration/build path: `NumberSum(const BlockModel&)`

## TODO

- Document expected dimension consistency across operands.
- Add an example showing addition in Daisy input.

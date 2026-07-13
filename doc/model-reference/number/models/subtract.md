---
doc_type: model
component: number
model: "-"
registered_name: "-"
base_model: component
implementation_class: NumberSubtract
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_arit.C
registration_description: Negate number or subtract numbers. With one operand, negates it. With more than one operand, subtracts all but the first from the first.
status: scaffold
---

# `number::-`

## Summary

Negate a number or subtract multiple numbers from the first operand.

## Exposed class

- `NumberSubtract`

## Declared entries

This model relies on the inherited `operands` declaration.

## Construction paths

- Direct-use path: `NumberSubtract(std::vector<std::unique_ptr<Number>>)`
- Registration/build path: `NumberSubtract(const BlockModel&)`

## TODO

- Document the unary-vs-n-ary behavior split more explicitly.
- Add an example showing subtraction in Daisy input.

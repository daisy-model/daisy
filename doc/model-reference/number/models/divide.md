---
doc_type: model
component: number
model: "/"
registered_name: "/"
base_model: component
implementation_class: NumberDivide
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_arit.C
registration_description: Divide the first operand by the rest.
status: scaffold
---

# `number::/`

## Summary

Divide the first operand by the remaining operands.

## Exposed class

- `NumberDivide`

## Declared entries

This model relies on the inherited `operands` declaration.

## Construction paths

- Direct-use path: `NumberDivide(std::vector<std::unique_ptr<Number>>)`
- Registration/build path: `NumberDivide(const BlockModel&)`

## TODO

- Document output-dimension simplification rules.
- Add an example showing division in Daisy input.

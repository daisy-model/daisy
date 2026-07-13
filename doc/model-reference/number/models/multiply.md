---
doc_type: model
component: number
model: "*"
registered_name: "*"
base_model: component
implementation_class: NumberProduct
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_arit.C
registration_description: Use the product of its operands.
status: scaffold
---

# `number::*`

## Summary

Use the product of the operand sequence.

## Exposed class

- `NumberProduct`

## Declared entries

This model relies on the inherited `operands` declaration.

## Construction paths

- Direct-use path: `NumberProduct(std::vector<std::unique_ptr<Number>>)`
- Registration/build path: `NumberProduct(const BlockModel&)`

## TODO

- Document output-dimension composition for mixed operand dimensions.
- Add an example showing multiplication in Daisy input.

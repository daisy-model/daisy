---
doc_type: model
component: boolean
model: ">"
registered_name: ">"
base_model: numbers
implementation_class: BooleanNumGT
implementation_header: include/object_model/parameter_types/boolean.h
registration_source: src/object_model/parameter_types/boolean_number.C
registration_description: True iff each operand is larger than the next.
status: scaffold
---

# `boolean::>`

## Summary

True iff each number operand is larger than the next.

## Exposed class

- `BooleanNumGT`

## Declared entries

This model relies on the inherited `operands` declaration.

## Construction paths

- Direct-use path: `BooleanNumGT(std::vector<std::unique_ptr<Number>>)`
- Registration/build path: `BooleanNumGT(const BlockModel&)`

## TODO

- Document behavior for empty and singleton operand lists.
- Add an example showing strict descending comparisons.

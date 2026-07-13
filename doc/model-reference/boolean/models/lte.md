---
doc_type: model
component: boolean
model: "<="
registered_name: "<="
base_model: numbers
implementation_class: BooleanNumLTE
implementation_header: include/object_model/parameter_types/boolean.h
registration_source: src/object_model/parameter_types/boolean_number.C
registration_description: True iff each operand is smaller than or equal to the next.
status: scaffold
---

# `boolean::<=`

## Summary

True iff each number operand is smaller than or equal to the next.

## Exposed class

- `BooleanNumLTE`

## Declared entries

This model relies on the inherited `operands` declaration.

## Construction paths

- Direct-use path: `BooleanNumLTE(std::vector<std::unique_ptr<Number>>)`
- Registration/build path: `BooleanNumLTE(const BlockModel&)`

## TODO

- Document behavior for empty and singleton operand lists.
- Add an example showing ascending non-strict comparisons.

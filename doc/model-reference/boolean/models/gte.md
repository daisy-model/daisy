---
doc_type: model
component: boolean
model: ">="
registered_name: ">="
base_model: numbers
implementation_class: BooleanNumGTE
implementation_header: include/object_model/parameter_types/boolean.h
registration_source: src/object_model/parameter_types/boolean_number.C
registration_description: True iff each operand is at least as large as the next.
status: scaffold
---

# `boolean::>=`

## Summary

True iff each number operand is at least as large as the next.

## Exposed class

- `BooleanNumGTE`

## Declared entries

This model relies on the inherited `operands` declaration.

## Construction paths

- Direct-use path: `BooleanNumGTE(std::vector<std::unique_ptr<Number>>)`
- Registration/build path: `BooleanNumGTE(const BlockModel&)`

## TODO

- Document behavior for empty and singleton operand lists.
- Add an example showing descending non-strict comparisons.

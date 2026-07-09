---
doc_type: model
component: boolean
model: and
registered_name: and
base_model: operands
implementation_class: BooleanAnd
implementation_header: include/object_model/parameter_types/boolean.h
registration_source: src/object_model/parameter_types/boolean.C
registration_description: True if and only if all operands are true.
status: scaffold
---

# `boolean::and`

## Summary

True if and only if all operands are true.

## Exposed class

- `BooleanAnd`

## Declared entries

This model currently relies on the inherited `operands` declaration from
`boolean::operands`.

## Construction paths

- Registration/build path: `BooleanAnd(const BlockModel&)`

## TODO

- Add direct-construction documentation when a non-`BlockModel` constructor is introduced.
- Document short-circuit or evaluation-order expectations if they become part of the contract.
- Add an example showing an operand list in Daisy input.

---
doc_type: model
component: integer
model: div
registered_name: div
base_model: component
implementation_class: IntegerDivide
implementation_header: include/object_model/parameter_types/integer.h
registration_source: src/object_model/parameter_types/integer_arit.C
registration_description: Divide the first operand by the rest.
status: scaffold
---

# `integer::div`

## Summary

Divide the first operand by the rest.

## Exposed class

- `IntegerDivide`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operands` | const integer object sequence of size 2 | The operands for this function. |

## Construction paths

- Direct-use path: `IntegerDivide(std::vector<std::unique_ptr<Integer>>)`
- Registration/build path: `IntegerDivide(const BlockModel&)`

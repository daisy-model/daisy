---
doc_type: model
component: integer
model: sqr
registered_name: sqr
base_model: component
implementation_class: IntegerSqr
implementation_header: include/object_model/parameter_types/integer.h
registration_source: src/object_model/parameter_types/integer_arit.C
registration_description: Take the square of its argument.
status: scaffold
---

# `integer::sqr`

## Summary

Take the square of its argument.

## Exposed class

- `IntegerSqr`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operand` | integer object | Operand for this function. |

## Construction paths

- Direct-use path: `IntegerSqr(std::unique_ptr<Integer>)`
- Registration/build path: `IntegerSqr(const BlockModel&)`

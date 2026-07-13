---
doc_type: model
component: integer
model: mod
registered_name: mod
base_model: component
implementation_class: IntegerModulo
implementation_header: include/object_model/parameter_types/integer.h
registration_source: src/object_model/parameter_types/integer_arit.C
registration_description: Modulo the first operand by the rest.
status: scaffold
---

# `integer::mod`

## Summary

Modulo the first operand by the rest.

## Exposed class

- `IntegerModulo`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operands` | const integer object sequence of size 2 | The operands for this function. |

## Construction paths

- Direct-use path: `IntegerModulo(std::vector<std::unique_ptr<Integer>>)`
- Registration/build path: `IntegerModulo(const BlockModel&)`

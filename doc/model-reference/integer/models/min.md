---
doc_type: model
component: integer
model: min
registered_name: min
base_model: component
implementation_class: IntegerMin
implementation_header: include/object_model/parameter_types/integer.h
registration_source: src/object_model/parameter_types/integer_arit.C
registration_description: Use the smallest value of its operands.
status: scaffold
---

# `integer::min`

## Summary

Use the smallest value of its operands.

## Exposed class

- `IntegerMin`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operands` | const variable integer object sequence | The operands for this function. |

## Construction paths

- Direct-use path: `IntegerMin(std::vector<std::unique_ptr<Integer>>)`
- Registration/build path: `IntegerMin(const BlockModel&)`

---
doc_type: model
component: integer
model: max
registered_name: max
base_model: component
implementation_class: IntegerMax
implementation_header: include/object_model/parameter_types/integer.h
registration_source: src/object_model/parameter_types/integer_arit.C
registration_description: Use the largest value of its operands.
status: scaffold
---

# `integer::max`

## Summary

Use the largest value of its operands.

## Exposed class

- `IntegerMax`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operands` | const variable integer object sequence | The operands for this function. |

## Construction paths

- Direct-use path: `IntegerMax(std::vector<std::unique_ptr<Integer>>)`
- Registration/build path: `IntegerMax(const BlockModel&)`

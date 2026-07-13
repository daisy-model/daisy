---
doc_type: model
component: integer
model: "+"
registered_name: "+"
base_model: component
implementation_class: IntegerSum
implementation_header: include/object_model/parameter_types/integer.h
registration_source: src/object_model/parameter_types/integer_arit.C
registration_description: Use the sum of its operands.
status: scaffold
---

# `integer::+`

## Summary

Use the sum of its operands.

## Exposed class

- `IntegerSum`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operands` | const variable integer object sequence | The operands for this function. |

## Construction paths

- Direct-use path: `IntegerSum(std::vector<std::unique_ptr<Integer>>)`
- Registration/build path: `IntegerSum(const BlockModel&)`

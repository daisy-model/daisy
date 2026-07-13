---
doc_type: model
component: integer
model: "*"
registered_name: "*"
base_model: component
implementation_class: IntegerProduct
implementation_header: include/object_model/parameter_types/integer.h
registration_source: src/object_model/parameter_types/integer_arit.C
registration_description: Use the product of its operands.
status: scaffold
---

# `integer::*`

## Summary

Use the product of its operands.

## Exposed class

- `IntegerProduct`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operands` | const variable integer object sequence | The operands for this function. |

## Construction paths

- Direct-use path: `IntegerProduct(std::vector<std::unique_ptr<Integer>>)`
- Registration/build path: `IntegerProduct(const BlockModel&)`

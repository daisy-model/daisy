---
doc_type: model
component: boolean
model: numbers
registered_name: numbers
base_model: component
implementation_class: BooleanNumbers
implementation_header: include/object_model/parameter_types/boolean.h
registration_source: src/object_model/parameter_types/boolean_number.C
registration_description: Base class for boolean expressions involving numbers.
status: scaffold
---

# `boolean::numbers`

## Summary

Base class for boolean expressions involving numbers.

## Exposed class

- `BooleanNumbers`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operands` | const variable number object sequence | List of operands to compare. |

## Construction paths

- Direct-use path: derived classes accept `std::vector<std::unique_ptr<Number>>`
- Registration/build path: `BooleanNumbers(const BlockModel&)`

## TODO

- Document dimension compatibility rules across operands.
- Explain how derived classes interpret operand order.

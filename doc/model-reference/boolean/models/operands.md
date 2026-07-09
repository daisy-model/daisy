---
doc_type: model
component: boolean
model: operands
registered_name: operands
base_model: component
implementation_class: BooleanOperands
implementation_header: include/object_model/parameter_types/boolean.h
registration_source: src/object_model/parameter_types/boolean.C
registration_description: Base class for boolean expressions involving multiple boolean operands.
status: scaffold
---

# `boolean::operands`

## Summary

Base class for boolean expressions involving multiple boolean operands.

## Exposed class

- `BooleanOperands`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operands` | const variable boolean object sequence | List of operands to compare. |

## Construction paths

- Registration/build path: `BooleanOperands(const BlockModel&)`

## TODO

- Document ownership and lifecycle expectations for built operand models.
- Add direct-construction documentation when a non-`BlockModel` constructor is introduced.
- Explain how derived classes narrow operand cardinality or semantics.

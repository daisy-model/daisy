---
doc_type: model
component: boolean
model: xor
registered_name: xor
base_model: component
implementation_class: BooleanXOr
implementation_header: include/object_model/parameter_types/boolean.h
registration_source: src/object_model/parameter_types/boolean.C
registration_description: True if and only if one operand is true, and one false.
status: scaffold
---

# `boolean::xor`

## Summary

True if and only if one operand is true, and one false.

## Exposed class

- `BooleanXOr`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operands` | const 2-element boolean object sequence | The two operands to compare. |

## Construction paths

- Registration/build path: `BooleanXOr(const BlockModel&)`

## TODO

- Add direct-construction documentation when a non-`BlockModel` constructor is introduced.
- Document why the registration tree treats this as a direct `component` child even though the implementation reuses `BooleanOperands`.
- Add an example showing the fixed two-operand shape.

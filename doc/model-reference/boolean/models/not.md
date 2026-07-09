---
doc_type: model
component: boolean
model: not
registered_name: not
base_model: component
implementation_class: BooleanNot
implementation_header: include/object_model/parameter_types/boolean.h
registration_source: src/object_model/parameter_types/boolean.C
registration_description: True if and only if the operand is not true.
status: scaffold
---

# `boolean::not`

## Summary

True if and only if the operand is not true.

## Exposed class

- `BooleanNot`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operands` | const 1-element boolean object sequence | The operand to check. |

## Construction paths

- Registration/build path: `BooleanNot(const BlockModel&)`

## TODO

- Add direct-construction documentation when a non-`BlockModel` constructor is introduced.
- Document why the registration tree treats this as a direct `component` child even though the implementation reuses `BooleanOperands`.
- Add an example showing the fixed one-operand shape.

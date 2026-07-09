---
doc_type: model
component: integer
model: const
registered_name: const
base_model: component
implementation_class: IntegerConst
implementation_header: include/object_model/parameter_types/integer.h
registration_source: src/object_model/parameter_types/integer.C
registration_description: Always give the specified value.
status: scaffold
---

# `integer::const`

## Summary

Always give the specified value.

## Exposed class

- `IntegerConst`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `value` | const integer | Fixed value for this integer. |

## Construction paths

- Direct-use path: `IntegerConst(int)`
- Registration/build path: `IntegerConst(const BlockModel&)`

## TODO

- Document whether integer bounds or other validation should become part of the public contract.
- Add an example showing how `value` appears in Daisy input.

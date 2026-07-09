---
doc_type: model
component: number
model: const
registered_name: const
base_model: component
implementation_class: NumberConst
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_const.C
registration_description: Always give the specified value.
status: scaffold
---

# `number::const`

## Summary

Always give the specified value.

## Exposed class

- `NumberConst`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `value` | user const number | Fixed value for this number. |

## Construction paths

- Direct-use path: `NumberConst(double, const Unit&)`
- Registration/build path: `NumberConst(const BlockModel&)`

## TODO

- Consider whether a direct-use convenience constructor should accept a unit symbol once unit lookup ownership is clearer.
- Add an example showing how `value` appears in Daisy input.

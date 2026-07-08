---
doc_type: model
component: function
model: const
registered_name: const
base_model: component
implementation_class: FunctionConst
implementation_header: include/object_model/function.h
registration_source: src/object_model/function.C
registration_description: Always return the same number.
status: scaffold
---

# `function::const`

## Summary

Always return the same number.

## Exposed class

- `FunctionConst`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `value` | user const number | The number to return. |

## Construction paths

- Registration/build path: `FunctionConst(const BlockModel&)`

## TODO

- Add direct-construction documentation when a non-`BlockModel` constructor is introduced.
- Document how units and dimensions are expected to interact with constant functions.
- Add an example showing how `value` appears in Daisy input.

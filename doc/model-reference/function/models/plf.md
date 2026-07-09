---
doc_type: model
component: function
model: plf
registered_name: plf
base_model: component
implementation_class: FunctionPLF
implementation_header: include/object_model/function.h
registration_source: src/object_model/function.C
registration_description: A piecewise linear function.
status: scaffold
---

# `function::plf`

## Summary

A piecewise linear function.

## Exposed class

- `FunctionPLF`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `plf` | user user const plf | The piecewise linear function. |

## Construction paths

- Direct-use path: `FunctionPLF(const PLF&)`
- Registration/build path: `FunctionPLF(const BlockModel&)`

## TODO

- Document interpolation and boundary behavior for piecewise linear functions.
- Add an example of the expected PLF input shape in Daisy configuration files.

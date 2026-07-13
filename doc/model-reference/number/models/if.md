---
doc_type: model
component: number
model: if
registered_name: if
base_model: component
implementation_class: NumberIf
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_lisp.C
registration_description: Select between two numbers depending on a boolean expression.
status: scaffold
---

# `number::if`

## Summary

Select between two numbers depending on a boolean expression.

## Exposed class

- `NumberIf`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `if` | state singleton boolean object | Select which number to use. |
| `then` | state singleton number object | Use this if true. |
| `else` | state singleton number object | Use this if false. |

## Construction paths

- Direct-use path: `NumberIf(std::unique_ptr<Boolean>, std::unique_ptr<Number>, std::unique_ptr<Number>)`
- Registration/build path: `NumberIf(const BlockModel&)`

## TODO

- Document how differing branch dimensions produce an unknown output dimension.
- Add an example combining `if` with other exposed number models.

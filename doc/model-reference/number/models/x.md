---
doc_type: model
component: number
model: x
registered_name: x
base_model: component
implementation_class: NumberX
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_const.C
registration_description: The value of the symbol 'x' in the current scope.
status: scaffold
---

# `number::x`

## Summary

The value of the symbol `x` in the current scope.

## Exposed class

- `NumberX`

## Declared entries

This model does not declare additional frame entries.

## Construction paths

- Direct-use path: `NumberX()`
- Registration/build path: `NumberX(const BlockModel&)`

## TODO

- Document which callers are expected to provide `x` in scope.
- Add an example showing how `x` participates in nested numeric expressions.

---
doc_type: model
component: number
model: identity
registered_name: identity
base_model: child
implementation_class: NumberIdentity
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_const.C
registration_description: Pass value unchanged.
status: scaffold
---

# `number::identity`

## Summary

Pass the child value through unchanged, with optional dimension assertion or conversion.

## Exposed class

- `NumberIdentity`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `dimension` | optional const string | Dimension of this value. |

## Construction paths

- Direct-use path: `NumberIdentity(std::unique_ptr<Number>, const Units&)`
- Direct-use path with dimension: `NumberIdentity(std::unique_ptr<Number>, const Units&, symbol)`
- Registration/build path: `NumberIdentity(const BlockModel&)`

## TODO

- Document when `identity` converts versus only reuses the child dimension.
- Add an example showing optional dimension coercion.

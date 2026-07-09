---
doc_type: model
component: number
model: dim
registered_name: dim
base_model: child
implementation_class: NumberDim
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_const.C
registration_description: Specify dimension for number.
status: scaffold
---

# `number::dim`

## Summary

Specify a dimension for the child number without changing its numeric value.

## Exposed class

- `NumberDim`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `warn_known` | const boolean | Issue a warning if the child dimension is already known. |
| `dimension` | const string | Dimension to use. |

## Construction paths

- Direct-use path: `NumberDim(std::unique_ptr<Number>, symbol, bool)`
- Registration/build path: `NumberDim(const BlockModel&)`

## TODO

- Clarify when `dim` should be preferred over `identity` or `convert`.
- Add an example showing the asserted-dimension workflow.

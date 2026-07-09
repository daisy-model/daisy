---
doc_type: model
component: number
model: convert
registered_name: convert
base_model: child
implementation_class: NumberConvert
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_const.C
registration_description: Convert to specified dimension.
status: scaffold
---

# `number::convert`

## Summary

Convert the child value into a specified dimension.

## Exposed class

- `NumberConvert`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `dimension` | const string | Dimension to convert to. |

## Construction paths

- Direct-use path: `NumberConvert(std::unique_ptr<Number>, const Units&, symbol)`
- Registration/build path: `NumberConvert(const BlockModel&)`

## TODO

- Document conversion failure behavior for incompatible units.
- Add an example showing a converted child number.

---
doc_type: model
component: number
model: get
registered_name: get
base_model: component
implementation_class: NumberGet
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_const.C
registration_description: Get the value of symbol in the current scope.
status: scaffold
---

# `number::get`

## Summary

Get the value of a named symbol in the current scope.

## Exposed class

- `NumberGet`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `name` | const string | Name of the symbol to read. |
| `dimension` | const string | Expected dimension for the symbol. |

## Construction paths

- Direct-use path: `NumberGet(symbol, const Unit&)`
- Registration/build path: `NumberGet(const BlockModel&)`

## TODO

- Document error behavior for missing or non-number scope entries.
- Add an example showing a converted lookup from scope.

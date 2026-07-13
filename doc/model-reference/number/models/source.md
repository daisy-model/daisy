---
doc_type: model
component: number
model: source
registered_name: source
base_model: component
implementation_class: NumberSource
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_source.C
registration_description: Extract information from a time series.
status: scaffold
---

# `number::source`

## Summary

Base model for extracting a numeric result from a time series source, optionally filtered by begin/end bounds.

## Exposed class

- `NumberSource`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `source` | state singleton source object | The time series we want to extract a number from. |
| `begin` | optional const submodule | Ignore values before or at this date. |
| `end` | optional const submodule | Ignore values after this date. |

## Construction paths

- Direct-use path: derived classes accept `std::unique_ptr<Source>` plus optional `std::unique_ptr<const Time>` bounds
- Registration/build path: `NumberSource(const BlockModel&)`

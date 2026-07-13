---
doc_type: model
component: rate
model: rate
registered_name: rate
base_model: component
implementation_class: RateRate
implementation_header: include/object_model/rate.h
registration_source: src/object_model/rate.C
registration_description: Specify rate directly.
status: scaffold
---

# `rate::rate`

## Summary

Specify a rate directly in `h^-1`.

## Exposed class

- `RateRate`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `rate` | const number in `h^-1` | Rate to use. |

## Construction paths

- Direct-use path: `RateRate(double)`
- Registration/build path: `RateRate(const BlockModel&)`

---
doc_type: model
component: rate
model: halftime
registered_name: halftime
base_model: component
implementation_class: RateHalftime
implementation_header: include/object_model/rate.h
registration_source: src/object_model/rate.C
registration_description: A rate specified through the equivalent halftime (rate = ln 2 / halftime).
status: scaffold
---

# `rate::halftime`

## Summary

Specify a rate through the equivalent halftime, using `ln(2) / halftime`.

## Exposed class

- `RateHalftime`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `halftime` | const positive number in `h` | Halftime of the rate to use. |

## Construction paths

- Direct-use path: `RateHalftime(double)`
- Registration/build path: `RateHalftime(const BlockModel&)`

## TODO

- Clarify whether callers should preserve the original halftime separately when formatting user-facing output.

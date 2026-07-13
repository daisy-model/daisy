---
doc_type: model
component: unit
model: factor
registered_name: factor
base_model: component
implementation_class: UnitFactor
implementation_header: include/object_model/unit_model.h
registration_source: src/object_model/unit_model.C
registration_description: Connvert to base units by multiplying with a factor.
status: scaffold
---

# `unit::factor`

## Summary

Convert to a named base unit by multiplying with a fixed factor.

## Exposed class

- `UnitFactor`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `base` | const string | Base unit to convert to and from. |
| `factor` | const scalar | Factor to multiply with to get base unit. |

## Construction paths

- Direct-use path: `UnitFactor(symbol name, symbol base, double factor)`
- Registration/build path: `UnitFactor(const BlockModel&)`

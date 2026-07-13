---
doc_type: model
component: unit
model: sifactor
registered_name: SIfactor
base_model: SI
implementation_class: UnitSIFactor
implementation_header: include/object_model/unit_model.h
registration_source: src/object_model/unit_model.C
registration_description: Connvert to SI base units by multiplying with a factor.
status: scaffold
---

# `unit::SIfactor`

## Summary

Convert to an SI-composed base unit by multiplying with a fixed factor.

## Exposed class

- `UnitSIFactor`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `factor` | const scalar | Factor to multiply with to get base unit. |

## Construction paths

- Direct-use path: `UnitSIFactor(symbol name, symbol base, double factor)`
- Registration/build path: `UnitSIFactor(const BlockModel&)`

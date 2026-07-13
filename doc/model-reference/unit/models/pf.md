---
doc_type: model
component: unit
model: pf
registered_name: pF
base_model: component
implementation_class: UnitpF
implementation_header: include/object_model/unit_model.h
registration_source: src/object_model/unit_model.C
registration_description: log10 (- cmH2O).
status: scaffold
---

# `unit::pF`

## Summary

Represent soil water pressure as `log10(-cmH2O)`.

## Exposed class

- `UnitpF`

## Construction paths

- Direct-use path: `UnitpF()`
- Registration/build path: `UnitpF(const BlockModel&)`

---
doc_type: model
component: unit
model: base
registered_name: base
base_model: component
implementation_class: UnitBase
implementation_header: include/object_model/unit_model.h
registration_source: src/object_model/unit_model.C
registration_description: A base unit.
status: scaffold
---

# `unit::base`

## Summary

Declare a base unit whose native and base dimensions are identical.

## Exposed class

- `UnitBase`

## Construction paths

- Direct-use path: `UnitBase(symbol name)`
- Registration/build path: `UnitBase(const BlockModel&)`

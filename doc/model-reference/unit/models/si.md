---
doc_type: model
component: unit
model: si
registered_name: SI
base_model: component
implementation_class: UnitSI
implementation_header: include/object_model/unit_model.h
registration_source: src/object_model/unit_model.C
registration_description: Base parameterization for all SI based units.
status: scaffold
---

# `unit::SI`

## Summary

Abstract base model for units whose base dimension is composed from SI exponents.

## Exposed class

- `UnitSI`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `length` | const integer | Dimension, base unit `[m]`. |
| `mass` | const integer | Dimension, base unit `[kg]`. |
| `time` | const integer | Dimension, base unit `[s]`. |
| `electric_current` | const integer | Dimension, base unit `[A]`. |
| `thermodynamic_temperature` | const integer | Dimension, base unit `[K]`. |
| `amount_of_substance` | const integer | Dimension, base unit `[mol]`. |
| `luminous_intensity` | const integer | Dimension, base unit `[cd]`. |

## Construction paths

- Registration/build path: `UnitSI(const BlockModel&)`

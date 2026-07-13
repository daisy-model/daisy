---
doc_type: component
component: unit
registered_name: unit
implementation_header: include/object_model/unit_model.h
registration_source: src/object_model/unit_model.C
registration_description: Specify units of physical quatities.
shared_class: MUnit
shared_base_model: component
status: scaffold
---

# `unit` component

## Summary

Specify units of physical quantities and convert between compatible native and base dimensions.

## Exposed classes

| Class | Role |
| --- | --- |
| `MUnit` | Abstract base class that combines `Model` registration with the `Unit` conversion interface. |
| `UnitSI` | Abstract helper base for units whose base dimension is assembled from SI exponents. |
| `UnitSIFactor` | Concrete SI-derived unit that converts by multiplying with a fixed factor. |
| `UnitpF` | Concrete pressure unit expressed as `log10(-cmH2O)`. |
| `UnitBase` | Concrete identity unit whose native and base names are the same. |
| `UnitFactor` | Concrete unit that converts to its base unit by multiplication only. |
| `UnitOffset` | Concrete unit that converts to its base unit by multiplication plus offset. |

## Shared declared entries

The component registration contributes only the standard `Model::load_model()` metadata.

## Registered models

- `SI`
- `SIfactor`
- `pF`
- `base`
- `factor`
- `offset`

## TODO

- Document the most important built-in parameterizations generated from `SIfactor`, `base`, `factor`, and `offset`.
- Add examples showing how custom units are declared in inputs.

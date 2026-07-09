---
doc_type: component
component: number
registered_name: number
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number.C
registration_description: Generic representation of numbers.
shared_class: Number
shared_base_model: component
status: scaffold
---

# `number` component

## Summary

Generic representation of numbers.

## Exposed classes

| Class | Role |
| --- | --- |
| `Number` | Abstract base class for numeric models. |
| `NumberConst` | Concrete model that always returns the same numeric value and dimension. |

## Shared declared entries

The component registration in `src/object_model/parameter_types/number.C`
currently does not add shared frame entries at the component level.

## Registered models

- `const`

## TODO

- Expose and document additional low-risk number models once the constant slice is established.
- Add usage examples showing how number models are embedded in Daisy inputs.

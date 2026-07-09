---
doc_type: component
component: integer
registered_name: integer
implementation_header: include/object_model/parameter_types/integer.h
registration_source: src/object_model/parameter_types/integer.C
registration_description: Generic representation of integers.
shared_class: Integer
shared_base_model: component
status: scaffold
---

# `integer` component

## Summary

Generic representation of integers.

## Exposed classes

| Class | Role |
| --- | --- |
| `Integer` | Abstract base class for integer-valued models. |
| `IntegerConst` | Concrete model that always returns the same integer value. |

## Shared declared entries

The component registration in `src/object_model/parameter_types/integer.C`
currently does not add shared frame entries at the component level.

## Registered models

- `const`
- `cond`

## TODO

- Expose and document `IntegerCond` after the low-risk `IntegerConst` slice is established.
- Document lifecycle expectations for `initialize`, `check`, and `missing`.
- Add usage examples showing how integer models are embedded in Daisy inputs.

---
doc_type: component
component: string
registered_name: string
implementation_header: include/object_model/parameter_types/stringer.h
registration_source: src/object_model/parameter_types/stringer.C
registration_description: Generic representation of strings.
shared_class: Stringer
shared_base_model: component
status: scaffold
---

# `string` component

## Summary

Generic representation of strings.

## Exposed classes

| Class | Role |
| --- | --- |
| `Stringer` | Abstract base class for string-valued models. |
| `StringerNumber` | Shared abstract base class for string models backed by numbers. |
| `StringerValue` | Concrete model that renders a number as a string. |
| `StringerDimension` | Concrete model that extracts a number dimension as a string. |
| `StringerIdentity` | Concrete model that always returns the same string. |

## Shared declared entries

The component registration in `src/object_model/parameter_types/stringer.C`
currently does not add shared frame entries at the component level.

## Registered models

- `cond`
- `number`
- `value`
- `dimension`
- `identity`

## TODO

- Expose and document `StringerCond` after the low-risk slices are established.
- Add usage examples showing how string models are embedded in Daisy inputs.

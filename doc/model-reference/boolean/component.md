---
doc_type: component
component: boolean
registered_name: boolean
implementation_header: include/object_model/parameter_types/boolean.h
registration_source: src/object_model/parameter_types/boolean.C
registration_description: Generic representation of booleans.
shared_class: Boolean
shared_base_model: component
status: scaffold
---

# `boolean` component

## Summary

Generic representation of booleans.

## Exposed classes

| Class | Role |
| --- | --- |
| `Boolean` | Abstract base class for boolean-valued models. |
| `BooleanTrue` | Concrete model that always evaluates to true. |
| `BooleanFalse` | Concrete model that always evaluates to false. |

## Shared declared entries

The component registration in `src/object_model/parameter_types/boolean.C`
currently does not add shared frame entries at the component level.

## Registered models

- `true`
- `false`
- `operands`
- `and`
- `or`
- `xor`
- `not`

## TODO

- Document the lifecycle expectations for `initialize`, `check`, `tick`, and `missing`.
- Extend the exposed-class list when operand-based boolean classes are promoted to headers.
- Add usage examples showing how boolean models are embedded in Daisy inputs.

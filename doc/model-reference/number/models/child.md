---
doc_type: model
component: number
model: child
registered_name: child
base_model: component
implementation_class: NumberChild
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_const.C
registration_description: Numbers based on another number.
status: scaffold
---

# `number::child`

## Summary

Base class for number models that wrap another number.

## Exposed class

- `NumberChild`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `value` | state singleton number object | Operand for this number. |

## Construction paths

- Direct-use path: derived classes accept `std::unique_ptr<Number>`
- Registration/build path: `NumberChild(const BlockModel&)`

## TODO

- Document ownership and lifecycle expectations for the wrapped child number.
- Explain how derived classes specialize the wrapped value.

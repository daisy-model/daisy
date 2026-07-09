---
doc_type: model
component: string
model: dimension
registered_name: dimension
base_model: number
implementation_class: StringerDimension
implementation_header: include/object_model/parameter_types/stringer.h
registration_source: src/object_model/parameter_types/stringer.C
registration_description: Extract the dimension of a number as a string.
status: scaffold
---

# `string::dimension`

## Summary

Extract the dimension of a number as a string.

## Exposed class

- `StringerDimension`

## Declared entries

This model currently relies on the inherited `number` declaration from
`string::number`.

## Construction paths

- Registration/build path: `StringerDimension(const BlockModel&)`

## TODO

- Document the exact dimension string format exposed to callers.
- Consider whether a direct-use constructor should be introduced once number ownership is redesigned.

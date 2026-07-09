---
doc_type: model
component: string
model: number
registered_name: number
base_model: component
implementation_class: StringerNumber
implementation_header: include/object_model/parameter_types/stringer.h
registration_source: src/object_model/parameter_types/stringer.C
registration_description: Extract the value of a number.
status: scaffold
---

# `string::number`

## Summary

Extract the value of a number.

## Exposed class

- `StringerNumber`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `number` | state singleton number object | Number to manipulate. |

## Construction paths

- Registration/build path: `StringerNumber(const BlockModel&)`

## TODO

- Document why this registration base maps to an abstract C++ helper class.
- Consider whether number-backed string classes should share a direct-use ownership API.

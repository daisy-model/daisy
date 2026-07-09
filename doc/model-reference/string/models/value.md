---
doc_type: model
component: string
model: value
registered_name: value
base_model: number
implementation_class: StringerValue
implementation_header: include/object_model/parameter_types/stringer.h
registration_source: src/object_model/parameter_types/stringer.C
registration_description: Extract the value of a number as a string.
status: scaffold
---

# `string::value`

## Summary

Extract the value of a number as a string.

## Exposed class

- `StringerValue`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `precision` | optional const integer | Number of decimals after point. By default, use a floating format. |

## Construction paths

- Registration/build path: `StringerValue(const BlockModel&)`

## TODO

- Document formatting semantics more precisely, including rounding and locale expectations.
- Consider whether a direct-use constructor should be introduced once number ownership is redesigned.

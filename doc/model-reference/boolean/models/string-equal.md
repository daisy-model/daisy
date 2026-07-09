---
doc_type: model
component: boolean
model: string-equal
registered_name: string-equal
base_model: component
implementation_class: BooleanStringEqual
implementation_header: include/object_model/parameter_types/boolean.h
registration_source: src/object_model/parameter_types/boolean_string.C
registration_description: True iff the supplied strings are identical.
status: scaffold
---

# `boolean::string-equal`

## Summary

True iff the supplied strings are identical.

## Exposed class

- `BooleanStringEqual`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `values` | const variable string sequence | Strings to compare. |

## Construction paths

- Direct-use path: `BooleanStringEqual(const std::vector<symbol>&)`
- Registration/build path: `BooleanStringEqual(const BlockModel&)`

## TODO

- Document whether fewer than two values being treated as equal should remain part of the public contract.
- Add an example showing a multi-value comparison in Daisy input.

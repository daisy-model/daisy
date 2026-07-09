---
doc_type: model
component: string
model: identity
registered_name: identity
base_model: component
implementation_class: StringerIdentity
implementation_header: include/object_model/parameter_types/stringer.h
registration_source: src/object_model/parameter_types/stringer.C
registration_description: Return the specified value.
status: scaffold
---

# `string::identity`

## Summary

Return the specified value.

## Exposed class

- `StringerIdentity`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `value` | const string | Constant value. |

## Construction paths

- Direct-use path: `StringerIdentity(symbol)`
- Registration/build path: `StringerIdentity(const BlockModel&)`

## TODO

- Add an example showing how `value` appears in Daisy input.
- Decide whether a `const char*` or `std::string` convenience constructor is worth adding.

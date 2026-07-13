---
doc_type: model
component: string
model: cond
registered_name: cond
base_model: component
implementation_class: StringerCond
implementation_header: include/object_model/parameter_types/stringer.h
registration_source: src/object_model/parameter_types/stringer.C
registration_description: Return the value of the first clause whose condition is true.
status: scaffold
---

# `string::cond`

## Summary

Return the string value from the first clause whose boolean condition is true.

## Exposed class

- `StringerCond`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `clauses` | const variable submodel sequence | List of clauses to match for. |

## Construction paths

- Direct-use path: `StringerCond(std::vector<StringerCond::Clause>)`
- Registration/build path: `StringerCond(const BlockModel&)`

## TODO

- Document clause ordering and failure behavior when no clause matches.
- Add an example showing a conditional string expression in Daisy input.

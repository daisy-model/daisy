---
doc_type: model
component: integer
model: cond
registered_name: cond
base_model: component
implementation_class: IntegerCond
implementation_header: include/object_model/parameter_types/integer.h
registration_source: src/object_model/parameter_types/integer.C
registration_description: Return the value of the first clause whose condition is true.
status: scaffold
---

# `integer::cond`

## Summary

Return the integer value from the first clause whose boolean condition is true.

## Exposed class

- `IntegerCond`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `clauses` | const variable submodel sequence | List of clauses to match for. |

## Construction paths

- Direct-use path: `IntegerCond(std::vector<IntegerCond::Clause>)`
- Registration/build path: `IntegerCond(const BlockModel&)`

## TODO

- Document clause ordering and failure behavior when no clause matches.
- Add an example showing a conditional integer expression in Daisy input.

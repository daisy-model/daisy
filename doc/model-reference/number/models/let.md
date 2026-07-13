---
doc_type: model
component: number
model: let
registered_name: let
base_model: component
implementation_class: NumberLet
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_lisp.C
registration_description: Bind symbols in 'clauses' in a new scope, and evaluate 'expr' in that scope.
status: scaffold
---

# `number::let`

## Summary

Bind temporary number identifiers in a nested scope, then evaluate an expression against that scope.

## Exposed classes

- `NumberLet`
- `NumberLet::Clause`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `clauses` | const submodule sequence | List of identifiers and values to bind in this scope. |
| `expr` | state singleton number object | Expression to evaluate. |

## Construction paths

- Direct-use path: `NumberLet(std::vector<NumberLet::Clause>, std::unique_ptr<Number>)`
- Registration/build path: `NumberLet(const BlockModel&)`

## TODO

- Document shadowing and lookup behavior relative to the inherited scope.
- Add an example showing a bound symbol reused multiple times in the final expression.

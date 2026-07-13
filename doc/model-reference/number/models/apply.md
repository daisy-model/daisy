---
doc_type: model
component: number
model: apply
registered_name: apply
base_model: component
implementation_class: NumberApply
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_apply.C
registration_description: Apply function to operand.
status: scaffold
---

# `number::apply`

## Summary

Apply a function model to a numeric operand and expose the function range as the resulting dimension.

## Exposed class

- `NumberApply`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `function` | user user function | Function to apply. |
| `operand` | user const value | Operand for this function. |

## Construction paths

- Direct-use path: `NumberApply(std::unique_ptr<Function>, double, symbol)`
- Registration/build path: `NumberApply(const BlockModel&)`

## TODO

- Document the domain/range compatibility requirements enforced by registration.
- Add an example using a piecewise linear function as the applied function.

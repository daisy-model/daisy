---
doc_type: model
component: number
model: exp
registered_name: exp
base_model: component
implementation_class: NumberExp
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_arit.C
registration_description: Take the exponential of its argument.
status: scaffold
---

# `number::exp`

## Summary

Take the exponential of the operand.

## Exposed class

- `NumberExp`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operand` | state singleton number object | Operand for this function. |

## Construction paths

- Direct-use path: `NumberExp(std::unique_ptr<Number>)`
- Registration/build path: `NumberExp(const BlockModel&)`

## TODO

- Document dimension expectations for exponential inputs.
- Add examples showing how exponential number models are used in Daisy inputs.

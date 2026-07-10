---
doc_type: model
component: number
model: plf
registered_name: plf
base_model: component
implementation_class: NumberPLF
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_plf.C
registration_description: Look up argumen in a piecewise linear function.
status: scaffold
---

# `number::plf`

## Summary

Look up an operand value in a piecewise linear function and return the mapped result.

## Exposed class

- `NumberPLF`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operand` | state singleton number object | Operand for this function. |
| `domain` | const string | Unit for the operand of the function. |
| `range` | const string | Unit for the value of the function. |
| `points` | const variable submodel sequence | Ordered `(x, y)` points defining the piecewise linear mapping. |

## Construction paths

- Direct-use path: `NumberPLF(std::unique_ptr<Number>, symbol, symbol, const PLF&)`
- Registration/build path: `NumberPLF(const BlockModel&)`

## TODO

- Document extrapolation semantics and ordering requirements more precisely.
- Add an example showing unit conversion across the PLF domain and range.

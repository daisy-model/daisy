---
doc_type: component
component: number
registered_name: number
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number.C
registration_description: Generic representation of numbers.
shared_class: Number
shared_base_model: component
status: scaffold
---

# `number` component

## Summary

Generic representation of numbers.

## Exposed classes

| Class | Role |
| --- | --- |
| `Number` | Abstract base class for numeric models. |
| `NumberConst` | Concrete model that always returns the same numeric value and dimension. |
| `NumberX` | Reads the current scope symbol `x`. |
| `NumberGet` | Reads a named number from scope and converts it to a requested dimension. |
| `NumberFetchGet` | Scope-backed helper that reads a named number without coercing its dimension. |
| `NumberFetch` | Concrete model that resolves a named number value from the current scope or from a declared numeric/model parameter. |
| `NumberApply` | Applies a registered function to a numeric operand and returns the function range dimension. |
| `NumberChild` | Abstract base for number models that wrap another `Number`. |
| `NumberIdentity` | Pass-through wrapper that can optionally assert or convert dimension. |
| `NumberConvert` | Wrapper that converts the child value into a requested dimension. |
| `NumberDim` | Wrapper that re-labels the child value with an asserted dimension. |
| `NumberOperand` | Abstract helper for unary arithmetic numbers backed by one operand. |
| `NumberLog10` | Base-10 logarithm of the operand. |
| `NumberLn` | Natural logarithm of the operand. |
| `NumberExp` | Exponential of the operand. |
| `NumberSqrt` | Square root of the operand. |
| `NumberSqr` | Square of the operand. |
| `NumberPow` | Raises one number to the power of another. |
| `NumberOperands` | Abstract helper for arithmetic number models backed by an operand sequence. |
| `NumberMax` | Largest value from the operand sequence. |
| `NumberMin` | Smallest value from the operand sequence. |
| `NumberProduct` | Product of the operand sequence. |
| `NumberSum` | Sum of the operand sequence. |
| `NumberSubtract` | Negation or left-associated subtraction across the operand sequence. |
| `NumberDivide` | Left-associated division across the operand sequence. |
| `NumberPLF` | Evaluates a number operand through a piecewise linear function. |

## Shared declared entries

The component registration in `src/object_model/parameter_types/number.C`
currently does not add shared frame entries at the component level.

## Registered models

- `const`
- `x`
- `get`
- `fetch`
- `child`
- `identity`
- `convert`
- `dim`
- `log10`
- `ln`
- `exp`
- `sqrt`
- `sqr`
- `pow`
- `apply`
- `max`
- `min`
- `*`
- `+`
- `-`
- `/`
- `plf`

## TODO

- Expose and document additional low-risk number models once the child-wrapper slice is established.
- Add usage examples showing how number models are embedded in Daisy inputs.

---
doc_type: component
component: function
registered_name: function
implementation_header: include/object_model/function.h
registration_source: src/object_model/function.C
registration_description: Pure function of one parameter.
shared_class: Function
shared_base_model: component
status: scaffold
---

# `function` component

## Summary

Pure function of one parameter.

## Exposed classes

| Class | Role |
| --- | --- |
| `Function` | Abstract base class for one-argument functions. |
| `FunctionPlotable` | Helper base class that samples a function into `x`/`y` vectors. |
| `FunctionConst` | Concrete function that always returns the same value. |
| `FunctionPLF` | Concrete function backed by a piecewise linear function. |

## Shared declared entries

The entries below are populated from `Model::load_model()` and the `Function`
component registration in `src/object_model/function.C`.

| Entry | Shape | Description |
| --- | --- | --- |
| `description` | optional const text | Description of this model or parameterization. The value will appear in the reference manual, and may also appear in some GUI front ends. |
| `cite` | const variable string sequence | BibTeX keys that would be relevant for this model or parameterization. |
| `domain` | const string | Function domain. |
| `range` | const string | Function range. |
| `formula` | optional const string | LaTeX formula for the function, for the reference manual. |

## Registered models

- `const`
- `plf`

## TODO

- Document semantic expectations for domain and range naming.
- Add examples that show how a function is referenced from other models.
- Record any invariants that direct constructors should preserve once they are
  added.

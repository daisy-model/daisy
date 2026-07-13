---
doc_type: model
component: function
model: python
registered_name: Python
base_model: component
implementation_class: FunctionPython
implementation_header: include/object_model/function.h
registration_source: src/object_model/function_Python.C
registration_description: Call Python function.
status: scaffold
---

# `function::Python`

## Summary

Call a Python function from a named module and treat the result as a one-argument function value.

## Exposed class

- `FunctionPython`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `module` | const string | Where to find the function. |
| `name` | const string | Name of the function. |
| `domain` | const string | Function domain. |
| `range` | const string | Function range. |

## Construction paths

- Direct-use path: `FunctionPython(symbol module, symbol name, symbol domain, symbol range)`
- Registration/build path: `FunctionPython(const BlockModel&)`

## TODO

- Document Python interpreter lifecycle expectations and import-path behavior.

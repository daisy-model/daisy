---
doc_type: model
component: number
model: fetch
registered_name: fetch
base_model: component
implementation_class: NumberFetch
implementation_header: include/object_model/parameter_types/number.h
registration_source: src/object_model/parameter_types/number_const.C
registration_description: Fetch the value and dimension in the current scope.
status: scaffold
---

# `number::fetch`

## Summary

Fetch the value and dimension for a named number from the current scope or from a declared numeric child parameter.

## Exposed classes

- `NumberFetch`
- `NumberFetchGet`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `name` | const string | Name of a the symbol. |

## Construction paths

- Direct-use helper path: `NumberFetchGet(symbol)`
- Direct-use path: `NumberFetch(std::unique_ptr<Number>)`
- Registration/build path: `NumberFetch(const BlockModel&)`

## TODO

- Document how `fetch` distinguishes scope lookups from embedded numeric/model parameters.
- Add examples for scope-backed and nested-model fetch usage.

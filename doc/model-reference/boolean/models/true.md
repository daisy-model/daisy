---
doc_type: model
component: boolean
model: true
registered_name: true
base_model: component
implementation_class: BooleanTrue
implementation_header: include/object_model/parameter_types/boolean.h
registration_source: src/object_model/parameter_types/boolean.C
registration_description: Always true.
status: scaffold
---

# `boolean::true`

## Summary

Always true.

## Exposed class

- `BooleanTrue`

## Declared entries

This model currently declares no model-specific frame entries.

## Construction paths

- Direct-use path: `BooleanTrue()`
- Registration/build path: `BooleanTrue(const BlockModel&)`

## TODO

- Document whether this model should be preferred over literal boolean flags in specific contexts.
- Add an example of where a `boolean::true` model is used in Daisy input.

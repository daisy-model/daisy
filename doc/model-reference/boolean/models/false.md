---
doc_type: model
component: boolean
model: false
registered_name: false
base_model: component
implementation_class: BooleanFalse
implementation_header: include/object_model/parameter_types/boolean.h
registration_source: src/object_model/parameter_types/boolean.C
registration_description: Always false.
status: scaffold
---

# `boolean::false`

## Summary

Always false.

## Exposed class

- `BooleanFalse`

## Declared entries

This model currently declares no model-specific frame entries.

## Construction paths

- Registration/build path: `BooleanFalse(const BlockModel&)`

## TODO

- Add direct-construction documentation when a non-`BlockModel` constructor is introduced.
- Document whether this model should be preferred over literal boolean flags in specific contexts.
- Add an example of where a `boolean::false` model is used in Daisy input.

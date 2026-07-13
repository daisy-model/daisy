---
doc_type: model
component: unit
model: offset
registered_name: offset
base_model: component
implementation_class: UnitOffset
implementation_header: include/object_model/unit_model.h
registration_source: src/object_model/unit_model.C
registration_description: Connvert to base units by multiplying factor, then substracting offset.
status: scaffold
---

# `unit::offset`

## Summary

Convert to a named base unit by multiplying with a factor and then applying an offset.

## Exposed class

- `UnitOffset`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `base` | const string | Base unit to convert to and from. |
| `factor` | const scalar | Factor to multiply with to get base unit. |
| `offset` | const scalar | Offset to add after multiplying with factor to get base unit. |

## Construction paths

- Direct-use path: `UnitOffset(symbol name, symbol base, double factor, double offset)`
- Registration/build path: `UnitOffset(const BlockModel&)`

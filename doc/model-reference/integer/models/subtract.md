---
doc_type: model
component: integer
model: "-"
registered_name: "-"
base_model: component
implementation_class: IntegerSubtract
implementation_header: include/object_model/parameter_types/integer.h
registration_source: src/object_model/parameter_types/integer_arit.C
registration_description: "Negate integer or subtract integers.\nWith one operand, negates it.  With more than one operand,\nsubtracts all but the first from the first."
status: scaffold
---

# `integer::-`

## Summary

Negate one integer or subtract later operands from the first.

## Exposed class

- `IntegerSubtract`

## Declared entries

| Entry | Shape | Description |
| --- | --- | --- |
| `operands` | const variable integer object sequence | The operands for this function. |

## Construction paths

- Direct-use path: `IntegerSubtract(std::vector<std::unique_ptr<Integer>>)`
- Registration/build path: `IntegerSubtract(const BlockModel&)`

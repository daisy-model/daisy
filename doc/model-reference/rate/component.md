---
doc_type: component
component: rate
registered_name: rate
implementation_header: include/object_model/rate.h
registration_source: src/object_model/rate.C
registration_description: Specify a rate or a halftime.
shared_class: Rate
shared_base_model: component
status: scaffold
---

# `rate` component

## Summary

Specify a rate directly or via an equivalent halftime.

## Exposed classes

| Class | Role |
| --- | --- |
| `Rate` | Abstract base class for models that resolve to an hourly rate. |
| `RateRate` | Concrete model that stores a rate directly in `h^-1`. |
| `RateHalftime` | Concrete model that converts a supplied halftime into a rate. |

## Shared declared entries

The `rate` component does not currently add shared frame entries at the component level.

## Registered models

- `rate`
- `halftime`

## Related parameterizations

- `zero` — alias of `rate` with the `rate` entry set to `0.0`.

## TODO

- Add examples showing where rate objects are embedded in higher-level models.
- Document any expectations around negative or zero rates beyond the registration checks.

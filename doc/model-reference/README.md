---
doc_type: model-reference-index
title: Model reference scaffolding
status: scaffold
render_targets:
  - markdown
  - html
source_of_truth:
  - registration metadata
  - exposed model class declarations
---

# Model reference scaffolding

This directory is the source tree for structured model documentation.

Each component or model is documented in its own Markdown file with YAML front
matter so the content is both human-readable in the repository and easy to turn
into HTML later with a static-site generator or a custom exporter.

Generated registration-derived scaffolding can also be emitted into a separate
output tree with:

```bash
python3 doc/model-reference/generate_from_registration.py \
  --repo-root . \
  --output-dir build/model-reference-generated \
  --source src/object_model/function.C \
  --source src/object_model/parameter_types/boolean.C \
  --source src/object_model/parameter_types/boolean_number.C \
  --source src/object_model/parameter_types/boolean_string.C \
  --source src/object_model/parameter_types/integer.C \
  --source src/object_model/parameter_types/number.C \
  --source src/object_model/parameter_types/number_arit.C \
  --source src/object_model/parameter_types/number_const.C \
  --source src/object_model/parameter_types/number_plf.C \
  --source src/object_model/parameter_types/stringer.C
```

That generated tree is meant to be the machine-derived baseline from
`DeclareComponent` / `DeclareModel` / `load_frame`, while the checked-in files in
this directory can evolve into an overlay for semantics, examples, and other
material that does not exist in registration metadata yet.

## Layout

- `component.md` describes a component-level API and shared declared entries.
- `models/<name>.md` describes one registered model and its exposed class.

## Required front matter

Component documents should include:

- `doc_type`
- `component`
- `registered_name`
- `implementation_header`
- `registration_source`

Model documents should include:

- `doc_type`
- `component`
- `model`
- `registered_name`
- `base_model`
- `implementation_class`
- `implementation_header`
- `registration_source`

## Content rules

1. Reuse registration text where possible instead of paraphrasing it.
2. Keep declared entries structured in tables or lists with stable field names.
3. Leave explicit `TODO` sections where semantics, examples, or caveats still
   need to be written.
4. Prefer one document per component or model so HTML generation can map files
   directly to pages.

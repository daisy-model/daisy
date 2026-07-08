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

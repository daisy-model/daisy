#!/usr/bin/env python3

from __future__ import annotations

import argparse
import pathlib
import re
from dataclasses import dataclass


@dataclass
class Entry:
    name: str
    registration: str
    description: str


@dataclass
class Declaration:
    kind: str
    component: str
    component_class: str
    registered_name: str
    base_model: str
    description: str
    implementation_class: str | None
    implementation_header: str | None
    source_path: str
    entries: list[Entry]


STRUCT_RE = re.compile(
    r"struct\s+(?P<name>\w+)\s*:\s*(?:public\s+)?(?P<kind>DeclareComponent|DeclareBase|DeclareModel)\s*\{(?P<body>.*?)\n\};",
    re.S,
)
COMPONENT_RE = re.compile(r'const char \*const\s+(\w+)::component\s*=\s*"([^"]+)";')
CLASS_HEADER_RE = re.compile(r"\bclass\s+(\w+)\b")
MAKE_RE = re.compile(r"return\s+new\s+(\w+)\s*\(al\);")
LOAD_FRAME_RE = re.compile(r"load_frame\s*\([^)]*Frame&[^)]*\)\s*const\s*\{(?P<body>.*?)\}", re.S)
DECLARE_CALL_RE = re.compile(r"frame\.(declare(?:_[A-Za-z]+)?|declare)\s*\((.*?)\);", re.S)
STRING_RE = re.compile(r'"((?:[^"\\]|\\.)*)"')


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo-root", required=True)
    parser.add_argument("--output-dir", required=True)
    parser.add_argument("--source", action="append", required=True)
    return parser.parse_args()


def split_top_level_args(argument_string: str) -> list[str]:
    args: list[str] = []
    current: list[str] = []
    depth = 0
    in_string = False
    escaped = False
    for char in argument_string:
        current.append(char)
        if in_string:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == '"':
                in_string = False
            continue
        if char == '"':
            in_string = True
        elif char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
        elif char == "," and depth == 0:
            current.pop()
            args.append("".join(current).strip())
            current = []
    tail = "".join(current).strip()
    if tail:
        args.append(tail)
    return args


def decode_cpp_string(value: str) -> str:
    return bytes(value, "utf-8").decode("unicode_escape")


def normalize_text(text: str) -> str:
    text = text.replace("\\\n", "")
    parts = [decode_cpp_string(match) for match in STRING_RE.findall(text)]
    if not parts:
        return ""
    normalized = " ".join(" ".join(parts).split())
    normalized = normalized.replace("\\ ", "").strip()
    if normalized.startswith("\\"):
        normalized = normalized[1:].strip()
    return normalized


def normalize_registration(call_name: str, args: list[str]) -> str:
    display_args: list[str] = []
    for part in args:
        stripped = part.strip()
        if '"' in stripped:
            display_args.append(f'"{normalize_text(stripped)}"')
        else:
            display_args.append(stripped)
    cleaned = ", ".join(display_args)
    cleaned = re.sub(r"\s+", " ", cleaned)
    return f"frame.{call_name}({cleaned})"


def find_header_for_class(repo_root: pathlib.Path, class_name: str) -> str | None:
    for header in repo_root.joinpath("include").rglob("*.h"):
        text = header.read_text()
        if re.search(rf"\bclass\s+{re.escape(class_name)}\b", text):
            return header.relative_to(repo_root).as_posix()
    return None


def parse_entries(load_frame_body: str) -> list[Entry]:
    entries: list[Entry] = []
    for match in DECLARE_CALL_RE.finditer(load_frame_body):
        call_name = match.group(1)
        call_args = split_top_level_args(match.group(2))
        literal_args = [normalize_text(arg) for arg in call_args if '"' in arg]
        if not literal_args:
            continue
        entry_name = literal_args[0]
        description = literal_args[-1] if len(literal_args) > 1 else ""
        entries.append(
            Entry(
                name=entry_name,
                registration=normalize_registration(call_name, call_args),
                description=description,
            )
        )
    return entries


def parse_source(repo_root: pathlib.Path, source: pathlib.Path) -> tuple[Declaration, list[Declaration]]:
    text = source.read_text()
    component_map = {f"{class_name}::component": component for class_name, component in COMPONENT_RE.findall(text)}
    if not component_map:
        raise ValueError(f"No component constants found in {source}")

    declarations: list[Declaration] = []
    for match in STRUCT_RE.finditer(text):
      kind = match.group("kind")
      body = match.group("body")
      syntax_name = match.group("name")
      constructor_match = re.search(
          rf"{re.escape(syntax_name)}\s*\(\)\s*:\s*{kind}\s*\((?P<args>.*?)\)\s*\{{",
          body,
          re.S,
      )
      if not constructor_match:
          continue
      constructor_args = split_top_level_args(constructor_match.group("args"))
      component_expr = constructor_args[0]
      component_name = component_map.get(component_expr)
      if not component_name:
          continue
      component_class = component_expr.split("::", 1)[0]
      string_values = [normalize_text(arg) for arg in constructor_args[1:] if '"' in arg]
      if kind == "DeclareComponent":
          registered_name = component_name
          base_model = "component"
          description = string_values[-1] if string_values else ""
      else:
          registered_name = string_values[0]
          description = string_values[-1]
          base_model = string_values[1] if len(string_values) >= 3 else "component"
      make_match = MAKE_RE.search(body)
      if make_match:
          implementation_class = make_match.group(1)
      elif kind == "DeclareComponent":
          implementation_class = component_class
      elif syntax_name.endswith("Syntax"):
          implementation_class = syntax_name[: -len("Syntax")]
      else:
          implementation_class = component_class
      implementation_header = find_header_for_class(repo_root, implementation_class)
      load_frame_match = LOAD_FRAME_RE.search(body)
      entries = parse_entries(load_frame_match.group("body")) if load_frame_match else []
      declarations.append(
          Declaration(
              kind=kind,
              component=component_name,
              component_class=component_class,
              registered_name=registered_name,
              base_model=base_model,
              description=description,
              implementation_class=implementation_class,
              implementation_header=implementation_header,
              source_path=source.relative_to(repo_root).as_posix(),
              entries=entries,
          )
      )

    component_declarations = [decl for decl in declarations if decl.kind == "DeclareComponent"]
    if len(component_declarations) != 1:
        raise ValueError(f"Expected exactly one DeclareComponent in {source}")
    component_decl = component_declarations[0]
    model_decls = [decl for decl in declarations if decl.kind != "DeclareComponent"]
    return component_decl, model_decls


def write_component_doc(output_dir: pathlib.Path, component_decl: Declaration, model_decls: list[Declaration]) -> None:
    component_dir = output_dir / component_decl.component
    component_dir.mkdir(parents=True, exist_ok=True)
    component_file = component_dir / "component.md"
    lines = [
        "---",
        "doc_type: component",
        f"component: {component_decl.component}",
        f"registered_name: {component_decl.component}",
        f"implementation_header: {component_decl.implementation_header or ''}",
        f"registration_source: {component_decl.source_path}",
        f"registration_description: {component_decl.description}",
        "generated_from_registration: true",
        "status: generated-scaffold",
        "---",
        "",
        f"# `{component_decl.component}` component",
        "",
        "## Summary",
        "",
        component_decl.description,
        "",
        "## Shared declared entries",
        "",
    ]
    if component_decl.entries:
        lines.extend(
            [
                "| Entry | Registration | Description |",
                "| --- | --- | --- |",
            ]
        )
        for entry in component_decl.entries:
            lines.append(f"| `{entry.name}` | `{entry.registration}` | {entry.description or '(none)'} |")
    else:
        lines.append("This component currently declares no shared frame entries.")
    lines.extend(
        [
            "",
            "## Registered models",
            "",
        ]
    )
    for model_decl in model_decls:
        lines.append(f"- `{model_decl.registered_name}`")
    lines.extend(
        [
            "",
            "## TODO",
            "",
            "- Add semantics, caveats, and examples that are not represented in registration metadata.",
            "- Keep this generated file as a machine-derived baseline and place hand-written overlay material elsewhere if needed.",
            "",
        ]
    )
    component_file.write_text("\n".join(lines))


def write_model_doc(output_dir: pathlib.Path, model_decl: Declaration) -> None:
    model_dir = output_dir / model_decl.component / "models"
    model_dir.mkdir(parents=True, exist_ok=True)
    model_file = model_dir / f"{model_decl.registered_name}.md"
    lines = [
        "---",
        "doc_type: model",
        f"component: {model_decl.component}",
        f"model: {model_decl.registered_name}",
        f"registered_name: {model_decl.registered_name}",
        f"base_model: {model_decl.base_model}",
        f"implementation_class: {model_decl.implementation_class or ''}",
        f"implementation_header: {model_decl.implementation_header or ''}",
        f"registration_source: {model_decl.source_path}",
        f"registration_description: {model_decl.description}",
        "generated_from_registration: true",
        "status: generated-scaffold",
        "---",
        "",
        f"# `{model_decl.component}::{model_decl.registered_name}`",
        "",
        "## Summary",
        "",
        model_decl.description,
        "",
        "## Exposed class",
        "",
        f"- `{model_decl.implementation_class or model_decl.component_class}`",
        "",
        "## Declared entries",
        "",
    ]
    if model_decl.entries:
        lines.extend(
            [
                "| Entry | Registration | Description |",
                "| --- | --- | --- |",
            ]
        )
        for entry in model_decl.entries:
            lines.append(f"| `{entry.name}` | `{entry.registration}` | {entry.description or '(none)'} |")
    else:
        lines.append("This model currently declares no model-specific frame entries.")
    lines.extend(
        [
            "",
            "## TODO",
            "",
            "- Add semantics, caveats, and examples that are not represented in registration metadata.",
            "- Add direct-construction notes when constructor APIs grow beyond the `BlockModel` path.",
            "",
        ]
    )
    model_file.write_text("\n".join(lines))


def main() -> None:
    args = parse_args()
    repo_root = pathlib.Path(args.repo_root).resolve()
    output_dir = pathlib.Path(args.output_dir).resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    for source_arg in args.source:
        source_path = (repo_root / source_arg).resolve()
        component_decl, model_decls = parse_source(repo_root, source_path)
        write_component_doc(output_dir, component_decl, model_decls)
        for model_decl in model_decls:
            write_model_doc(output_dir, model_decl)


if __name__ == "__main__":
    main()

from __future__ import annotations

import argparse
import json
import os
import shlex
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


def configure_output_streams() -> None:
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(line_buffering=True)
    if hasattr(sys.stderr, "reconfigure"):
        sys.stderr.reconfigure(line_buffering=True)


@dataclass(frozen=True)
class TestCase:
    suite: str
    test_id: str
    command_name: str
    test_file: Path
    common_dir: Path
    baseline_dir: Path | None = None
    output_dir: Path | None = None


@dataclass(frozen=True)
class FailureDetail:
    test_id: str
    stdout: str
    stderr: str


def detect_platform() -> str:
    if sys.platform.startswith("linux"):
        return "linux"
    if sys.platform == "darwin":
        return "macos"
    if sys.platform in {"win32", "cygwin", "msys"}:
        return "mingw"
    raise SystemExit(f"Unsupported platform: {sys.platform}")


def load_known_failures(bundle_root: Path, platform_name: str) -> set[str]:
    known_failures_path = bundle_root / "metadata" / "known_failures.json"
    with known_failures_path.open(encoding="utf-8") as handle:
        known_failures = json.load(handle)
    return set(known_failures.get(platform_name, []))


def load_excluded_tests(bundle_root: Path) -> set[str]:
    known_failures_path = bundle_root / "metadata" / "known_failures.json"
    with known_failures_path.open(encoding="utf-8") as handle:
        known_failures = json.load(handle)
    return set(known_failures.get("excluded", []))


def bundle_pythonpath(bundle_root: Path) -> Path | None:
    python_dir = bundle_root / "sample" / "python"
    if python_dir.is_dir():
        return python_dir
    return None


def discover_unit_tests(bundle_root: Path) -> list[TestCase]:
    tests_root = bundle_root / "dai-unit-tests" / "tests"
    common_dir = tests_root / "common"
    cases: list[TestCase] = []
    for test_file in sorted(tests_root.glob("test-*.dai")):
        test_name = test_file.stem.removeprefix("test-")
        cases.append(
            TestCase(
                suite="dai-unit",
                test_id=f"dai_unit_test.{test_name}",
                command_name="check_daisy",
                test_file=test_file,
                common_dir=common_dir,
            )
        )
    return cases


def discover_system_tests(bundle_root: Path, output_root: Path, excluded_tests: set[str]) -> list[TestCase]:
    tests_root = bundle_root / "dai-system-tests" / "tests"
    baseline_root = bundle_root / "dai-system-tests" / "baseline"
    common_dir = tests_root / "common"
    cases: list[TestCase] = []
    for test_file in sorted(tests_root.rglob("test-*.dai")):
        if common_dir in test_file.parents:
            continue
        rel_dir = test_file.parent.relative_to(tests_root)
        test_name = test_file.stem.removeprefix("test-")
        test_id = f"dai_system_test.{rel_dir.as_posix()}.{test_name}"
        if test_id in excluded_tests:
            continue
        baseline_dir = baseline_root / rel_dir / test_name
        cases.append(
            TestCase(
                suite="dai-system",
                test_id=test_id,
                command_name="test_daisy",
                test_file=test_file,
                common_dir=common_dir,
                baseline_dir=baseline_dir,
                output_dir=output_root / rel_dir / test_name,
            )
        )
    return cases


def build_cases(bundle_root: Path, output_root: Path, suite: str) -> list[TestCase]:
    cases: list[TestCase] = []
    excluded_tests = load_excluded_tests(bundle_root)
    if suite in {"all", "dai-unit"}:
        cases.extend(discover_unit_tests(bundle_root))
    if suite in {"all", "dai-system"}:
        cases.extend(discover_system_tests(bundle_root, output_root, excluded_tests))
    return cases


def verify_harness(commands: set[str]) -> None:
    missing = [command for command in sorted(commands) if shutil.which(command) is None]
    if missing:
        raise SystemExit(
            "Missing test harness command(s): "
            + ", ".join(missing)
            + ". Install daisypy-test first, for example with "
            + "'uv pip install git+https://github.com/daisy-model/daisypy-test'."
        )


def build_daisy_command(daisy_bin: Path, env: dict[str, str]) -> list[str]:
    try:
        launcher = daisy_bin.read_text(encoding="utf-8")
    except (OSError, UnicodeDecodeError):
        return [str(daisy_bin)]

    for line in launcher.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        command_line = stripped.replace('"$@"', "").replace("$@", "").strip()
        if not command_line.startswith("flatpak run"):
            break
        command = shlex.split(command_line)
        if env.get("PYTHONPATH"):
            command.insert(2, f"--env=PYTHONPATH={env['PYTHONPATH']}")
        return command
    return [str(daisy_bin)]


def resolve_daisy_launcher(daisy_bin: Path, env: dict[str, str]) -> str:
    cached_launcher = env.get("DAISY_PACKAGED_LAUNCHER")
    if cached_launcher:
        return cached_launcher

    command = build_daisy_command(daisy_bin, env)
    if len(command) == 1:
        env["DAISY_PACKAGED_LAUNCHER"] = command[0]
        return command[0]

    wrapper_path = Path(env["TMPDIR"]) / "daisy-launcher.sh"
    quoted_command = " ".join(shlex.quote(part) for part in command)
    wrapper_path.write_text(
        "#!/bin/sh\n"
        f"exec {quoted_command} \"$@\"\n",
        encoding="utf-8",
    )
    wrapper_path.chmod(0o755)
    env["DAISY_PACKAGED_LAUNCHER"] = str(wrapper_path)
    return env["DAISY_PACKAGED_LAUNCHER"]


def run_case(case: TestCase, daisy_bin: Path, env: dict[str, str]) -> tuple[int, FailureDetail | None]:
    command = [case.command_name, resolve_daisy_launcher(daisy_bin, env), str(case.test_file)]
    if case.baseline_dir is not None and case.output_dir is not None:
        case.output_dir.mkdir(parents=True, exist_ok=True)
        command.extend([str(case.baseline_dir), str(case.output_dir)])
    command.extend(["--path", str(case.common_dir)])

    completed = subprocess.run(
        command,
        env=env,
        check=False,
        capture_output=True,
        text=True,
    )
    if completed.returncode == 0:
        print(f"[ PASS ] {case.test_id}", flush=True)
        return completed.returncode, None

    print(f"[ FAIL ] {case.test_id}", flush=True)
    return completed.returncode, FailureDetail(
        test_id=case.test_id,
        stdout=completed.stdout,
        stderr=completed.stderr,
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run Daisy package verification tests from a downloaded test bundle."
    )
    parser.add_argument("--bundle-root", type=Path, required=True)
    parser.add_argument("--daisy-bin", type=Path, required=True)
    parser.add_argument("--daisy-home", type=Path)
    parser.add_argument("--pythonpath", type=Path)
    parser.add_argument(
        "--suite",
        choices=("all", "dai-unit", "dai-system"),
        default="all",
    )
    parser.add_argument(
        "--platform",
        choices=("linux", "macos", "mingw"),
        default=detect_platform(),
    )
    parser.add_argument(
        "--filter",
        action="append",
        default=[],
        help="Only run tests whose id contains this substring. May be repeated.",
    )
    parser.add_argument(
        "--output-root",
        type=Path,
        default=Path.cwd() / "daisy-test-results",
    )
    parser.add_argument(
        "--include-known-failures",
        action="store_true",
        help="Run tests marked as known failures for the selected platform.",
    )
    parser.add_argument(
        "--list",
        action="store_true",
        help="List discovered tests and exit without running them.",
    )
    parser.add_argument(
        "--fail-fast",
        action="store_true",
        help="Stop after the first failing test.",
    )
    return parser.parse_args()


def main() -> int:
    configure_output_streams()
    args = parse_args()
    bundle_root = args.bundle_root.resolve()
    output_root = args.output_root.resolve()
    output_root.mkdir(parents=True, exist_ok=True)

    known_failures = load_known_failures(bundle_root, args.platform)
    cases = build_cases(bundle_root, output_root, args.suite)
    if args.filter:
        cases = [
            case for case in cases
            if all(fragment in case.test_id for fragment in args.filter)
        ]

    if args.list:
        for case in cases:
            marker = " known-failure" if case.test_id in known_failures else ""
            print(f"{case.test_id}{marker}", flush=True)
        return 0

    daisy_bin = args.daisy_bin.resolve()
    verify_harness({case.command_name for case in cases})

    env = os.environ.copy()
    if args.daisy_home:
        env["DAISYHOME"] = str(args.daisy_home.resolve())
    python_paths: list[str] = []
    bundled_python_dir = bundle_pythonpath(bundle_root)
    if bundled_python_dir is not None:
        python_paths.append(str(bundled_python_dir))
    if args.pythonpath:
        python_paths.append(str(args.pythonpath.resolve()))
    if python_paths:
        existing_pythonpath = env.get("PYTHONPATH")
        if existing_pythonpath:
            python_paths.append(existing_pythonpath)
        env["PYTHONPATH"] = os.pathsep.join(python_paths)
    harness_tmpdir = output_root / ".tmp"
    harness_tmpdir.mkdir(parents=True, exist_ok=True)
    env["TMPDIR"] = str(harness_tmpdir)
    env["TMP"] = str(harness_tmpdir)
    env["TEMP"] = str(harness_tmpdir)

    failed = 0
    passed = 0
    skipped = 0
    failure_details: list[FailureDetail] = []
    for case in cases:
        if case.test_id in known_failures and not args.include_known_failures:
            skipped += 1
            print(f"[ SKIP ] {case.test_id} (known failure on {args.platform})", flush=True)
            continue
        if case.baseline_dir is not None and not case.baseline_dir.is_dir():
            print(f"[ FAIL ] {case.test_id}", flush=True)
            failed += 1
            failure_details.append(
                FailureDetail(
                    test_id=case.test_id,
                    stdout=f"Missing baseline: {case.baseline_dir}\n",
                    stderr="",
                )
            )
            if args.fail_fast:
                break
            continue
        result, failure_detail = run_case(case, daisy_bin, env)
        if result == 0:
            passed += 1
        else:
            failed += 1
            if failure_detail is not None:
                failure_details.append(failure_detail)
            if args.fail_fast:
                break

    if failure_details:
        print("\nFailure details:", flush=True)
        for detail in failure_details:
            print(f"\n--- {detail.test_id} ---", flush=True)
            if detail.stdout:
                print(detail.stdout, end="" if detail.stdout.endswith("\n") else "\n", flush=True)
            if detail.stderr:
                print(detail.stderr, end="" if detail.stderr.endswith("\n") else "\n", flush=True)

    print(
        f"Completed {len(cases)} discovered tests: "
        f"{passed} passed, {failed} failed, {skipped} skipped.",
        flush=True,
    )
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())

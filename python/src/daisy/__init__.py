import os as _os
import pathlib as _pathlib

# On Windows, DLLs bundled next to the .pyd are not found automatically.
# Register the package directory so Windows finds libcore.dll and the
# other MinGW runtime DLLs that the build step copies here.
_pkg_dir = str(_pathlib.Path(__file__).parent)
if hasattr(_os, "add_dll_directory"):
    _os.add_dll_directory(_pkg_dir)

try:
    from .daisy_bmi import DaisyBMI
except ImportError as e:
    raise ImportError(
        "Failed to import daisy_bmi. Make sure the .pyd and required DLLs are present.\n"
        f"  .pyd directory: {_pkg_dir}"
    ) from e

__all__ = ["DaisyBMI"]

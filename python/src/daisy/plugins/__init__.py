"""
Plugin system for Daisy.

This module exposes the base interface for defining Python plugins
that can be called from Daisy (C++ side).
"""

from .daisy_py_fun_interface import DaisyPyFun

__all__ = ["DaisyPyFun"]

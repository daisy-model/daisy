// function_Python.C  -- Call Python to find funtion value.
// 
// Copyright 2024 KU
//
// This file is part of Daisy.
// 
// Daisy is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// Daisy is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
// 
// You should have received a copy of the GNU Lesser Public License
// along with Daisy; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#define BUILD_DLL

#include "object_model/function.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "util/assertion.h"

#include <pybind11/embed.h>
#include <pybind11/stl.h>

// The 'Python' model.

struct FunctionPython : public Function
{
  const symbol pmodule;
  const symbol pname;
  const symbol domain;
  const symbol range;

  mutable pybind11::object py_module;
  mutable pybind11::object py_function;
  mutable enum class state_t { uninitialized, working, error } state;
  
  // Simulation.
  double value (const double arg) const
  {
    switch (state)
      {
      case state_t::error:
	return NAN;
      case state_t::uninitialized:
	// Find module.
	try
	  {
	    py_module = pybind11::module::import (pmodule.name ().c_str ());
	  }
	catch (...)
	  {
	    Assertion::message ("Could not find Python module '"
				+ pmodule + ".");
	    break;
	  }

	// Find function.
	try
	  {
	    py_function = py_module.attr(pname.name ().c_str ());
	  }
	catch (...)
	  {
	    Assertion::message ("Can't find Python function '"
				+ pname + "' in '" + pmodule + "'.");
	    break;
	  }
	state = state_t::working;
	/* fall through */
      case state_t::working:
	try
	  {
	    pybind11::object py_object = py_function (arg);
	    return py_object.cast<double> ();
	  }
	catch (...)
	  {
	    Assertion::message ("Call to Python function '"
				+ pname + "' in '" + pmodule + "' failed.");
	  }
      }
    state = state_t::error;
    return NAN;
  }

  // Create.
  FunctionPython (const BlockModel& al)
    : Function (al),
      pmodule (al.name ("module")),
      pname (al.name ("name")),
      domain (al.name ("domain")),
      range (al.name ("range")),
      state (state_t::uninitialized)
  { }
};

static struct FunctionPythonSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new FunctionPython (al); }
  FunctionPythonSyntax ()
    : DeclareModel (Function::component, "Python", 
		    "Call Python function.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_string ("module", Attribute::Const, "\
Where to find the function.");
    frame.declare_string ("name", Attribute::Const, "\
Name of the function.");
    frame.declare_string ("domain", Attribute::Const, "\
Function domain.");
    frame.declare_string ("range", Attribute::Const, "\
Function range.");
  }
} FunctionPython_syntax;

// function_Python.C ends here.

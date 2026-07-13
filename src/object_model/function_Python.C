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
#include "object_model/object_model_registration_internal.h"
#include "util/assertion.h"

#include <pybind11/embed.h>
#include <pybind11/stl.h>

struct FunctionPython::Implementation
{
  pybind11::object py_module;
  pybind11::object py_function;
  enum class state_t { uninitialized, working, error } state;

  Implementation ()
    : state (state_t::uninitialized)
  { }
};

double
FunctionPython::value (const double arg) const
{
  switch (impl_->state)
    {
    case Implementation::state_t::error:
      return NAN;
    case Implementation::state_t::uninitialized:
      // Find module.
      try
        {
          impl_->py_module = pybind11::module::import (pmodule_.name ().c_str ());
        }
      catch (std::exception &e)
        {
          Assertion::message ("Could not find Python module '"
                              + pmodule_ + ".");
          Assertion::message (e.what());
          break;
        }

      // Find function.
      try
        {
          impl_->py_function = impl_->py_module.attr(pname_.name ().c_str ());
        }
      catch (std::exception &e)
        {
          Assertion::message ("Can't find Python function '"
                              + pname_ + "' in '" + pmodule_ + "'.");
          Assertion::message (e.what());
          break;
        }
      impl_->state = Implementation::state_t::working;
      /* fall through */
    case Implementation::state_t::working:
      try
        {
          pybind11::object py_object = impl_->py_function (arg);
          return py_object.cast<double> ();
        }
      catch (std::exception &e)
        {
          Assertion::message ("Call to Python function '"
                              + pname_ + "' in '" + pmodule_ + "' failed.");
          Assertion::message (e.what());
        }
    }
  impl_->state = Implementation::state_t::error;
  return NAN;
}

FunctionPython::FunctionPython (symbol module, symbol name, symbol domain, symbol range)
  : pmodule_ (module),
    pname_ (name),
    domain_ (domain),
    range_ (range),
    impl_ (std::make_unique<Implementation> ())
{ }

FunctionPython::FunctionPython (const BlockModel& al)
  : Function (al),
    pmodule_ (al.name ("module")),
    pname_ (al.name ("name")),
    domain_ (al.name ("domain")),
    range_ (al.name ("range")),
    impl_ (std::make_unique<Implementation> ())
{ }

FunctionPython::~FunctionPython ()
{ }

struct FunctionPythonSyntax : public DeclareModel
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
};

void
register_function_python_models ()
{
  static FunctionPythonSyntax function_python_syntax;
}

// function_Python.C ends here.

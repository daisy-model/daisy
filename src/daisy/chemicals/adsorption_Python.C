// adsorption_Python.C -- Equilibrirum sorption in Python.
// 
// Copyright 2024 KU.
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
#ifdef BUILD_PYTHON
#define BUILD_DLL
#include "daisy/chemicals/adsorption.h"
#include "object_model/block_model.h"
#include "object_model/check.h"
#include "daisy/soil/soil.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"
#include "util/assertion.h"
#include "object_model/vcheck.h"
#include "daisy/chemicals/awi.h"
#include "daisy/chemicals/chemical.h"

#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include <sstream>

static const double c_fraction_in_humus = 0.587;

struct AdsorptionPython : public Adsorption
{
  // Parameters.
  const symbol pmodule;
  const symbol pC_to_M;
  const symbol pM_to_C;

  const std::set<symbol> extra;

  // State
  mutable pybind11::object py_module;
  mutable pybind11::object py_C_to_M;
  mutable pybind11::object py_M_to_C;
  mutable enum class state_t { uninitialized, working, error } state;

  // Initialiation.
  void initialize () const
  {
    switch (state)
      {
      case state_t::error:
      case state_t::working:
	return;
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
	    state = state_t::error;
	    return;
	  }

	// Find C_to_M
	try
	  {
	    py_C_to_M = py_module.attr(pC_to_M.name ().c_str ());
	  }
	catch (...)
	  {
	    Assertion::message ("Can't find Python function '"
				+ pC_to_M + "' in '" + pmodule + "'.");
	    state = state_t::error;
	    return;
	  }

	// Find M_to_C
	if (pM_to_C != Attribute::None ())
	  try
	    {
	      py_M_to_C = py_module.attr(pM_to_C.name ().c_str ());
	    }
	  catch (...)
	    {
	      Assertion::message ("Can't find Python function '"
				  + pM_to_C + "' in '" + pmodule + "'.");
	      state = state_t::error;
	      return;
	    }
      }
    state = state_t::working;
  }
  
  // Simulation.
public:
  double C_to_M (const Soil& soil, const Chemical& chemical, const AWI& awi,
		 double Theta, double T, int i, 
                 double C, double sf) const
  {
    initialize ();
    if (state != state_t::working)
      return NAN;

    try
      {
	pybind11::dict kwargs;
	kwargs["C"] = C;
	
	// "extra".
	if (extra.find ("Theta_sat") != extra.end ())
	  kwargs["Theta_sat"] = soil.Theta_sat (i); // [cm^3 W/cm^3 Sp]
	if (extra.find ("Theta") != extra.end ())
	  kwargs["Theta"] = Theta; // [cm^3 W/cm^3 Sp]
	if (extra.find ("rho_b") != extra.end ())
	  kwargs["rho_b"] = soil.dry_bulk_density (i); // [g/cm^3 Sp]
	if (extra.find ("f_OC") != extra.end ())
	  kwargs["f_OC"] = soil.humus (i) * c_fraction_in_humus; // [g/g]
	if (extra.find ("f_clay") != extra.end ())
	  kwargs["f_clay"] = soil.clay (i); // [g/g]
	if (extra.find ("d50") != extra.end ())
	  kwargs["d50"] = soil.texture_fractile (i, 0.5); // [um]
	if (extra.find ("area_AWI") != extra.end ())
	  kwargs["area_AWI"] = awi.area (i); // [um]
	if (extra.find ("molar_mass") != extra.end ())
	  kwargs["molar_mass"] = chemical.molar_mass (); // [um]
	if (extra.find ("T") != extra.end ())
	  kwargs["T"] = T;
	pybind11::object py_object = py_C_to_M (**kwargs);
	return py_object.cast<double> ();
      }
    catch (...)
      {
	Assertion::message ("Call to Python function '"
			    + pC_to_M + "' in '" + pmodule + "' failed");
	const double Theta_sat = soil.Theta_sat (i);	// [cm^3 W/cm^3 Sp]
	const double rho_b = soil.dry_bulk_density (i); // [g/cm^3 Sp]
	const double f_OC = soil.humus (i) * c_fraction_in_humus; // [g/g]
	const double f_clay = soil.clay (i); // [g/g]
	const double d50 = soil.texture_fractile (i, 0.5); // [um]
	const double area_AWI = awi.area (i); // [um]
	std::ostringstream tmp;
	tmp << "C = " << C
	    << ", Theta_sat = " << Theta_sat
	    << ", Theta = " << Theta
	    << ", rho_b = " << rho_b
	    << ", f_OC = " << f_OC
	    << ", f_clay = " << f_clay
	    << ", d50 = " << d50
	    << ", area_AWI = " << area_AWI
	    << ", T = " << T;
	Assertion::message (tmp.str ());
	state = state_t::error;
	return NAN;
      }
  }
  double M_to_C (const Soil& soil, const Chemical& chemical, const AWI& awi,
		 double Theta, double T, int i, 
                 double M, double sf) const
  {
    initialize ();
    if (state != state_t::working)
      return NAN;

    if (pM_to_C == Attribute::None ())
      return M_to_C_bisect (soil, chemical, awi, Theta, T, i, M, sf, 0.0, 1.0);

    try
      {
	pybind11::dict kwargs;
	kwargs["M"] = M;
	
	// "extra".
	if (extra.find ("Theta_sat") != extra.end ())
	  kwargs["Theta_sat"] = soil.Theta_sat (i); // [cm^3 W/cm^3 Sp]
	if (extra.find ("Theta") != extra.end ())
	  kwargs["Theta"] = Theta; // [cm^3 W/cm^3 Sp]
	if (extra.find ("rho_b") != extra.end ())
	  kwargs["rho_b"] = soil.dry_bulk_density (i); // [g/cm^3 Sp]
	if (extra.find ("f_OC") != extra.end ())
	  kwargs["f_OC"] = soil.humus (i) * c_fraction_in_humus; // [g/g]
	if (extra.find ("f_clay") != extra.end ())
	  kwargs["f_clay"] = soil.clay (i); // [g/g]
	if (extra.find ("d50") != extra.end ())
	  kwargs["d50"] = soil.texture_fractile (i, 0.5); // [um]
	if (extra.find ("area_AWI") != extra.end ())
	  kwargs["area_AWI"] = awi.area (i); // [um]
	if (extra.find ("molar_mass") != extra.end ())
	  kwargs["molar_mass"] = chemical.molar_mass (); // [um]
	if (extra.find ("T") != extra.end ())
	  kwargs["T"] = T;
	pybind11::object py_object = py_M_to_C (**kwargs);
	return py_object.cast<double> ();
      }
    catch (...)
      {
	Assertion::message ("Call to Python function '"
			    + pM_to_C + "' in '" + pmodule + "' failed");
	const double Theta_sat = soil.Theta_sat (i);	// [cm^3 W/cm^3 Sp]
	const double rho_b = soil.dry_bulk_density (i); // [g/cm^3 Sp]
	const double f_OC = soil.humus (i) * c_fraction_in_humus; // [g/g]
	const double f_clay = soil.clay (i); // [g/g]
	const double d50 = soil.texture_fractile (i, 0.5); // [um]
	const double area_AWI = awi.area (i); // [um]
	std::ostringstream tmp;
	tmp << "M = " << M
	    << ", Theta_sat = " << Theta_sat
	    << ", Theta = " << Theta
	    << ", rho_b = " << rho_b
	    << ", f_OC = " << f_OC
	    << ", f_clay = " << f_clay
	    << ", d50 = " << d50
	    << ", area_AWI = " << area_AWI
	    << ", T = " << T;
	Assertion::message (tmp.str ());
	state = state_t::error;
	return NAN;
      }
  }

  // Create.
  static std::set<symbol> v2s (const std::vector<symbol>& v)
  { return std::set (v.begin (), v.end ()); }
public:
  AdsorptionPython (const BlockModel& al)
    : Adsorption (al),
      pmodule (al.name ("module")),
      pC_to_M (al.name ("C_to_M")),
      pM_to_C (al.name ("M_to_C", Attribute::None ())),
      extra (v2s (al.name_sequence ("extra"))),
      state (state_t::uninitialized)
  { }
};

static struct AdsorptionPythonSyntax : DeclareModel
{
  Model* make (const BlockModel& al) const
  {
    return new AdsorptionPython (al);
  }
  AdsorptionPythonSyntax ()
    : DeclareModel (Adsorption::component, "Python", "\
Adsorption defined in user specified Python module.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_string ("module", Attribute::Const, "\
Python module to find the functions.");
    frame.declare_string ("C_to_M", Attribute::Const, "\
Name of function to convert concentration to mass.\n\
C_to_M (C, *extra*)\n\
-> [g CHEMICAL/cm^3 SPACE] total content in soil/water/air.\n\
\n\
where *extra* is the extra parameters specified by 'extra'.");
    frame.declare_string ("M_to_C", Attribute::OptionalConst, "\
Name of the function to convert mass to concentration.\n\
M_to_C (M *extra*)\n\
-> [g CHEMICAL/cm^3 WATER] concentration in soil water.\n\
\n\
where *extra* is the extra parameters specified by 'extra'.\n\
By default this will use C_to_M and bisection between 0 and 1.");
    frame.declare_string ("extra",
			  Attribute::Const, Attribute::Variable, "\
Extra input parameters to Python soil function.\n\
\n\
Options include:\n\
\n\
  Theta_sat [cm^3 W/cm^3 V]: Saturated water per system volume.\n\
  Theta [cm^3 W/cm^3 V]: Actual water per system volume.\n \
  rho_b [g S/cm^3]: Soil dry bulk density.\n\
  f_OC [g OC/g SOIL]: organic carbon content in soil\n\
  f_clay [g clay/g SOIL]: soil clay fraction\n\
  d50 [um]: median particle diameter\n\
  area_AWI [cm^2/cm^3]: air-water interface area\n\
  molar_mass [g/mol]: molar mass of compound\n\
  T [dg C]: soil temperature.");
    static struct ExtraCheck : public VCheck::Enum
    {
      ExtraCheck ()
	: VCheck::Enum ()
      {
	add ("Theta_sat");
	add ("Theta");
	add ("rho_b");
	add ("f_OC");
	add ("f_clay");
	add ("d50");
	add ("area_AWI");
	add ("molar_mass");
	add ("T");
      }
    } extra_check;
    frame.set_check ("extra", extra_check);
  }
} AdsorptionPython_syntax;

// adsorption_Python.C ends here.
#endif

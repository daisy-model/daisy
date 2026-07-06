// deposition.C --- Deposition of inorganic material from atmosphere.
// 
// Copyright 2013 KU.
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

#include "daisy/upper_boundary/weather/deposition.h"
#include "daisy/output/log.h"
#include "daisy/upper_boundary/weather/weather.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "object_model/units.h"
#include "daisy/daisy_registration_internal.h"

// The 'deposition' component.

const char *const Deposition::component = "deposition";

symbol
Deposition::library_id () const
{
  static const symbol id (component);
  return id;
}

const IM& 
Deposition::deposit () const
{ return my_deposit; }

void 
Deposition::output (Log& log) const
{ output_submodule (deposit (), "deposit", log); }

Deposition::Deposition (const BlockModel& al)
  : ModelFramed (al),
    my_deposit (al, "deposit")    // For the units...
{ }

Deposition::~Deposition ()
{ }

struct DepositionInit : public DeclareComponent 
{
  static void load_flux (Frame& frame)
  { IM::add_syntax (frame, Attribute::LogOnly, IM::flux_unit ()); }
  void load_frame (Frame& frame) const
  {
    Model::load_model (frame);
    frame.declare_submodule_sequence ("deposit", Attribute::LogOnly, "\
Total atmospheric deposition.", load_flux);
  }
  DepositionInit ()
    : DeclareComponent (Deposition::component, "\
Deposition of inorganic material from atmosphere.")
  { }
};

// The 'weather' model.

struct DepositionWeather : public Deposition
{
  void tick (const Time&, const Vegetation&, const Weather& weather, Treelog&)
  { my_deposit = weather.deposit (); }

  DepositionWeather (const BlockModel& al)
    : Deposition (al)
  { }
};

struct DepositionWeatherSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new DepositionWeather (al); }
  DepositionWeatherSyntax ()
    : DeclareModel (Deposition::component, "weather", "\
Rely solely on weather model for deposition.")
  { }
  void load_frame (Frame& frame) const
  { }
};

// The 'const' model.

struct DepositionConst : public Deposition
{
  const Units& units;
  const Time before;
  const Time after;
  const IM wet;
  const IM dry;
  
  void tick (const Time& time, const Vegetation&, const Weather& weather,
	     Treelog&)
  {
    if (time > before || time <= after)
      {
	my_deposit.clear ();
	return;
      }
#if 1
    const double precip = weather.rain () + weather.snow (); // [mm/h]
    
    const Unit& u_flux = units.get_unit (IM::flux_unit ());
    const Unit& u_solute = units.get_unit (IM::solute_unit ());
    const Unit& u_precip = units.get_unit (Units::mm_per_h ());
    const IM x_dry (u_flux, dry);
    const IM solute (u_solute, wet);
    const IM x_wet (solute.multiply (Scalar (precip, u_precip), u_flux));
    my_deposit = x_dry + x_wet;
#endif
  }

  DepositionConst (const BlockModel& al)
    : Deposition (al),
      units (al.units ()),
      before (al.submodel ("before")),
      after (al.submodel ("after")),
      wet (al, "wet"),
      dry (al, "dry")
      
  { }
};

struct DepositionConstSyntax : public DeclareModel
{
  static const symbol dry_deposit_unit ()
  {
    static const symbol unit ("kg/ha/y");
    return unit;
  }
  
  static void load_ppm (Frame& frame)
  { IM::add_syntax (frame, Attribute::Const, Units::ppm ()); }

  static void load_dry (Frame& frame)
  { IM::add_syntax (frame, Attribute::Const, dry_deposit_unit ()); }

  Model* make (const BlockModel& al) const
  { return new DepositionConst (al); }
  DepositionConstSyntax ()
    : DeclareModel (Deposition::component, "const", "\
Constant deposition in time interval.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_submodule ("before", Attribute::Const, "\
Only deposit before or at this time.", Time::load_syntax);
    frame.declare_submodule ("after", Attribute::Const, "\
Only deposit after this time.", Time::load_syntax);
    frame.declare_submodule_sequence ("wet", Attribute::Const, "\
Solutes in precipitaion.", load_ppm);
    frame.set_empty ("wet");
    frame.declare_submodule_sequence ("dry", Attribute::Const, "\
Dry deposition.", load_dry);
    frame.set_empty ("dry");
}
};

void
register_deposition_models ()
{
  static DepositionInit Deposition_init;
  static DepositionWeatherSyntax DepositionWeather_syntax;
  static DepositionConstSyntax DepositionConst_syntax;
}

// deposition.C ends here.

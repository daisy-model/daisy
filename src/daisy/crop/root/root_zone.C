// root_zone.C -- Root zone growth.
// 
// Copyright 1996-2001 Per Abrahamsen and SÃ¸ren Hansen
// Copyright 2000-2001 KVL.
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

#include "daisy/crop/root/root_zone.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "object_model/check.h"

// The 'zone' base model.

const std::vector<double>&
RootZone::dynamic_root_death () const // [cm/cm^3/h]
{ static std::vector<double> empty; return empty; }

double
RootZone::dynamic_root_death_DM () const // [g DM/h]
{ return 0; }

void
RootZone::output (Log& log) const
{ RootUptake::output (log); }

RootZone::RootZone (const BlockModel& al)
  : RootUptake (al),
    PenPar1 (al.number ("PenPar1")),
    PenPar2 (al.number ("PenPar2")),
    PenpFFac (al.plf ("PenpFFac")),
    PenClayFac (al.plf ("PenClayFac")),
    PenWaterFac (al.plf ("PenWaterFac")),
    PenDSFac (al.plf ("PenDSFac")),
    MaxPen (al.number ("MaxPen")),
    MaxWidth (al.number ("MaxWidth", MaxPen))
{ }
RootZone::~RootZone ()
{ }

static struct RootZoneSyntax : DeclareBase
{
  static bool
  check_alist (const Metalib&, const Frame& al, Treelog& msg)
  {
    bool ok = true;
    return ok;
  }

  void load_frame (Frame& frame) const
  {
    frame.declare ("DptEmr", "cm", Check::non_negative (), Attribute::Const,
		   "Penetration at emergence.");
    frame.set ("DptEmr", 10.0);
    frame.declare ("PenPar1", "cm/dg C/d", Check::non_negative (), Attribute::Const,
		   "Penetration rate parameter, coefficient.");
    frame.set ("PenPar1", 0.25);
    frame.declare ("PenPar2", "dg C", Check::none (), Attribute::Const,
		   "Penetration rate parameter, threshold.");
    frame.set ("PenPar2", 4.0);
    frame.declare ("PenpFFac", "pF", Attribute::None (),
		   Check::non_negative (), Attribute::Const, 
		   "Moisture dependent factor to multiply 'PenPar1' with.\n\
If pressure is less than -1 cm, pF will be assumed to be 0.");
    frame.set ("PenpFFac", PLF::always_1 ());
    frame.declare ("PenClayFac", Attribute::Fraction (), Attribute::None (),
		   Check::non_negative (), Attribute::Const, 
		   "Clay dependent factor to multiply 'PenPar1' with.");
    frame.set ("PenClayFac", PLF::always_1 ());
    frame.declare ("PenWaterFac", Attribute::Fraction (), Attribute::None (),
		   Check::non_negative (), Attribute::Const, 
		   "Water dependent factor to multiply 'PenPar1' with.\n\
The factor is a function of relative water content (Theta/Theta_sat).");
    frame.set ("PenWaterFac", PLF::always_1 ());
    frame.declare ("PenDSFac", "DS", Attribute::None (),
		   Check::non_negative (), Attribute::Const, "\
Development stage dependent factor to multiply 'PenPar1' with.");
    frame.set ("PenDSFac", PLF::always_1 ());
  }

  RootZoneSyntax () 
    : DeclareBase (RootSystem::component, "zone", "uptake", "\
Root zone grow mostly as function of thermal age.")
  { }
} RootZone_syntax;


// root_zone.C ends here.

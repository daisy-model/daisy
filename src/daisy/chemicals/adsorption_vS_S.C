// adsorption_vS_S.C
// 
// Copyright 1996-2001 Per Abrahamsen and Søren Hansen
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

#include "daisy/chemicals/adsorption.h"
#include "daisy/soil/soil.h"
#include "util/mathlib.h"
#include "object_model/librarian.h"
#include "object_model/block_model.h"
#include "object_model/check.h"
#include <sstream>

class Adsorption_vS_S : public Adsorption
{
  const double S_planar;	// [g N/g clay]
  const double S_edge;		// [g N/g clay]
  const double K_planar;	// [g N/cm^3]
  const double K_edge;		// [g N/cm^3]

  // Simulation.
public:
  double C_to_A (const Soil& soil, int i, double C, double sf) const;
  double C_to_M (const Soil&, const Chemical&,
		 const AWI&, double Theta, double T,
		 int, double C, double sf) const;
  double M_to_C (const Soil&, const Chemical&,
		 const AWI&, double Theta, double T,
		 int, double M, double sf) const;

  // Chemical soil constants.
  double v_planar (const Soil& soil, int i, double sf) const
    { 
      const double clay = soil.clay (i); // []
      const double rho_b = soil.dry_bulk_density (i); // [g N/cm^3]
      return S_planar * clay * rho_b * sf; 
    }
  double v_edge (const Soil& soil, int i, double sf) const
    {
      const double clay = soil.clay (i); // []
      const double rho_b = soil.dry_bulk_density (i); // [g N/cm^3]
      return S_edge * clay * rho_b * sf; 
    }

  // Create.
public:
  Adsorption_vS_S (const BlockModel& al)
    : Adsorption (al),
      S_planar (al.number ("S_planar")),
      S_edge (al.number ("S_edge")),
      K_planar (al.number ("K_planar")),
      K_edge (al.number ("K_edge"))
  { }
};

double
Adsorption_vS_S::C_to_A (const Soil& soil, int i, double C, double sf) const
{
  return (v_planar (soil, i, sf) * C) / (K_planar + C)
    + (v_edge (soil, i, sf) * C) / (K_edge + C);
}

double 
Adsorption_vS_S::C_to_M (const Soil& soil, const Chemical&, const AWI&, double Theta, double,
			 int i, double C,
                         double sf) const
{
  return C_to_A (soil, i, C, sf) + Theta * C;
}

double 
Adsorption_vS_S::M_to_C (const Soil& soil, const Chemical& chemical,
			 const AWI& awi, double Theta, double T,
			 int i, double M, double sf) const
{
  const double ve = v_edge (soil, i, sf); 
  const double Ke = K_edge;
  const double vp = v_planar (soil, i, sf);
  const double Kp = K_planar;

  double C;

  if (M < 1e-6 * std::min (Ke, Kp))
    // There are numerical problems in the general solution for small M. 
    C = M / (Theta + ve  / Ke + vp / Kp);
  else
    {
      daisy_assert (Theta > 0.0);
      const double a = Theta;
      const double b = Theta * (Kp + Ke) + vp + ve - M;
      const double c = vp * Ke + ve * Kp - M * (Kp + Ke) + Kp * Ke * Theta;
      const double d = - M * Kp * Ke;
    
      C = single_positive_root_of_cubic_equation (a, b, c, d);
      const double new_M = C_to_M (soil, chemical, awi, Theta, T, i, C, sf);
      if (!approximate (M, new_M, 0.01))
	{
	  std::ostringstream tmp;
	  tmp << objid << ": M[i]: old = " << M << ", new = " << new_M
	      << "; Theta = " << Theta << "; C = " << C;
	  Assertion::warning (tmp.str ());
	}
	    
	  
    }
  return C;
}

static struct Adsorption_vS_SSyntax : DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new Adsorption_vS_S (al); }
  Adsorption_vS_SSyntax ()
    : DeclareModel (Adsorption::component, "vS_S", "\
Double langmuir description of NH4-N sorption to clay.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.set_strings ("cite", "van1963potassium");
    frame.declare ("S_planar", "g N/g clay", Check::non_negative (), Attribute::Const, "\
Absorption capacity of the clay planar sites.");
    frame.declare ("S_edge", "g N/g clay", Check::non_negative (), Attribute::Const, "\
Absorption capacity of the clay edge sites.");
    frame.declare ("K_planar", "g N/cm^3", Check::non_negative (), Attribute::Const, "\
Half-saturation constant of the clay planer sites.");
    frame.declare ("K_edge", "g N/cm^3", Check::non_negative (), Attribute::Const, "\
Half-saturation constant of the clay edge sites.");
  }
} Adsorption_vS_S_syntax;

// The 'vS_S_Hansen' parameterization.

static struct AdsorptionvS_S_HansenSyntax : public DeclareParam
{ 
  AdsorptionvS_S_HansenSyntax ()
    : DeclareParam (Adsorption::component, "vS_S_Hansen", "vS_S", "\
.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.set ("S_planar", 5.964e-3);
    frame.set ("S_edge",  0.308e-3);
    frame.set ("K_planar", 6.3e-5);
    frame.set ("K_edge", 1.372e-5);
  }
} AdsorptionvS_S_Hansen_syntax;

// The 'vS_S_Styczen' parameterization.

static struct AdsorptionvS_S_StyczenSyntax : public DeclareParam
{ 
  AdsorptionvS_S_StyczenSyntax ()
    : DeclareParam (Adsorption::component, "vS_S_Styczen", "vS_S", "\
Parameter update..")
  { }
  void load_frame (Frame& frame) const
  {
    frame.set ("S_planar", 5.964e-3);
    frame.set ("S_edge", 0.2801e-3);
    frame.set ("K_planar", 6.338e-4);
    frame.set ("K_edge", 1.369e-5);
  }
} AdsorptionvS_S_Styczen_syntax;

// The 'vS_S_old' model.

class Adsorption_vS_S_old : public Adsorption
{
  // Simulation.
public:
  double C_to_A (const Soil& soil, int i, double C, double sf) const;
  double C_to_M (const Soil&, const Chemical& chemical, const AWI&,
		 double Theta, double, int, double C, double sf) const;
  double M_to_C (const Soil&, const Chemical& chemical, const AWI&,
		 double Theta, double, int, double M, double sf) const;

  // Chemical soil constants.
  double K_planar () const
    { return 6.3e-5; }		// [g/cm�]
  double K_edge () const
    { return 1.372e-5; }	// [g/cm�]
  double v_planar (const Soil& soil, int i, double sf) const
    { 
      // Maximum specific absorbtion [g / g clay]
      const double S_planar = 5.964e-3;
      const double porosity = soil.Theta (i, 0.0, 0.0);
      const double clay = soil.clay (i);
      const double rho_b = soil.dry_bulk_density (i);
      return S_planar * clay * rho_b * sf * (1.0 - porosity); 
    }
  double v_edge (const Soil& soil, int i, double sf) const
    {
      const double S_edge = 0.308e-3;	// Same for edges. [g / g clay]
      const double porosity = soil.Theta (i, 0.0, 0.0);
      const double clay = soil.clay (i);
      const double rho_b = soil.dry_bulk_density (i);
      return S_edge * clay * rho_b * sf * (1.0 - porosity); 
    }

  // Create.
public:
  Adsorption_vS_S_old (const BlockModel& al)
    : Adsorption (al)
    { }
};

double
Adsorption_vS_S_old::C_to_A (const Soil& soil, int i, double C, double sf) const
{
  return (v_planar (soil, i, sf) * C) / (K_planar () + C)
    + (v_edge (soil, i, sf) * C) / (K_edge () + C);
}

double 
Adsorption_vS_S_old::C_to_M (const Soil& soil, const Chemical& chemical,
			     const AWI&,
			     double Theta, double,
			     int i, double C, double sf) const
{
  return C_to_A (soil, i, C, sf) + Theta * C;
}

double 
Adsorption_vS_S_old::M_to_C (const Soil& soil, const Chemical& chemical,
			     const AWI& awi,
			     double Theta, double T,
			     int i, double M, double sf) const
{
  const double ve = v_edge (soil, i, sf); 
  const double Ke = K_edge ();
  const double vp = v_planar (soil, i, sf);
  const double Kp = K_planar ();

  double C;

  if (M < 1e-6 * std::min (Ke, Kp))
    // There are numerical problems in the general solution for small M. 
    C = M / (Theta + ve  / Ke + vp / Kp);
  else
    {
      daisy_assert (Theta > 0.0);
      const double a = Theta;
      const double b = Theta * (Kp + Ke) + vp + ve - M;
      const double c = vp * Ke + ve * Kp - M * (Kp + Ke) + Kp * Ke * Theta;
      const double d = - M * Kp * Ke;
    
      C = single_positive_root_of_cubic_equation (a, b, c, d);
      daisy_assert (approximate (M, C_to_M (soil, chemical,
					    awi, Theta, T, i, C, sf)));
    }
  return C;
}

static struct Adsorption_vS_S_oldSyntax : DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new Adsorption_vS_S_old (al); }
  Adsorption_vS_S_oldSyntax ()
    : DeclareModel (Adsorption::component, "vS_S_old", "\
Buggy implementation included as vS_S before Daisy 6.47.\n\
Included here as reference.\n\
Note, linear sorption was used as default for NH4 as long as memory serve.")
  { }
  void load_frame (Frame& frame) const
  { }
} Adsorption_vS_S_old_syntax;

// adsorption_vS_S.C ends here.

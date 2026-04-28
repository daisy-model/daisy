// max_exfiltration.C  -- Max exfiltration allowed by soil.
// 
// Copyright 2026 KU
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

#include "daisy/upper_boundary/surface/max_exfiltration.h"
#include "daisy/soil/soil_water.h"
#include "daisy/soil/soil.h"
#include "daisy/soil/transport/geometry.h"
#include "object_model/check.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "util/mathlib.h"

// The 'max_exfiltration' component.

const char *const MaxExfiltration::component = "max_exfiltration";

MaxExfiltration::MaxExfiltration (const BlockModel&)
{ }

MaxExfiltration::~MaxExfiltration ()
{ }

static struct MaxExfiltrationInit : public DeclareComponent 
{
  void load_frame (Frame& frame) const
  { Model::load_model (frame); }
  MaxExfiltrationInit ()
    : DeclareComponent (MaxExfiltration::component, "\
Soil limited maximum exfiltration.")
  { }
} MaxExfiltration_init;

// The 'Hansen' model.

struct MaxExfiltrationHansen : public MaxExfiltration
{
  // Simulation.
  double value (const Geometry& geo, const size_t edge,
		const Soil& soil, const SoilWater& soil_water,
		const double T, const double /* h_atm */) const
  {
    const size_t n = geo.edge_other (edge, Geometry::cell_above);
    const double h0 = soil_water.h (n);
    const double K0 = soil.K (n, h0, soil_water.h_ice (n), T);
    const double Cw2 = soil.Cw2 (n, h0);
    const double Theta0 = soil_water.Theta (n);
    const double Theta_surf = soil.Theta_res (n);
    const double delta_Theta = Theta0 - Theta_surf;
    const double z0 = geo.cell_z (n);
    // Darcy formulated for Theta between middle of node and soil surface.
    return - (K0 / Cw2) * (delta_Theta / z0);
  }

  // Create.
  MaxExfiltrationHansen (const BlockModel& al)
    : MaxExfiltration (al)
  { }
};

static struct MaxExfiltrationHansenSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new MaxExfiltrationHansen (al); }
  MaxExfiltrationHansenSyntax ()
    : DeclareModel (MaxExfiltration::component, "Hansen", "\
Darcy formulated for water content with mimimum water at surface.")
  { }
  void load_frame (Frame&) const
  { }
} MaxExfiltrationHansen_syntax;

// The 'gradient' model.

struct MaxExfiltrationGradient : public MaxExfiltration
{
  const double max_exfiltration_gradient; // []
  
  // Simulation.
  double value (const Geometry& geo, const size_t edge,
		const Soil& soil, const SoilWater& soil_water,
		const double T, const double /* h_atm */) const
  {
    const size_t n = geo.edge_other (edge, Geometry::cell_above);
    const double h0 = soil_water.h (n);
    const double K0 = soil.K (n, h0, soil_water.h_ice (n), T);
    return K0 * max_exfiltration_gradient;
  }
  // Create.
  MaxExfiltrationGradient (const BlockModel& al)
    : MaxExfiltration (al),
      max_exfiltration_gradient (al.number ("max_exfiltration_gradient"))
  { }
};

static struct MaxExfiltrationGradientSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new MaxExfiltrationGradient (al); }
  MaxExfiltrationGradientSyntax ()
    : DeclareModel (MaxExfiltration::component, "gradient", "\
Darcy formulated for water content with mimimum water at surface.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare ("max_exfiltration_gradient", Attribute::None (),
		   Check::positive (), Attribute::Const,
		   "Maximal pressure gradient for calculating exfiltration.\n\
The gradient is assumed from center of top node to surface of top node.");
    frame.order ("max_exfiltration_gradient");
  }
} MaxExfiltrationGradient_syntax;

// The 'Theta' model.

struct MaxExfiltrationTheta : public MaxExfiltration
{
  const double h_crit;		// [cm]
  
  // Simulation.
  double value (const Geometry& geo, const size_t edge,
		const Soil& soil, const SoilWater& soil_water,
		const double T, const double h_atm) const
  {
    const size_t n = geo.edge_other (edge, Geometry::cell_above);
    const double h0 = soil_water.h (n);
    const double h_ice = soil_water.h_ice (n);
    const double K0 = soil.K (n, h0, h_ice, T);
    const double Cw2 = soil.Cw2 (n, h0);
    const double Theta0 = soil_water.Theta (n);
    const double h_surf = (h_crit < 0)
      ? h_crit
      : h_atm;
    const double Theta_surf = soil.Theta (n, h_surf, h_ice);
    const double delta_Theta = Theta0 - Theta_surf;
    const double z0 = geo.cell_z (n);
    // Darcy formulated for Theta between middle of node and soil surface.
    return - (K0 / Cw2) * (delta_Theta / z0);
  }

  // Create.
  MaxExfiltrationTheta (const BlockModel& al)
    : MaxExfiltration (al),
      h_crit (al.number ("h_crit", 42.42e42))
  { }
};

static struct MaxExfiltrationThetaSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new MaxExfiltrationTheta (al); }
  MaxExfiltrationThetaSyntax ()
    : DeclareModel (MaxExfiltration::component, "Theta", "\
Darcy formulated for water content with mimimum water at surface.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare ("h_crit", "cm",
		   Check::none (), Attribute::OptionalConst,
		   "Surface potential for soil limited evaporation.\n\
If positive, use atmospheric potential (Kelvin's equation).");
  }
} MaxExfiltrationTheta_syntax;

// The 'steady-state' model.

struct MaxExfiltrationSteadyState : public MaxExfiltration
{
  const double h_crit;		// [cm]
  
  // Simulation.
  double value (const Geometry& geo, const size_t edge,
		const Soil& soil, const SoilWater& soil_water,
		const double T, const double h_atm) const
  {
    const double L = geo.edge_length (edge);
    const size_t n = geo.edge_other (edge, Geometry::cell_above);
    const double h0 = soil_water.h (n);
    const double M0 = soil.M (n, h0);
    const double h_surf = (h_crit < 0)
      ? h_crit
      : h_atm;
    const double M_surf = soil.M (n, h_surf);
    // Steady state flow solution ignoring gravity.
    const double q = - (M_surf - M0) / L;
    return q;
  }

  // Create.
  MaxExfiltrationSteadyState (const BlockModel& al)
    : MaxExfiltration (al),
      h_crit (al.number ("h_crit", 42.42e42))
  { }
};

static struct MaxExfiltrationSteadyStateSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new MaxExfiltrationSteadyState (al); }
  MaxExfiltrationSteadyStateSyntax ()
    : DeclareModel (MaxExfiltration::component, "steady-state", "\
Steady state flow from top cell to surface.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare ("h_crit", "cm",
		   Check::none (), Attribute::OptionalConst,
		   "Surface potential for soil limited evaporation.\n\
If positive, use atmospheric potential (Kelvin's equation).");
  }
} MaxExfiltrationSteadyState_syntax;

// max_exfiltration.C ends here.

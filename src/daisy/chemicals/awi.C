// AWI.C --- Air-Water Interface area.
// 
// Copyright 2025 KU.
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

#include "daisy/chemicals/awi.h"
#include "object_model/librarian.h"
#include "object_model/block_model.h"
#include "daisy/output/log.h"
#include "daisy/soil/transport/geometry.h"
#include "daisy/soil/soil.h"
#include "daisy/soil/soil_water.h"
#include "util/mathlib.h"

// The 'AWI' component.

const char *const AWI::component = "AWI";

symbol 
AWI::library_id () const
{
  static const symbol id (component);
  return id;
}

void
AWI::output (Log& log) const
{ output_value (AWI_area, "area", log); }
    
void
AWI::tick (const Geometry& geo, const Soil& soil, const SoilWater& soil_water)
{
  const size_t cell_size = geo.cell_size ();
  AWI_area.resize (cell_size);
  for (size_t c = 0; c < cell_size; c++)
    {
      const double Theta = soil_water.Theta (c);
      const double Theta_sat = soil.Theta_sat (c);
      const double d50_um = soil.texture_fractile (c, 0.5);
      const double d50_cm = d50_um * 1e-4 /* [cm/um] */;

      AWI_area[c] = find_area (Theta, Theta_sat, d50_cm);
    }
}

AWI::AWI (const BlockModel& al)
  : ModelDerived (al.type_name ())
{ }

AWI::~AWI ()
{ }

static struct AWIInit : public DeclareComponent 
{
  void load_frame (Frame& frame) const
  {
    Model::load_model (frame);
    frame.declare ("area", "cm^2/cm^3", 
                   Attribute::LogOnly, Attribute::SoilCells, 
                   "Air-Water interface area.");
  }
  AWIInit ()
    : DeclareComponent (AWI::component, "\
Estimate area of air-water interface.")
  { }
} AWI_init;

// The 'Brusseau2023' AWI model.

struct AWIBrusseau2023 : public AWI
{
  double find_area (double Theta, double Theta_sat,
		    double d50 /* [cm] */) const // [cm^2/cm^3]
  {
    const double Sw = Theta/Theta_sat;

    // Equation 5.
    return (-2.85 * Sw + 3.6) * ((1.0 - Sw) * 3.9 * std::pow (d50,-1.2));
  }
  
  explicit AWIBrusseau2023 (const BlockModel& al)
    : AWI (al)
  { }      
};

static struct AWIBrusseau2023Syntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new AWIBrusseau2023 (al); }
  AWIBrusseau2023Syntax ()
    : DeclareModel (AWI::component, "Brusseau2023", "\
Estimate AWI area from water and median particle size (Equation 5).")
  { }
  void load_frame (Frame& frame) const
  {
    frame.set_strings ("cite", "brusseau2023");
  }
} AWIBrusseau2023_syntax;

// The 'Costanza2008x' AWI model.

struct AWICostanza2008x : public AWI
{
  double find_area (double Theta, double Theta_sat,
		    double d50 /* [cm] */) const // [cm^2/cm^3]
  {
    const double Sw = Theta/Theta_sat;

    // Equation 2.
    const double SA = 6.0 * (1.0 - Theta_sat) / d50;

    // Equation 1.
    const double area = SA * (-0.9112 * Sw + 0.9031); // [cm^2/cm^3]

    return std::max (0.0, area);
  }
  
  explicit AWICostanza2008x (const BlockModel& al)
    : AWI (al)
  { }      
};

static struct AWICostanza2008xSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new AWICostanza2008x (al); }
  AWICostanza2008xSyntax ()
    : DeclareModel (AWI::component, "Costanza2008x", "\
Estimate AWI area from water and median particle size (Equation 1 and 2).")
  { }
  void load_frame (Frame& frame) const
  {
    frame.set_strings ("cite", "costanza2008x");
  }
} AWICostanza2008x_syntax;

// AWI.C ends here.

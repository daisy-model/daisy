// denprod_Parton1996.C --- Split denitrification per Parton et al, 1996.
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

#define BUILD_DLL

#include "daisy/chemicals/denprod.h"
#include "object_model/function.h"
#include "object_model/librarian.h"
#include "util/mathlib.h"
#include "object_model/frame.h"
#include "object_model/block_model.h"
#include "daisy/soil/transport/geometry.h"
#include "daisy/soil/soil.h"
#include "daisy/soil/soil_water.h"
#include "daisy/organic_matter/organic.h"
#include "daisy/chemicals/chemical.h"
#include "daisy/output/log.h"

class Geometry;

// The 'P96WFPS' function.

struct FunctionP96WFPS : public FunctionPlotable
{
  double value (const double wfps) const
  { return 1.4 / std::pow (13.0, 17.0 / std::pow (13.0, 2.2 * wfps)); }

  // Plot.
  double x_min () const
  { return 0.0; }
  double x_max () const
  { return 1.0; }
  
  // Create.
  FunctionP96WFPS (const BlockModel& al)
    : FunctionPlotable (al)
  { }
};

static struct FunctionP96WFPSSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new FunctionP96WFPS (al); }
  FunctionP96WFPSSyntax ()
    : DeclareModel (Function::component, "P96WFPS", 
		    "Water filed pore space effect on N2/N2O ratio.\n\
Figure 5, top. Note extra parentheses compared to source.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.set ("domain", Attribute::Fraction ());
    frame.set ("range", Attribute::None ());
    frame.set_strings ("cite", "Parton1996");
    frame.set ("formula", "\\frac{1.4}{13^{\\left(13^{\\left(\\frac{17}{2.2\\; \\mathrm{wfps}}\\right)}\\right)}}");
  }
} FunctionP96WFPS_syntax;

// The 'P96NO3' function.

struct FunctionP96NO3 : public FunctionPlotable
{
  double value (const double NO3) const
  { return (1.0 - (0.5 + std::atan (M_PI * 0.01 * (NO3 - 190)) / M_PI))
      * 25.0; }
  
  // Plot.
  double x_min () const
  { return 0.0; }
  double x_max () const
  { return 360.0; }

  // Create.
  FunctionP96NO3 (const BlockModel& al)
    : FunctionPlotable (al)
  { }
};

static struct FunctionP96NO3Syntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new FunctionP96NO3 (al); }
  FunctionP96NO3Syntax ()
    : DeclareModel (Function::component, "P96NO3", 
		    "NO3 effect on N2/N2O ratio.\n\
Figure 5, middle.  Note extra parentheses compared to source.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.set_strings ("cite", "Parton1996");
    frame.set ("domain", "ug N/g");
    frame.set ("range", Attribute::None ());
    frame.set ("formula", "\\left(1 - \\left(0.5 + \\frac{1\\arctan{(\\pi\\;0.01\\; (\\textrm{NO3}-190))}}{\\pi}\\right)\\right) 25");
  }
} FunctionP96NO3_syntax;

// The 'P96CO2' function.

struct FunctionP96CO2 : public FunctionPlotable
{
  double value (const double CO2) const
  { return 13.0 + 30.78 * std::atan (M_PI * 0.07 * (CO2 - 13.0)) / M_PI; }
  
  // Plot.
  double x_min () const
  { return 0.0; }
  double x_max () const
  { return 35.0; }

  // Create.
  FunctionP96CO2 (const BlockModel& al)
    : FunctionPlotable (al)
  { }
};

static struct FunctionP96CO2Syntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new FunctionP96CO2 (al); }
  FunctionP96CO2Syntax ()
    : DeclareModel (Function::component, "P96CO2", 
		    "CO2 effect on N2/N2O ratio.\n\
Figure 5, bottom.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.set_strings ("cite", "Parton1996");
    frame.set ("domain", "kg C/ha/d");
    frame.set ("range", Attribute::None ());
    frame.set ("formula", "13+\\frac{30.78 \\arctan{(\\pi\\;0.07\\;(\\textrm{CO2}-13))}}{\\pi}");
  }
} FunctionP96CO2_syntax;

// The 'Parton1996' denprod model.

struct DenprodParton1996 : public Denprod
{
  const std::unique_ptr<Function> FR_NO3; // [ug N/g S] -> []
  const std::unique_ptr<Function> FR_CO2; // [kg C/ha/d] -> []
  const std::unique_ptr<Function> FR_WFPS; // [0-1] -> []
  const double FR_CO2_depth;		   // [cm]
  std::vector<double> N2O;		   // [g N/cm^3 S]
  
  // Simulation.
  void split (const std::vector<double>& N, const Geometry& geo,
	      const Soil& soil, const SoilWater& soil_water,
	      Chemical& soil_NO3, const OrganicMatter& organic,
	      Treelog& msg)
  {
    const double micro = 1e6;		 // [u]
    const double cm2_per_ha = 1e4 * 1e4; // [cm^2/ha]
    const double h_per_d = 24.0;	 // [h/d]
    const double kilo = 1e-3;		 // [k]
    
    const size_t cell_size = geo.cell_size ();
    daisy_assert (N.size () == cell_size);
    for (size_t c = 0; c < cell_size; c++)
      {
	const double Theta = soil_water.Theta (c);	// [cm^3 W/cm^3 S]
	const double wfps = Theta / soil.Theta_sat (c); // [0-1]

	const double no3_in = soil_NO3.M_primary (c); // [g N/cm^3 S]
	const double dry_bulk_density // [g S/cm^3 S]
	  = soil.dry_bulk_density (c);
	const double NO3 = micro * no3_in / dry_bulk_density; // [ug N/g S]

	const double co2_in = organic.CO2 (c); // [g C/cm^3 S/h]
	const double CO2		       // [kg C/ha/d]
	  = co2_in * kilo * FR_CO2_depth * cm2_per_ha * h_per_d;

	const double R_N2_per_N2O //   []
	  = std::min (FR_NO3->value (NO3), FR_CO2->value (CO2))
	  * FR_WFPS->value (wfps);
	daisy_assert (R_N2_per_N2O > 0.0);
	N2O[c] = N[c] / (1.0 + R_N2_per_N2O); // [g N/cm^3 S/h]
      }
  }
  void output (Log& log) const
  { output_variable (N2O, log); }

  // create.
  void initialize (const Geometry& geo)
  { N2O = std::vector<double> (geo.cell_size (), 0.0); }
  explicit DenprodParton1996 (const BlockModel& al)
    : Denprod (al),
      FR_NO3 (Librarian::build_item<Function> (al, "FR_NO3")),
      FR_CO2 (Librarian::build_item<Function> (al, "FR_CO2")),
      FR_WFPS (Librarian::build_item<Function> (al, "FR_WFPS")),
      FR_CO2_depth (al.number ("FR_CO2_depth"))
  { }      
};

static struct DenprodParton1996Syntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new DenprodParton1996 (al); }
  DenprodParton1996Syntax ()
    : DeclareModel (Denprod::component, "Parton1996", "\
Find N2O from denitrification based on WFPS, NO3, and CO2.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_function ("FR_NO3", "ug N/g", Attribute::None (), "\
NO3 effect N2O.");
    frame.set ("FR_NO3", "P96NO3");
    frame.declare_function ("FR_CO2", "kg C/ha/d", Attribute::None (), "\
CO2 effect N2O.");
    frame.set ("FR_CO2", "P96CO2");
    frame.declare_function ("FR_WFPS",
			    Attribute::Fraction (), Attribute::None (), "\
WFPS effect N2O.");
    frame.set ("FR_WFPS", "P96WFPS");
    frame.declare ("FR_CO2_depth", "cm", Attribute::Const, "\
Depth used for converting CO2 from volume to area units.");
    frame.set ("FR_CO2_depth", 30.0);
    frame.declare ("N2O", "g N/cm^3/h",
                   Attribute::LogOnly, Attribute::Variable,
                   "Amount of N2O-N generated by denitrification.");
  }
} DenprodParton1996_syntax;

// denprod.C ends here.

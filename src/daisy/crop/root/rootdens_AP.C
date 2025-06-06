// rootdens_AP.C -- Anders Pedersen variant of Gerwitz and Page.
// 
// Copyright 1996-2001, 2004 Per Abrahamsen and SÃ¸ren Hansen
// Copyright 2000-2001, 2004 KVL.
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

#include "daisy/crop/root/rootdens.h"
#include "object_model/block_model.h"
#include "daisy/soil/transport/geometry.h"
#include "daisy/output/log.h"
#include "object_model/plf.h"
#include "object_model/check.h"
#include "util/mathlib.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"
#include <sstream>

struct Rootdens_AP : public Rootdens
{
  // Parameters.
  const PLF a_DS;               // Form "parameter" DS -> [cm^-1]
  const double q;               // Extra root depth [cm]

  // Log variables.
  double a;                     // Form "parameter" [cm^-1]
  double L0;                    // Root density at soil surface [cm/cm^3]

  // simulation.
  void set_density (const Geometry& geo, 
		    double SoilDepth, double CropDepth,
		    const double /* CropWidth [cm] */,
		    double WRoot, double DS,
		    std::vector<double>& Density, 
		    Treelog& );
  void output (Log& log) const;

  // Create.
  void initialize (const Geometry&, double /* row_width */, double, Treelog&)
  { }
  Rootdens_AP (const BlockModel&);
};

void
Rootdens_AP::set_density (const Geometry& geo, 
                          const double SoilDepth, const double CropDepth,
			  const double /* CropWidth [cm] */,
                          const double WRoot, const double DS,
			  std::vector<double>& Density,
                          Treelog&)
{
  const double Depth = std::min (SoilDepth, CropDepth);
  a = a_DS (DS);
  static const double m_per_cm = 0.01;
  const double LengthPrArea = m_per_cm * SpRtLength * WRoot; // [cm/cm^2]
  const double d_m = Depth + q;
  L0 = LengthPrArea 
    / ((1.0 / a) * (1.0 - exp (-a * Depth))
       + ((exp (-a * Depth) / (Depth - d_m))
          * (-0.5 * sqr (d_m) - 0.5 * sqr (Depth) + d_m * Depth)));
  daisy_assert (L0 >= 0.0);

  PLF tip;                      // Linear decrease downto Depth + q;
  tip.add (Depth, L0 * exp (- a * Depth));
  tip.add (d_m, 0.0);

  const size_t size = geo.cell_size ();
  for (size_t i = 0; i < size; i++)
    {
      const double d = -geo.cell_z (i);
      const double f_top = geo.fraction_in_z_interval (i, 0.0, -Depth);
      const double f_tip = geo.fraction_in_z_interval (i, -Depth, -d_m);
      Density[i] = L0 * exp (- a * d) * f_top + tip (d) * f_tip;
    }
}

void 
Rootdens_AP::output (Log& log) const
{
  output_variable (a, log); 
  output_variable (L0, log); 
}

Rootdens_AP::Rootdens_AP (const BlockModel& al)
  : Rootdens (al),
    a_DS (al.plf ("a_DS")),
    q (al.number ("q")),
    a (-42.42e42),
    L0 (-42.42e42)
  
{ }

static struct Rootdens_APSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new Rootdens_AP (al); }
  Rootdens_APSyntax ()
    : DeclareModel (Rootdens::component, "Anders Pedersen", 
	       "Use exponential function for root density.\n\
In this variant of Gerwitz and Page, 'a' is specified as a function of\n\
development stage.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.set_strings ("cite", "gp74");

    frame.declare ("a_DS", "DS", "cm^-1", Attribute::Const, 
                "Form parameter as a function of development stage.");
    frame.declare ("q", "cm", Check::non_negative (), Attribute::Const, 
                "Extra root length below max rooting depth.\n\
Root density will decrease linearly from the GP calculated amount\n\
at max rooting depth to zero 'q' further down.");
    frame.declare ("a", "cm^-1", Attribute::LogOnly, "Form parameter.\n\
Calculated from 'a_DS'.");
    frame.declare ("L0", "cm/cm^3", Attribute::LogOnly,
                "Root density at soil surface.");
  }
} Rootdens_AP_syntax;

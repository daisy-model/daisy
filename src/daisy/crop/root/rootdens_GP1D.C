// rootdens_GP1D.C -- Gerwitz and Page model for calculating root density.
// 
// Copyright 1996-2001 Per Abrahamsen and SÃ¸ren Hansen
// Copyright 2000-2001 KVL.
// Copyright 2007 Per Abrahamsen and KVL.
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
#include "object_model/check.h"
#include "util/mathlib.h"
#include "object_model/librarian.h"
#include "util/iterative.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"

#include <sstream>

struct Rootdens_GP1D : public Rootdens
{
  // Parameters.
  const double DensRtTip;	// Root density at (pot) pen. depth. [cm/cm^3]
  const double DensIgnore;	// Ignore cells below this density. [cm/cm^3]

  // Log variables.
  double a;                     // Form parameter. [cm^-1]
  double L0;                    // Root density at soil surface. [cm/cm^3]
  double k;			// Scale factor due to soil limit. []

  // LogProduct
  struct InvW
  {
    static double derived (const double W)
    { return std::exp (W) + W * std::exp (W); }
    const double k;
    double operator()(const double W) const
    { return W * std::exp (W) - k; }
    InvW (const double k_)
      : k (k_)
    { }
  };

  // simulation.
  void set_density (const Geometry& geo, 
		    double SoilDepth, double CropDepth, double CropWidth,
		    double WRoot, double DS, std::vector<double>& Density,
		    Treelog&);
  void limit_depth (const Geometry& geo, 
		    const double l_r /* [cm/cm^2] */,
		    const double d_s /* [cm] */, 
		    const double d_a /* [cm] */, 
		    std::vector<double>& Density  /* [cm/cm^3] */,
		    Treelog& msg);
  void uniform (const Geometry& geo, const double l_r, const double d_a,
		std::vector<double>& Density);
  void output (Log& log) const;

  // Create.
  void initialize (const Geometry&, double row_width, double row_pos, 
                   Treelog& msg);
  explicit Rootdens_GP1D (const BlockModel&);
};

void
Rootdens_GP1D::set_density (const Geometry& geo, 
			    const double SoilDepth /* [cm] */, 
			    const double CropDepth /* [cm] */,
			    const double /* CropWidth [cm] */,
			    const double WRoot /* [g DM/m^2] */, const double,
			    std::vector<double>& Density  /* [cm/cm^3] */,
			    Treelog& msg)
{
  const size_t cell_size = geo.cell_size ();

  // Check input.
  daisy_assert (Density.size () == cell_size);
  daisy_assert (CropDepth > 0);
  daisy_assert (WRoot > 0);

  static const double m_per_cm = 0.01;

  // Root dry matter.
  const double M_r = WRoot /* [g/m^2] */ * m_per_cm * m_per_cm; // [g/cm^2]

  // Specific root length.
  const double S_r = SpRtLength /* [m/g] */ / m_per_cm; // [cm/g]

  // Root length (\ref{eq:root_length} Eq 3).
  const double l_r = S_r * M_r;	// [cm/cm^2]
  
  // Potential depth.
  const double d_c = CropDepth;	// [cm]

  // Soil depth.
  const double d_s = SoilDepth;	// [cm]

  // Actual depth.
  const double d_a = std::min (d_c, d_s); // [cm]

  // Minimum density.
  const double L_m = DensRtTip;	// [cm/cm^3]

  // Minimum root length in root zone.
  const double l_m = L_m * d_c;	// [cm/cm^2]

  // Minimum root length as fraction of total root length.
  const double D = l_m / l_r;	// []

  // Identity: W = - a d_c
  // Solve: W * exp (W) = -D (\ref{eq:Lambert} Eq 6):
  // IW (W) = W * exp (W) - D
  // Since we know D, we can construct the function f.
  const InvW f (-D);		// [] -> []

  // ... And the derived df/dW

  // The function f has a local minimum at -1.
  const double W_min = -1;	   // []

  // There is no solution when -D is below the value at W_min.
  const double D_max = -W_min * std::exp (W_min); // []

  // Too little root mass to fill the root zone.
  if (D > D_max)
    {
      // We warn once.
      static bool warn_about_to_little_root = true;
      if (warn_about_to_little_root)
	{
	  // warn_about_to_little_root = false;
	  std::ostringstream tmp;
	  tmp << "Min ratio is " << D << ", max is " << D_max << ".\n"
	      << "Not enough root mass to fill root zone.\n"
	      << "Using uniform distribution.";
	  msg.warning (tmp.str ());
	}
      uniform (geo, l_r, d_a, Density);
      return;
    }

  // There are two solutions to f (W) = 0, we are interested in the
  // one for W < W_min.  We start with a guess of -2.
  const double W = Newton (-2.0, f, f.derived);
  
  // Check the solution.
  const double f_W = f (W);
  if (!approximate (D, D + f_W))
    {
      std::ostringstream tmp;
      tmp << "Newton's methods did not converge.\n";
      tmp << "W = " << W << ", f (W) = " << f_W << ", D = " << D << "\n";
      (void) Newton (-2.0, f, f.derived, &tmp);
      tmp << "Using uniform distribution.";
      msg.error (tmp.str ());
      uniform (geo, l_r, d_a, Density);
      return;
    }

  // Find a from W (\ref{eq:a-solved} Eq 7):
  a = -W / d_c;			// [cm^-1]
  // and L0 from a (\ref{eq:L0-found} Eq 8):
  L0 = L_m  * std::exp (a * d_c); // [cm/cm^3]

  // Fill Density.
  for (size_t cell = 0; cell < cell_size; cell++)
    {
      // TODO: Using cell average would be better than cell center.
      const double z = -geo.cell_z (cell); // Positive below ground. [cm]
      Density[cell] = L0 * std::exp (-a * z);
    }

  // Redistribute roots from outside root zone.
  limit_depth (geo, l_r, d_s, d_a, Density, msg);
}

void
Rootdens_GP1D::limit_depth (const Geometry& geo, 
			    const double l_r /* [cm/cm^2] */,
			    const double d_s /* [cm] */, 
			    const double d_a /* [cm] */, 
			    std::vector<double>& Density  /* [cm/cm^3] */,
			    Treelog& msg)
{
  const size_t cell_size = geo.cell_size ();
			    
  // Lowest density worth calculating on.
  const double L_epsilon = DensIgnore; // [cm/cm^3]

  // We find the total root length from cells above minimum.
  double l_i = 0;		// Integrated root length [cm]
  for (size_t cell = 0; cell < cell_size; cell++)
    {
      // Density in cell.
      double L_c = Density[cell];

      // Eliminate low density cells.
      if (L_c < L_epsilon)
	L_c = 0.0;

      // Eliminate roots below actual root depth.
      // TODO:  We really would like the soil maximum instead
      L_c *= geo.fraction_in_z_interval (cell, 0, -d_s);

      // Add and update.
      l_i += L_c * geo.cell_volume (cell);
      Density[cell] = L_c;
    }
  l_i /= geo.surface_area ();	// Per area [cm/cm^2]

  // No roots, nothing to do.
  if (iszero (l_i))
    {
      if (l_r > 0)
	{
	  msg.error ("We lost all roots.  Using uniform distribution");
	  uniform (geo, l_r, d_a, Density);
	}
      k = -1.0;
      return;
    }
  
  // Scale factor.
  k = l_r / l_i;		// \ref{eq:scale-factor} Eq. 13
  for (size_t cell = 0; cell < cell_size; cell++)
    Density[cell] *= k;		// \ref{eq:limited-depth} Eq. 12
}

void
Rootdens_GP1D::uniform (const Geometry& geo, const double l_r, const double d_a,
			std::vector<double>& Density)
{
  const size_t cell_size = geo.cell_size ();

  // Uniform distribution parameters.
  a = 0;
  L0 = std::max (l_r / d_a, DensRtTip);
  for (size_t cell = 0; cell < cell_size; cell++)
    {
      const double f = geo.fraction_in_z_interval (cell, 0, -d_a);
      Density[cell] = f * L0;
    }
  k = 1.0;
}

void 
Rootdens_GP1D::output (Log& log) const
{
  output_variable (a, log); 
  output_variable (L0, log); 
  output_variable (k, log); 
}

void 
Rootdens_GP1D::initialize (const Geometry&, double row_width, double, 
                           Treelog& msg)
{
  if (!iszero (row_width))
    msg.warning ("Row width not supported for '" + objid 
                 + "' root density model");
}

Rootdens_GP1D::Rootdens_GP1D (const BlockModel& al)
  : Rootdens (al),
    DensRtTip (al.number ("DensRtTip")),
    DensIgnore (al.number ("DensIgnore", DensRtTip)),
    a (-42.42e42),
    L0 (-42.42e42),
    k (-42.42e42)
{ }

static struct Rootdens_GP1DSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new Rootdens_GP1D (al); }
  Rootdens_GP1DSyntax ()
    : DeclareModel (Rootdens::component, "GP1D", 
	       "Use exponential function for root density.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.set_strings ("cite", "gp74");
    frame.declare ("DensRtTip", "cm/cm^3", Check::positive (), Attribute::Const,
                "Root density at (potential) penetration depth.");
    frame.set ("DensRtTip", 0.1);
    frame.declare ("DensIgnore", "cm/cm^3", Check::positive (),
                Attribute::OptionalConst,
                "Ignore cells with less than this root density.\n\
By default, this is the same as DensRtTip.");
    frame.declare ("a", "cm^-1", Attribute::LogOnly, "Form parameter.\n\
Calculated from 'DensRtTip'.");
    frame.declare ("L0", "cm/cm^3", Attribute::LogOnly,
                "Root density at soil surface.");
    frame.declare ("k", Attribute::None (), Attribute::LogOnly,
                "Scale factor due to soil limit.\n\
\n\
Some roots might be below the soil imposed maximum root depth, or in areas\n\
with a density lower than the limit specified by DensIgnore.\n\
These roots will be re distributed within the root zone by multiplying the\n\
density with this scale factor.");
  }
} Rootdens_GP1D_syntax;

// rootdens_GP1D.C ends here.

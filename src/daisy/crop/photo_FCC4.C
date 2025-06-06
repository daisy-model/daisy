// photo_FCC4.C -- C4 leaf photosynthesis and stomata model (Collatz et al., 1992)
// 
// Copyright 1996-2001,2005 Per Abrahamsen and Søren Hansen
// Copyright 2000-2001,2005-2006 KVL.
// Copyright 2006,2007 Birgitte Gjettermann.
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
#include "daisy/crop/photo_Farquhar.h"
#include "daisy/crop/root/rubiscoNdist.h"
#include "daisy/upper_boundary/bioclimate/bioclimate.h"
#include "daisy/crop/canopy_std.h"
#include "daisy/crop/phenology.h"
#include "daisy/output/log.h"
#include "object_model/plf.h"
#include "object_model/frame.h"
#include "object_model/block_model.h"
#include "util/mathlib.h"
#include "object_model/check.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "daisy/upper_boundary/bioclimate/resistance.h"
#include <sstream>

class PhotoFCC4 : public PhotoFarquhar
{
  // Parameters.
private:
  const double Q10k;  // Q10 value for rate constant
  const double Q10vm; // Q10 value for capacity
  const double Q10rd; // Q10 value for respiration
  const double kj;    // Initial slope of photosynthetic CO2 response 
  const double alpha; // Initial slope of photosynthetic light response
  const double paab;  // leaf absorbtivity to PAR
  const double theta; // Curvature parameter
  const double beta;  // Curvanture parameter

  // Simulation.
public:
  double V_m (const double Vm_25, double Tl) const;
  double J_m (const double vmax25, const double T) const;
  double Qt (const double k, const double Tl, const double Qtk) const;
  void CxModel (const double CO2_atm, const double O2_atm, const double Ptot, 
                double& pn, double& ci, const double Q, 
		const double gsw, const double gbw,
                const double T, const double vmax, 
		const double rd, Treelog& msg) const ;
  double respiration_rate(const double Vm_25, const double Tl) const;
  
  // Create and Destroy.
public:
  PhotoFCC4 (const BlockModel& al)
    : PhotoFarquhar (al),
      Q10k (al.number ("Q10k")),
      Q10vm (al.number ("Q10vm")),
      Q10rd (al.number ("Q10rd")),
      kj (al.number ("kj")),
      alpha (al.number ("alpha")),
      paab (al.number ("paab")),
      theta (al.number("theta")),
      beta (al.number("beta"))
  { }
  ~PhotoFCC4 ()
  { }
};

// Temperature response function for Vmax according to Collatz et al., 1992.
double
PhotoFCC4::V_m (const double vm_25, const double Tl) const
{
  const double vm1 = vm_25/(1.+ exp(0.3*(Tl-40.)))*(1.+ exp(0.2425 *(15.-Tl))); 
  return vm1; //mol/m2/s
}

double
PhotoFCC4::J_m (const double, const double) const
{ return 0.0;}

// Q10 function for Vmax, , :
double
PhotoFCC4::Qt (const double k, const double Tl, const double Qtk) const
{
  const double qt = (Tl-25.)/10.;
  const double kQt = k * pow(Qtk, qt); 
  return kQt; //[mol/m2/s/fraction]
}

void 
PhotoFCC4::CxModel (const double CO2_atm, const double,
                    const double Ptot, double& pn, double& ci/*[Pa]*/, 
		    const double PAR /*[mol/m²leaf/s/fraction]*/, 
		    const double gsw /*[mol/m²/s]*/, 
		    const double gbw /*[mol/m²/s]*/, 
                    const double T, 
		    const double vmax25 /*[mol/m² leaf/s/fraction]*/, 
		    const double rd /*[mol/m² leaf/s]*/, Treelog& msg) const  
{
  // Updating temperature dependent parameters:
  const double Vm1 = V_m(vmax25, T); //[mol/m²leaf/s/fraction]
  const double Vm = Qt(Vm1, T, Q10vm);  
  daisy_assert (Vm >=0.0);
  const double kjc = Qt(kj, T, Q10k); //[mol/Pa/m²leaf/s/fraction]

  daisy_assert (gsw > 0.0);
  double rsw = 1./gsw; // stomatal resistance to water [m²leaf*s/mol]
  
  //Newton Raphsons solution to 'net'photosynthesis and stomatal conductance.
  const int maxiter = 150;
  int aiter; 
  double lastci, newci;

  if (std::isnormal (ci)) 
    lastci = ci;
  else 
    lastci = CO2_atm;
  newci = 0.;
  aiter = 0;

  while(std::abs(newci - lastci) > 0.01)
    {
      if (aiter > maxiter)
	{
	  std::ostringstream tmp;
	  tmp << "Bug: total iterations in C4 model exceed " << maxiter;
	  msg.error (tmp.str ());
	  break;
	}
      // Gross CO2 uptake limited by Rubisco
      const double wc = kjc * ci / Ptot; //[mol/m² leaf/s]
      const double we = alpha * paab * PAR; //[mol/m²leaf/s/fraction]
      const double a = first_root_of_square_equation(theta, -(Vm+we), we*Vm); //[mol/m² leaf/s/fraction]
      const double p = first_root_of_square_equation(beta, -(a+wc), a*wc);    //[mol/m² leaf/s/fraction]

      // Net CO2 uptake
      pn = p - rd; // [mol/m² leaf/s] 
      
      //Total resistance to CO2
      const double gtc = 1./(1.6*rsw);   //[mol/m² leaf/s]
      newci = ((gtc * CO2_atm /Ptot)-pn)/gtc*Ptot;//[Pa]

      const double dp = (kjc*(p-a))/((2. * beta * p) -a -wc);
      const double dcc = -(Ptot/gtc)*dp; // [unitless]
      newci = ci-(newci-ci)/(dcc-1.);    // Newtons next iteration, [Pa]
      lastci = ci; 
      if (newci > 0.) 
	ci = newci;
      else 
	{
	  ci = 0.5 * CO2_atm;
	  newci = ci;
	}
      aiter = 1 + aiter;
    }  
}

double
PhotoFCC4::respiration_rate (const double Vm_25, const double Tl) const 
{
  const double rd_25 = 0.015 * Vm_25;// [mol/m²leaf/s/fraction]
  const double rdz = Qt(rd_25, Tl, Q10rd); 
  const double rd = rdz/(1. + exp(1.3*(Tl-55.)));
  return rd;
}

static struct Photo_FCC4Syntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new PhotoFCC4 (al); }
  Photo_FCC4Syntax ()
    : DeclareModel (Photo::component, "FC_C4", "Farquhar", "\
C4 photosynthesis and stomatal conductance model by Collatz et al., 1992.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare ("Q10k", Attribute::Unknown (), Check::positive (), Attribute::Const,
                "Q10k = 1.8 (Collatz et al., 1992)");
    frame.set ("Q10k", 1.8);

    frame.declare ("Q10vm", Attribute::Unknown (), Check::positive (), Attribute::Const,
                "Q10vm = 2.4 (Collatz et al., 1992)");
    frame.set ("Q10vm", 2.4);

    frame.declare ("Q10rd", Attribute::Unknown (), Check::positive (), Attribute::Const,
                "Q10rd = 2.0 (Collatz et al., 1992)");
    frame.set ("Q10rd", 2.0);

    frame.declare ("kj", Attribute::Unknown (), Check::positive (), Attribute::Const,
                "Initial slope of photosynthetic CO2 response, kj = 0.6 mol/m^2/s (Collatz et al., 1992)");
    frame.set ("kj", 0.6);

    frame.declare ("alpha", "mol/mol", Check::positive (), Attribute::Const,
                "Initial slope of photosynthetic light response. alpha = 0.04 (Collatz et al., 1992)");
    frame.set ("alpha", 0.04);

    frame.declare ("paab", Attribute::Unknown (), Check::positive (), Attribute::Const,
                "Leaf absorbtivity to PAR. paab = 0.86 (Collatz et al., 1992)");
    frame.set ("paab", 0.86);

    frame.declare ("theta", Attribute::Unknown (), Check::positive (), Attribute::Const,
                "Curvature parameter");
    frame.set ("theta", 0.83);
    
    frame.declare ("beta", Attribute::Unknown (), Check::positive (), Attribute::Const,
                "Curvanture parameter");
    frame.set ("beta", 0.93);

    // Ball & Berry parameters:
    // frame.set ("m", 3.0);
    // frame.set ("b", 0.08);
  }

} PhotoFCC4_syntax;

// photo_FCC4 ends here.

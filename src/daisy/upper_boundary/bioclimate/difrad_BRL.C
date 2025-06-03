// difrad_BRL.C -- Diffuse radiation based on the model of Ridley et al., 2010.
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU Lesser Public License for more details.
// 
// You should have received a copy of the GNU Lesser Public License
// along with Daisy; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#define BUILD_DLL

#include "daisy/upper_boundary/bioclimate/difrad.h"
#include "object_model/block_model.h"
#include "daisy/upper_boundary/weather/weather.h"
#include "daisy/upper_boundary/bioclimate/fao.h"
#include "util/mathlib.h"
#include "object_model/check.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"
#include <sstream>

struct DifradBRL : public Difrad
{
  // Parameters.
  private:
  const double beta0;
  const double beta1;
  const double beta2;
  const double beta3;
  const double beta4;

  // Simulation.
  double value (const Time& time, const Weather& weather, Treelog&)
  {
    // Solar elevation angle and atmospheric pressure
    const double sin_beta = weather.sin_solar_elevation_angle ();

    if (sin_beta < 0.01)
      return 1.0;

    const double I_e = weather.extraterrestrial_radiation ();//[W/m^2]
    const double kt = bound(0.0, weather.global_radiation () / I_e, 1.0);
    const double AST = weather.day_length ();
    const double daily_Ig = weather.daily_global_radiation ();
    const double daily_Ie = weather.daily_extraterrestrial_radiation ();
    const double daily_Kt = bound(0.0, daily_Ig/daily_Ie, 1.0);
    const double beta = beta0 + beta1 * kt + beta2 * AST + beta3 * asin(sin_beta) + beta4 * daily_Kt;
    const double fd = 1.0/(1.0 + exp(beta));
    daisy_assert (std::isfinite (fd));
    return bound (0.0, fd, 1.0); 
  }

  void output (Log& log) const
  {
    Difrad::output (log);
  }

// Create and Destroy.
  public:
  DifradBRL (const BlockModel& al)
    : Difrad (al),
       beta0 (al.number ("beta0")),
       beta1 (al.number ("beta1")),
       beta2 (al.number ("beta2")),
       beta3 (al.number ("beta3")),
       beta4 (al.number ("beta4"))
    { }
  ~DifradBRL ()
    { }
};

static struct DifradBRLSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new DifradBRL (al); }
  DifradBRLSyntax ()
    : DeclareModel (Difrad::component, "BRL", "\
Diffuse radiation calculation based on the model of Ridley et al., 2010. Last predictor (persistence) is not taken into account.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare ("beta0", Attribute::None (), Attribute::Const, "1st parameter (constant) of BRL model.");
    frame.set ("beta0", -2.4);
    frame.declare ("beta1", Attribute::None (), Attribute::Const, "2nd parameter (hourly clearness index) of BRL model.");
    frame.set ("beta1", 2.77);
    frame.declare ("beta2", Attribute::None (), Attribute::Const, "3rd parameter (apparent solar time) of BRL model.");
    frame.set ("beta2", 0.033);
    frame.declare ("beta3", Attribute::None (), Attribute::Const, "4th parameter (solar altitude) of BRL model.");
    frame.set ("beta3", 0.054);
    frame.declare ("beta4", Attribute::None (), Attribute::Const, "5th parameter (daily clearness index) of BRL model.");
    frame.set ("beta4", 1.028);
  }
} DifradBRL_syntax;

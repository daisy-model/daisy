// om.C -- A single unspecified organic matter pool.
// 
// Copyright 1996-2001 Per Abrahamsen and Søren Hansen
// Copyright 2000-2002 KVL.
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

#include "daisy/organic_matter/om.h"
#include "daisy/organic_matter/som.h"
#include "daisy/organic_matter/smb.h"
#include "daisy/organic_matter/dom.h"
#include "object_model/frame.h"
#include "object_model/check.h"
#include "object_model/vcheck.h"
#include "daisy/soil/transport/geometry.h"
#include "daisy/output/log.h"
#include "util/mathlib.h"
#include "util/assertion.h"
#include "object_model/treelog.h"
#include "object_model/block_model.h"
#include "object_model/rate.h"
#include <sstream>
#include <numeric>

void
OM::output (Log& log) const
{
  output_variable (initial_C_per_N, log); // For checkpoint
  output_variable (C, log);
  output_variable (N, log);
  static const symbol C_per_N_symbol ("C_per_N");
  if (log.check_leaf (C_per_N_symbol))
    {
      std::vector<double> C_per_N;
      size_t size = N.size ();
      daisy_assert (C.size () >= size);
      for (size_t i = 0; i < size; i++)
	{
	  if (N[i] < 1e-51)
	    C_per_N.push_back (Unspecified);
	  else
	    C_per_N.push_back (C[i] / N[i]);
	}
      output_variable (C_per_N, log);
    }
}

void 
OM::mix (const Geometry& geo, const double from, const double to,
         std::vector<double>& N_change, std::vector<double>& C_change)
{
  // Ignore tiny pools.
  if (soil_C (geo) < 1e-20)
    return;

  // Mix.
  daisy_non_negative (C);
  geo.mix (C, from, to, C_change);
  daisy_non_negative (C);
  daisy_non_negative (N);
  geo.mix (N, from, to, N_change);
  daisy_non_negative (N);
}

void 
OM::mix (const Geometry& geo, const Volume& volume,
         std::vector<double>& N_change, std::vector<double>& C_change)
{
  // Ignore tiny pools.
  if (soil_C (geo) < 1e-20)
    return;

  // Mix.
  daisy_non_negative (C);
  geo.mix (C, volume, C_change);
  daisy_non_negative (C);
  daisy_non_negative (N);
  geo.mix (N, volume, N_change);
  daisy_non_negative (N);
}

void
OM::swap (const Geometry& geo, 
          const double from, const double middle, const double to,
          std::vector<double>& N_change, std::vector<double>& C_change)
{
  // Ignore tiny pools.
  if (soil_C (geo) < 1e-20)
    return;

  // Swap.
  daisy_non_negative (C);
  geo.swap (C, from, middle, to, C_change);
  daisy_non_negative (C);
  daisy_non_negative (N);
  geo.swap (N, from, middle, to, N_change);
  daisy_non_negative (N);
}

double 
OM::soil_C (const Geometry& geo) const
{ return geo.total_soil (C); }

double 
OM::soil_N (const Geometry& geo) const
{ return geo.total_soil (N); }

double 
OM::soil_C (const Geometry& geo, double from, double to) const
{ return geo.total_soil (C, from, to); }

double 
OM::soil_N (const Geometry& geo, const double from, const double to) const
{ return geo.total_soil (N, from, to); }

double 
OM::goal_C_per_N (const size_t at) const // Desired C/N ratio.
{
  daisy_assert (C_per_N_goal.size () > at);
  return C_per_N_goal[at];
}

void
OM::turnover (const double from_C, const double from_N, 
	      const double to_C_per_N, double N_avail,
	      double rate, const double efficiency,
	      double& C_use, double& N_produce, double& N_consume)
{
  daisy_assert (rate >= 0.0);
  if (rate < 1e-200)
    {
      N_produce = N_consume = C_use = 0;
      return;
    }

  // Full turnover.
  N_produce = from_N * rate;
  N_consume = from_C * rate * efficiency / to_C_per_N;

  // Is there enough N?
  if (N_consume - N_produce < N_avail)
    {
      // Yes.  We are done.
      C_use = from_C * rate;
      return;
    }
  
  // Is there any N?
  if (N_avail < 1e-12) // Less than 1 [ug/l] to use...
    {
      // Nope.  Prevent turnover entirely.
      N_produce = N_consume = C_use = 0;
      return;
    }

  // Lower rate to force 
  //   N_consume - N_produce == N_avail
  // This is what calc tell me:
  // We need to introduce a new variable because BCC5 is braindead.
  const double N_avail2 = N_avail - 1e-12; // Leave 1 [ug/l].
  rate = N_avail2 / (efficiency * from_C / to_C_per_N - from_N);
  daisy_assert (std::isfinite (rate));
  if (rate < 0)
    rate = 0;

  // Aside: We could also have solved the equation by decreasing the 
  // efficiency.
  //   efficiency = (N_avail + rate * from_N) * to_C_per_N / rate * from_C;
  // But we don't

  // Update the N and C fluxes.
  N_produce = from_N * rate;
  N_consume = from_C * rate * efficiency / to_C_per_N;
  C_use = from_C * rate;

  // Check that we calculated the right rate.
  daisy_assert (approximate (N_consume - N_produce, N_avail2));
}

void
OM::turnover_pool (const std::vector<bool>& active, const double* factor,
		   double fraction, double efficiency,
		   const double* N_soil, double* N_used, double* CO2, OM& om,
                   const double dt)
{
  const size_t cell_size = active.size ();
  daisy_assert (C.size () == cell_size);
  daisy_assert (N.size () == cell_size);

  const double speed = turnover_rate * fraction;

  for (size_t i = 0; i < cell_size; i++)
    {
      if (!active[i] || C[i] < 1.0e-20)
	continue;

      const double rate = std::min (factor[i] * speed, 0.1);
      daisy_assert (std::isfinite (rate));
      daisy_assert (rate >=0);
      daisy_assert (N_soil[i] * 1.001 >= N_used[i]);
      daisy_assert (N[i] >= 0.0);
      daisy_assert (C[i] >= 0.0);
      daisy_assert (om.N[i] >= 0.0);
      daisy_assert (om.C[i] >= 0.0);
      double C_use;
      double N_produce;
      double N_consume;
      
      turnover (C[i], N[i], om.goal_C_per_N (i), N_soil[i] - N_used[i],
		rate, efficiency, C_use, N_produce, N_consume);

      // Update C.
      daisy_assert (om.C[i] >= 0.0);
      CO2[i] += C_use * (1.0 - efficiency);
      om.C[i] += C_use * efficiency * dt;
      C[i] -= C_use * dt;
      daisy_assert (om.C[i] >= 0.0);
      daisy_assert (C[i] >= 0.0);

      // Update N.
      N_used[i] += (N_consume - N_produce);
      daisy_assert (N_soil[i] * 1.001 >= N_used[i]);
      daisy_assert (om.N[i] >= 0.0);
      daisy_assert (N[i] >= 0.0);
      om.N[i] += N_consume * dt;
      N[i] -= N_produce * dt;
      daisy_assert (om.N[i] >= 0.0);
      daisy_assert (N[i] >= 0.0);
    }
}

void
OM::turnover_dom (const std::vector<bool>& active, const double* factor,
		  double fraction, DOM& dom, const double dt)
{
  const size_t cell_size = active.size ();
  const double speed = turnover_rate * fraction;

  for (size_t i = 0; i < cell_size; i++)
    {
      if (!active[i])
        continue;
      const double rate = std::min (speed * factor[i], 0.1);
      const double C_use = C[i] * rate;
      const double N_use = N[i] * rate;
      dom.add_to_source (i, C_use, N_use);
      C[i] -= C_use * dt;
      N[i] -= N_use * dt;
      daisy_assert (C[i] >= 0.0);
      daisy_assert (N[i] >= 0.0);
    }
}

void
OM::tick (const std::vector<bool>& active, const double* abiotic_factor, 
	  const double* N_soil, double* N_used,
	  double* CO2, const std::vector<SMB*>& smb, 
          const std::vector<SOM*>&som,
	  const std::vector<DOM*>&dom, const double dt)
{
  if (turnover_rate < 1e-200)
    return;

  const size_t cell_size = active.size ();
  daisy_assert (C.size () == cell_size);
  daisy_assert (N.size () == cell_size);

  const size_t smb_size = smb.size ();
  const size_t som_size = som.size ();
  const size_t dom_size = dom.size ();
  daisy_assert (fractions.size () == smb_size + som_size + dom_size);
  // Distribute to all biological pools.
  for (size_t j = 0; j < smb_size; j++)
    {
      const double fraction = fractions[j];
      if (fraction > 1e-50)
	turnover_pool (active, abiotic_factor, fraction, efficiency[j],
		       N_soil, N_used, CO2, *smb[j], dt);
    }
  // Distribute to all soil pools.
  for (size_t j = 0; j < som_size; j++)
    {
      const double fraction = fractions[smb_size + j];
      if (fraction > 1e-50)
	turnover_pool (active, abiotic_factor, fraction, 1.0,
		       N_soil, N_used, CO2, *som[j], dt);
    }
  // Distribute to all dissolved pools.
  for (size_t j = 0; j < dom_size; j++)
    {
      const double fraction = fractions[smb_size + som_size + j];
      if (fraction > 1e-50)
	turnover_dom (active, abiotic_factor, fraction, *dom[j], dt);
    }
  for (size_t i = 0; i < cell_size; i++)
    {
      if (!active[i])
        continue;

      daisy_assert (N_soil[i] * 1.001 >= N_used[i]);
      daisy_assert (C[i] >= 0.0);
      daisy_assert (N[i] >= 0.0);
    }
}

const double OM::Unspecified = -1042.42e42;

static bool check_alist (const Metalib&, const Frame& al, Treelog& err)
{
  bool ok = true;

  if (al.check ("C") && al.check ("C_per_N"))
    {
      const std::vector<double>& C = al.number_sequence ("C");
      const std::vector<double>& C_per_N = al.number_sequence ("C_per_N");
      
      if (C_per_N.size () > C.size () && C_per_N.size () > 1)
	{
	  err.entry ("\
You cannot specify 'C_per_N' for intervals where 'C' is unspecified.");
	  ok = false;
	}
      else if (al.check ("N") && C.size () > 0)
	{
	  bool bogus = false;
	  const std::vector<double>& N = al.number_sequence ("N");

	  const size_t check_size = std::min (N.size (), C_per_N.size ());
	  for (size_t i = 0; i < check_size; i++)
	    {
	      if (C_per_N[i] < 0.0)
		/* do nothing */;
	      else if (iszero (N[i]))
		bogus = true;
	      else if (!approximate (C[i] / N[i], C_per_N[i]))
		bogus = true;
	    }
	  if (bogus)
	    {
	      err.entry ("'C' / 'N' is inconsistent with 'C_per_N'");
	      ok = false;
	    }
	}
    }

  return ok;
}

void
OM::load_syntax (Frame& frame, const std::string& frac_desc)
{
  Model::load_model (frame);
  frame.add_check (check_alist);
  frame.declare ("C", "g C/cm^3", Check::non_negative (),
	      Attribute::OptionalState, Attribute::SoilCells,
	      "Carbon in each soil interval.");
  frame.declare ("C_per_N", "(g C/cm^3)/(g N/cm^3)", Check::none (), 
	      Attribute::OptionalState, Attribute::SoilCells, 
	      "The carbon/nitrogen ratio.");
  frame.declare ("N", "g N/cm^3", Check::non_negative (),
	      Attribute::OptionalState, Attribute::SoilCells,
	      "Nitrogen in each soil interval.");
  Rate::declare (frame, "turnover", "\n\
Fraction converted to other pools each hour.");
  frame.declare_fraction ("efficiency", Attribute::Const, Attribute::Variable, "\
The efficiency this pool can be digested by each of the SMB pools.");
  frame.declare_fraction ("fractions", Attribute::Const, Attribute::Variable, "\
How this pool is divided into other pools.\n" + frac_desc);
  frame.set_check ("fractions", VCheck::sum_equal_1 ());
  frame.declare ("initial_C_per_N", "g C/g N", Attribute::OptionalState, "\
The initial C/N ratio when this pool is created.\n\
Negative numbers mean unspecified.");
  frame.declare ("heat_factor", "dg C", Attribute::None (), Attribute::OptionalConst,
	      "Heat factor.  If empty, use default from 'OrganicMatter'.");
  frame.declare ("water_factor", "cm", Attribute::None (), Attribute::OptionalConst, "\
Water potential factor.  If empty, use default from 'OrganicMatter'.");
}

double
OM::get_initial_C_per_N (const Frame& al)
{
  if (al.check ("initial_C_per_N"))
    {
      if (al.number ("initial_C_per_N") < 0.0)
        return OM::Unspecified;
      else
        return al.number ("initial_C_per_N");
    }
  if (al.check ("C_per_N"))
    {
      const std::vector<double>& C_per_N = al.number_sequence ("C_per_N");
      if (C_per_N.size () > 0U)
	return C_per_N[0];
    }
  if (al.check ("C") && al.check ("N"))
    {
      const std::vector<double>& C = al.number_sequence ("C");
      const std::vector<double>& N = al.number_sequence ("N");
      
      if (C.size () > 0 && C[0] > 0 && N.size () > 0 && N[0] > 0)
	return C[0] / N[0];
    }
  return OM::Unspecified;
}

void 
OM::initialize (size_t size)
{
  // Make sure C is large enough.
  const int extra_C = size - C.size ();
  if (extra_C > 0)
    C.insert (C.end (), extra_C, 0.0);

  // Make sure N is large enough.
  const int extra_N = size - N.size ();
  if (extra_N > 0)
    N.insert (N.end (), extra_N, 0.0);
}

OM::OM (const BlockModel& al)
  : ModelFramed (al),
    initial_C_per_N (get_initial_C_per_N (al.frame ())),
    turnover_rate (Rate::value (al,"turnover")),
    efficiency (al.number_sequence ("efficiency")),
    fractions (al.number_sequence ("fractions"))
{ 
  if (al.check ("heat_factor"))
    heat_factor = al.plf ("heat_factor");
  if (al.check ("water_factor"))
    water_factor = al.plf ("water_factor");
  if (al.check ("C"))
    C = al.number_sequence ("C");
  if (al.check ("N"))
    N = al.number_sequence ("N");

  if (al.check ("C_per_N"))
    {
      const std::vector<double>& C_per_N = al.number_sequence ("C_per_N");
      if (C.size () > 0)
	{
	  daisy_assert (C_per_N.size () >= C.size ());
	  for (size_t i = N.size (); i < C_per_N.size (); i++)
	    {
	      if (C_per_N[i] > 0)
		N.push_back (C[i] / C_per_N[i]);
	      else
		N.push_back (0.0);
	    }
	}
    }
}

OM::~OM ()
{ }

// om.C ends here.


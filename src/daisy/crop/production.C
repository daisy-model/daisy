// production.C -- Default crop production submodel.
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

#include "daisy/crop/production.h"
#include "daisy/crop/crpn.h"
#include "daisy/crop/partition.h"
#include "daisy/organic_matter/organic.h"
#include "daisy/soil/transport/geometry.h"
#include "daisy/organic_matter/am.h"
#include "daisy/output/log.h"
#include "daisy/daisy_time.h"
#include "object_model/plf.h"
#include "object_model/treelog.h"
#include "util/mathlib.h"
#include "object_model/frame_submodel.h"
#include "object_model/librarian.h"
#include <sstream>

// Chemical constants affecting the crop.
const double molWeightCH2O = 30.0; // [gCH2O/mol]
const double molWeightCO2 = 44.0; // [gCO2/mol]
const double molWeightC = 12.0; // [gC/mol]

// Dimensional conversion.
static const double m2_per_cm2 = 0.0001;
// Based on Penning de Vries et al. 1989, page 63
// E is the assimilate conversion effiency
static double DM_to_C_factor (double E)
{
   return molWeightC/molWeightCH2O * (1.0 - (0.5673 - 0.5327 * E)) / E;
   // return 12.0/30.0 * (1.0 - (0.5673 - 0.5327 * E)) / E;
}

double
Production::remobilization (const double DS, const double dt)
{
  if (DS < ReMobilDS)
    {
      StemRes = 0.0;            // [g DM/m^2]
      return 0.0;
    }
  else if (StemRes < 1.0e-9)
    {
      StemRes = ShldResC * WStem;
      return 0.0;
    }
  else
    {
      const double ReMobilization = ReMobilRt / 24. * ReMobil_Mod (DS) * StemRes;
      StemRes -= ReMobilization * dt;
      return ReMobilization;
    }
}

double
Production::RSR () const
{
  const double shoot = WStem + WSOrg + WLeaf;
  const double root = WRoot;
  if (shoot < 20.0 || root < 20.0)
    return 0.33333;
  return root/shoot;
}

double
Production::DM () const
{
  return (WSOrg + WStem + WLeaf + WDead) * 10; // [g/m^2 -> kg/ha]
}

double
Production::total_N () const
{
  // kg/ha -> g/m^2
  const double conv = 1000.0 / (100.0 * 100.0);
  return (NCrop + NDead) / conv;
}

double
Production::total_C () const
{
  // kg/ha -> g/m^2
  const double conv = 1000.0 / (100.0 * 100.0);
  return (CCrop + CDead) / conv;
}

bool 
Production::root_growth () const
{ return DailyNetRoot > 0.0; }

bool
Production::shoot_growth () const
{ return DailyNetShoot > 0.0; }

double
Production::maintenance_respiration (double r, double w, double T)
{
  if (w <= 0.0)
    return 0.0;

  return (molWeightCH2O / molWeightCO2)
    * r / 24. 
    * std::max (0.0, 
                0.4281 * (exp (0.57 - 0.024 * T + 0.0020 * T * T)
                          - exp (0.57 - 0.042 * T - 0.0051 * T * T))) * w;
}

// Based on Penning de Vries et al. 1989, page 63
// Simple biochemical analysis
double
Production::GrowthRespCoef (double E)
{ return  0.5673 - 0.5327 * E; }

void
Production::tick (const double AirT, const double SoilT,
		  const std::vector<double>& Density,
		  const std::vector<double>& D,
		  const double D_tot_DM,
		  const Geometry& geo, const SoilWater& soil_water,
		  const double DS, const double CAImRat,
		  const CrpN& nitrogen,
                  const double nitrogen_stress,
		  const double NNI,
                  const double seed_C,
		  Partition& partition,
		  double& residuals_DM,
		  double& residuals_N_top, double& residuals_C_top,
		  std::vector<double>& residuals_N_soil,
		  std::vector<double>& residuals_C_soil,
                  const double dt,
		  Treelog& msg)
{
  daisy_assert (DS >= 0.0);
  const double LeafGrowthRespCoef = GrowthRespCoef (E_Leaf);
  const double StemGrowthRespCoef = GrowthRespCoef (E_Stem);
  const double SOrgGrowthRespCoef = GrowthRespCoef (E_SOrg);
  const double RootGrowthRespCoef = GrowthRespCoef (E_Root);
  const double DS1 = fmod (DS, 2.0);

  // Seed.
  const double seed_CH2O = seed_C * molWeightCH2O / molWeightC;
  CH2OPool += seed_CH2O;

  // Remobilization
  const double ReMobil = remobilization (DS, dt); // [g DM/m^2/h]
  const double CH2OReMobil                        // [g CH2O/m^2/h]
    = ReMobil * DM_to_C_factor (E_Stem) * 30.0/12.0;
  const double ReMobilResp = (1 - ReMobilEff) * CH2OReMobil; // [g CH2O/m^2/h]
  // Note:  We used to have "CH2OPool += Remobil", but that does not
  // give C balance.  ReMobilResp was invented, and gets it's strange
  // value, to get the same result but with a mass balance.  
  CH2OPool += CH2OReMobil * dt; // [g CH2O/m^2]

  // Release of root reserves
  bool ReleaseOfRootReserves = false;
  if (DS1 > IntDSRelRtRes && DS1 < EndDSRelRtRes)
    {
      if (WLeaf < LfRtRelRtRes * WRoot)
	{
	  const double RootRelease = RelRateRtRes * WRoot * dt / 24.;
	  CH2OPool += RootRelease * DM_to_C_factor (E_Root) * (30./12.);
	  WRoot -= RootRelease;
	  ReleaseOfRootReserves = true;
	}
    }

  // Photosyntheses.
  daisy_assert (std::isfinite (CanopyAss));
  CH2OPool += CanopyAss * dt;
  double NetAss = CanopyAss;    // [g CH2O/m^2/h]

  // Maintenance respiration.
  double RMLeaf                 // [g CH2O/m^2/h]
    = maintenance_respiration (r_Leaf, WLeaf, AirT) * MaintResp_DS(DS);
  daisy_assert (std::isfinite (RMLeaf));
  const double RMStem           // [g CH2O/m^2/h]
    = maintenance_respiration (r_Stem, WStem, AirT) * MaintResp_DS(DS);
  daisy_assert (std::isfinite (RMStem));
  const double RMSOrg           // [g CH2O/m^2/h]
    = maintenance_respiration (r_SOrg, WSOrg, AirT) * MaintResp_DS(DS);
  daisy_assert (std::isfinite (RMSOrg));
  const double RMRoot           // [g CH2O/m^2/h]
    = maintenance_respiration (r_Root, WRoot, SoilT) * MaintResp_DS(DS);
  daisy_assert (std::isfinite (RMRoot));

  // Water stress stops leaf respiration.
  daisy_assert (std::isfinite (PotCanopyAss));
  daisy_assert (std::isfinite (CanopyAss));
  RMLeaf = std::max (0.0, RMLeaf - PotCanopyAss + CanopyAss);
  daisy_assert (std::isfinite (RMLeaf));
  const double RM =             // [g CH2O/m^2/h]
    RMLeaf + RMStem + RMSOrg + RMRoot + ReMobilResp;
  Respiration = RM;
  MaintRespiration = RM;
  daisy_assert (std::isfinite (RM));
  NetAss -= RM;

  double LeafMResp = RMLeaf;     // [g CH2O/m^2/h]
  double StemMResp = RMStem;     // [g CH2O/m^2/h]
  double SOrgMResp = RMSOrg;     // [g CH2O/m^2/h]
  double RootMResp = RMRoot;     // [g CH2O/m^2/h]
  
  double LeafGResp = 0.0;
  double StemGResp = 0.0;
  double SOrgGResp = 0.0;
  double RootGResp = 0.0;
 
  daisy_assert (CH2OPool >= 0.0);
  if (CH2OPool >= RM * dt)
    {
      // We have enough assimilate to cover respiration.
      CH2OPool -= RM * dt;
      const double AssG = CH2OReleaseRate * CH2OPool;  // [g CH2O/m^2/h]
      CH2OPool -= AssG * dt;

      // Partition growth.
      double f_Leaf, f_Stem, f_SOrg, f_Root;

      partition.tick (DS, RSR (), WRoot, nitrogen_stress, NNI,
		      f_Leaf, f_Stem, f_Root, f_SOrg);
      if (ReleaseOfRootReserves)
        {
          f_Leaf += f_Root;
          f_Root = 0.0;
        }

      IncWLeaf = E_Leaf * f_Leaf * AssG; // [g DM/m^2/h]
      IncWStem = E_Stem * f_Stem * AssG - ReMobil; // [g DM/m^2/h]
      IncWSOrg = E_SOrg * f_SOrg * AssG; // [g DM/m^2/h]
      IncWRoot = E_Root * f_Root * AssG; // [g DM/m^2/h]

      GrowthRespiration         // [g CH2O/m^2/h]
        = LeafGrowthRespCoef * f_Leaf * AssG
        + StemGrowthRespCoef * f_Stem * AssG
        + SOrgGrowthRespCoef * f_SOrg * AssG
        + RootGrowthRespCoef * f_Root * AssG;
      const double IncC         // // [g C/m^2/h]
        = IncWLeaf * DM_to_C_factor (E_Leaf)
        + IncWStem * DM_to_C_factor (E_Stem)
        + IncWSOrg * DM_to_C_factor (E_SOrg)
        + IncWRoot * DM_to_C_factor (E_Root);
      if (!approximate (AssG - CH2OReMobil,
                        IncC * 30.0/12.0 + GrowthRespiration))
        {
          std::ostringstream tmp;
          tmp << "C inblance in growth\n"
              << "Assimilate = " <<  AssG * 10 * 12./30. << " kg C/ha/h\n"
              << "Remobil = " << CH2OReMobil * 10 * 12./30. << " kg C/ha/h\n"
              << "Growth = " <<  IncC * 10 << " kg C/ha/h\n"
              << "Resp = " <<  GrowthRespiration * 10 * 12./30.
              << " kg C/ha/h\nError " 
              << ((AssG - CH2OReMobil) * 12./30
                  - IncC - GrowthRespiration * 12./30) * 10 
              <<  " kg C/ha/h";
          msg.error (tmp.str ());
        }
      daisy_assert (std::isfinite (GrowthRespiration));
      NetAss -= GrowthRespiration;
      Respiration += GrowthRespiration;
      LeafGResp = LeafGrowthRespCoef * f_Leaf * AssG;
      StemGResp = StemGrowthRespCoef * f_Stem * AssG;
      SOrgGResp = SOrgGrowthRespCoef * f_SOrg * AssG;
      RootGResp = RootGrowthRespCoef * f_Root * AssG;
    }
  else
    {
      partition.tick_none ();

      // Too little assimilate to cover respiration.  
      // Give assimilate to most important parts first.

      // Remobilization should generate more assimilate than it
      // consumes.  We satisfy that process first. 
      daisy_assert (ReMobilResp * dt <= CH2OPool);
      CH2OPool -= ReMobilResp * dt;

      // We need leafs for photosyntheses, to generate more assimilate.
      if (RMLeaf * dt <= CH2OPool)
	{
	  IncWLeaf = 0.0;
	  CH2OPool -= RMLeaf * dt;
	}
      else
	{
	  IncWLeaf = (CH2OPool / dt - RMLeaf) * 12.0/30.0
            / DM_to_C_factor (E_Leaf);
	  CH2OPool = 0.0;
	}

      // Will somebody please think of the children!
      if (RMSOrg * dt <= CH2OPool)
	{
	  IncWSOrg = 0.0;
	  CH2OPool -= RMSOrg * dt;
	}
      else
	{
	  IncWSOrg = (CH2OPool - RMSOrg * dt) * 12.0/30.0
            / DM_to_C_factor (E_SOrg);
	  CH2OPool = 0.0;
	}

      // An upstanding crop.
      if (RMStem * dt <= CH2OPool)
	{
	  IncWStem = -ReMobil;
	  CH2OPool -= RMStem * dt;
	}
      else
	{
	  IncWStem = (CH2OPool / dt - RMStem) * 12.0/30.0
             / DM_to_C_factor (E_Stem) - ReMobil;
          // If we have more stem than sorg, and enough stem to cover
          // the loss of sorg, we do so.
          if (WStem + IncWStem * dt + IncWSOrg * dt >= 0.0
	      && WStem > WSOrg)
            {
              daisy_assert (IncWSOrg <= 0.0);
	      IncWStem += IncWSOrg * DM_to_C_factor (E_SOrg) 
                / DM_to_C_factor (E_Stem);
	      IncWSOrg  = 0.0;
            }
	  CH2OPool = 0.0;
	}

      // Roots are furthest away from the CH2O source, gets last.
      if (RMRoot * dt < CH2OPool)
	{
	  IncWRoot = 0.0;
	  CH2OPool -= RMRoot * dt;
	  std::ostringstream tmp;
	  tmp << "BUG: Extra CH2O: " << CH2OPool;
	  msg.error (tmp.str ());
	}
      else
	{
	  IncWRoot = (CH2OPool / dt - RMRoot) * 12.0/30.0
            / DM_to_C_factor (E_Root) ;
	  CH2OPool = 0.0;
	}
    }
  daisy_assert (std::isfinite (NetAss));
  NetPhotosynthesis = molWeightCO2 / molWeightCH2O * NetAss;
  AccNetPhotosynthesis += NetPhotosynthesis * dt;

  LeafGrowthRespiration = molWeightCO2 / molWeightCH2O * LeafGResp;  
  StemGrowthRespiration = molWeightCO2 / molWeightCH2O * StemGResp;  
  SOrgGrowthRespiration = molWeightCO2 / molWeightCH2O * SOrgGResp;
  RootGrowthRespiration = molWeightCO2 / molWeightCH2O * RootGResp;

  LeafMaintRespiration = molWeightCO2 / molWeightCH2O * LeafMResp;  
  StemMaintRespiration = molWeightCO2 / molWeightCH2O * StemMResp;  
  SOrgMaintRespiration = molWeightCO2 / molWeightCH2O * SOrgMResp;
  RootMaintRespiration = molWeightCO2 / molWeightCH2O * RootMResp;

  LeafRespiration = LeafGrowthRespiration + LeafMaintRespiration;
  StemRespiration = StemGrowthRespiration + StemMaintRespiration;
  SOrgRespiration = SOrgGrowthRespiration + SOrgMaintRespiration;
  RootRespiration = RootGrowthRespiration + RootMaintRespiration;

  // Update dead leafs
  DeadWLeaf = LfDR (DS) / 24.0 * WLeaf;
  DeadWLeaf += WLeaf * 0.333 * CAImRat / 24.0;
  daisy_assert (DeadWLeaf >= 0.0);
  double DdLeafCnc;
  daisy_assert (WLeaf >= 0.0);
  if (NCrop > 1.05 * nitrogen.PtNCnt)
    {
      if (WLeaf > 0.0)
        DdLeafCnc = NLeaf/WLeaf;
      else if (WStem > 0.0)
        DdLeafCnc = NStem/WStem;
      else 
	{
	  daisy_assert (DeadWLeaf < 1e-10);
	  DdLeafCnc = 1.0;
	}
    }
  else
    {
      if (WLeaf > 0.0)
        DdLeafCnc = (NLeaf / WLeaf - nitrogen.NfLeafCnc (DS))
          * ( 1.0 - nitrogen.TLLeafEff (DS)) +  nitrogen.NfLeafCnc (DS);
      else if (WStem > 0.0)
        DdLeafCnc = NStem/WStem;
      else
	{
	  daisy_assert (DeadWLeaf < 1e-10);
	  DdLeafCnc = 1.0;
	}
    }

  daisy_assert (DdLeafCnc >= 0.0);
  daisy_assert (DeadWLeaf >= 0.0);
  DeadNLeaf = DdLeafCnc * DeadWLeaf;
  daisy_assert (DeadNLeaf >= 0.0);
  IncWLeaf -= DeadWLeaf;
  daisy_assert (DeadWLeaf >= 0.0);
  WDead += (1.0 - ExfoliationFac) * DeadWLeaf * dt;
  NDead += (1.0 - ExfoliationFac) * DeadNLeaf * dt;
  daisy_assert (NDead >= 0.0);

  const double W_foli = ExfoliationFac * DeadWLeaf;
  const double C_foli = DM_to_C_factor (E_Leaf) * W_foli;
  C_Loss = C_foli;
  const double N_foli = ExfoliationFac * DeadNLeaf;
  daisy_assert (N_foli >= 0.0);
  if (C_foli < 1e-50)
    daisy_assert (N_foli < 1e-40);
  else
    {
      daisy_assert (N_foli > 0.0);
      AM_leaf->add (dt * C_foli * m2_per_cm2, dt * N_foli * m2_per_cm2);
      residuals_DM += W_foli;
      residuals_N_top += N_foli;
      residuals_C_top += C_foli;
      C_AM += C_foli * dt;
      N_AM += N_foli * dt;
    }

  // Root zone watter logging.
  const size_t cell_size = geo.cell_size ();
  daisy_assert (Density.size () == cell_size);
  double root_volume = 0.0; // [cm^3]
  double water_logged_volume = 0.0; // [cm^3]
  for (size_t c = 0; c < cell_size; c++)
    {
      if (Density[c] <= water_log_root_limit)
	continue;
      const double vol = geo.cell_volume (c);
      root_volume += vol;
      if (soil_water.h (c) < water_log_h_limit)
	continue;
      water_logged_volume += vol;
    }

  double WL_fac = 1.0;
  double WL_add = 0.0;
  if (root_volume > 0.0)
    {
      daisy_assert (water_logged_volume <= root_volume);
      water_logged = water_logged_volume / root_volume;
      WL_fac = RtDR_water_log_factor (water_logged);
      WL_add = RtDR_water_log_addend (water_logged);
    }
  else
    water_logged = NAN;
      
  // Update dead roots.
  double root_death_rate = (RtDR (DS) + WL_add)
    * RtDR_T_factor (SoilT) * WL_fac;
  
  if (RSR () > 1.1 * partition.RSR (DS))
    root_death_rate += Large_RtDR;

  if (root_death_rate > 0.0 && WRoot > 0.0)
    {
      {
	double WRoot_new_dummy = NAN;
	first_order_change (WRoot, 0.0, root_death_rate / 24.0, dt,
			    WRoot_new_dummy, DeadWRoot);
      }
      double DdRootCnc;
      const double ActRootCnc = NRoot/WRoot;
      if (NCrop > 1.05 * nitrogen.PtNCnt
	  || ActRootCnc < nitrogen.NfRootCnc (DS))
        DdRootCnc = ActRootCnc;
      else
	DdRootCnc = (ActRootCnc - nitrogen.NfRootCnc (DS))
	  * ( 1.0 - nitrogen.TLRootEff (DS)) +  nitrogen.NfRootCnc (DS);
      daisy_assert (DdRootCnc >= 0.0);
      DeadNRoot = DdRootCnc * DeadWRoot;
      IncWRoot -= DeadWRoot;
      const double C_Root = DM_to_C_factor (E_Root) * DeadWRoot;
      C_Loss += C_Root;
      daisy_assert (AM_root);
      daisy_assert (CRoot >= 0.0);
      daisy_assert (DeadNRoot >= 0.0);
      AM_root->add_surface (geo, dt * C_Root * m2_per_cm2,
                            dt * DeadNRoot * m2_per_cm2,
                            Density);
      daisy_assert (iszero (C_Root) || DeadNRoot > 0.0);
      residuals_DM += DeadWRoot;
      geo.add_surface (residuals_C_soil, Density, C_Root * m2_per_cm2);
      geo.add_surface (residuals_N_soil, Density, DeadNRoot * m2_per_cm2);
      C_AM += C_Root * dt;
      N_AM += DeadNRoot * dt;
    }
  else
    DeadWRoot = DeadNRoot = 0.0;

  // Local root death.
  if (D_tot_DM > 0.0)
    {
      const double ActRootCnc = NRoot / WRoot; // [g N/g DM]

      const double D_tot_N = D_tot_DM * ActRootCnc; // [g N/h]
      const double D_tot_C	       // [g C/h]
	= DM_to_C_factor (E_Root)      // [g C/g DM]
	* D_tot_DM;		       // [g DM/h]
      
      C_Loss += D_tot_C;
      daisy_assert (AM_root);
      daisy_assert (D_tot_C >= 0.0);
      daisy_assert (D_tot_N >= 0.0);
      const double surface_area = geo.surface_area ();
      AM_root->add_surface (geo,
			    D_tot_C * dt / surface_area,
			    D_tot_N * dt / surface_area,
			    D);
      residuals_DM += D_tot_DM * dt;
      geo.add_soil (residuals_C_soil, D, D_tot_C * dt);
      geo.add_soil (residuals_N_soil, D, D_tot_N * dt);
      WRoot -= D_tot_DM * dt;
      C_AM += D_tot_C * dt;
      N_AM += D_tot_N * dt;
      DeadWRoot += D_tot_DM;
      DeadNRoot += D_tot_N;
    }

  // Update production.
  NCrop -= (DeadNLeaf + DeadNRoot) * dt;
  daisy_assert (NCrop > 0.0);
  WLeaf += IncWLeaf * dt;
  if (WLeaf < 0.0)
    WLeaf = 0.0;
  WStem += IncWStem * dt;
  if (WStem < 0.0)
    WStem = 0.0;
  WSOrg += IncWSOrg * dt;
  if (WSOrg < 0.0)
    WSOrg = 0.0;
  WRoot += IncWRoot * dt;
  if (WRoot < 0.0)
    WRoot = 0.0;
  DailyNetRoot += (IncWRoot + DeadWRoot) * dt;
  DailyNetShoot += (IncWLeaf + DeadWLeaf + IncWStem) * dt;

  const double old_CCrop = CCrop;
  update_carbon ();
  const double error 
    = (old_CCrop 
       + (dt * (NetPhotosynthesis *  12./44. - C_Loss)
          + seed_C)
       - CCrop) * 10;
  static double accum = 0.0;
  accum += error;
  daisy_assert (std::isfinite (NetPhotosynthesis));
  if (!approximate (old_CCrop 
                    + dt * (NetPhotosynthesis *  12./44 - C_Loss)
                    + seed_C,
                    CCrop)
      || error > 0.00001 /* 0.001 g/ha */)
    {
      std::ostringstream tmp;
      tmp << "C balance error\n"
          << "Old C = " << old_CCrop * 10 << " kg/ha\n"
          << "NetPhotosynthesis = "
          << NetPhotosynthesis *  12./30. * 10 * dt
          << " kg/ha\n"
          << "Loss " << C_Loss * dt * 10 << " kg/ha\n"
          << "Seed " << seed_C * 10 << " kg/ha\n"
          << "New C = " << CCrop * 10 << " kg/ha\n"
          << "Error = " << error * 1000 << " g/ha\n"
          << "Accumulated = " << accum << " kg/ha"; 
      msg.error (tmp.str ());
    }
}

void
Production::tick_daily ()
{ DailyNetRoot = DailyNetShoot = 0.0; }

void 
Production::update_carbon ()
{
  CLeaf = WLeaf * DM_to_C_factor (E_Leaf);
  CStem = WStem * DM_to_C_factor (E_Stem);
  CSOrg = WSOrg * DM_to_C_factor (E_SOrg);
  CRoot = WRoot * DM_to_C_factor (E_Root);
  CDead = WDead * DM_to_C_factor (E_Leaf);
  CCrop = CLeaf + CStem + CSOrg + CRoot + CDead + CH2OPool * 12./30.;
}
void
Production::none ()
{
  IncWLeaf = 0.0;
  IncWStem = 0.0;
  IncWSOrg = 0.0;
  IncWRoot = 0.0;
  NetPhotosynthesis = 0.0;
  AccNetPhotosynthesis = 0.0;
  Respiration = 0.0;
  MaintRespiration = 0.0;
  GrowthRespiration = 0.0;
  LeafRespiration = 0.0;
  StemRespiration = 0.0;
  SOrgRespiration = 0.0;
  RootRespiration = 0.0;
  LeafMaintRespiration = 0.0;
  StemMaintRespiration = 0.0;
  SOrgMaintRespiration = 0.0;
  RootMaintRespiration = 0.0;
  LeafGrowthRespiration = 0.0;
  StemGrowthRespiration = 0.0;
  SOrgGrowthRespiration = 0.0;
  RootGrowthRespiration = 0.0;
  DeadWLeaf = 0.0;
  DeadNLeaf = 0.0;
  DeadWRoot = 0.0;
  DeadNRoot = 0.0;
  C_Loss = 0.0;
  DailyNetRoot = 0.0;
  DailyNetShoot = 0.0;
}

void 
Production::output (Log& log) const
{
  output_variable (StemRes, log);
  output_variable (CH2OPool, log);
  output_variable (WLeaf, log);
  output_variable (WStem, log);
  output_variable (WRoot, log);
  output_variable (WSOrg, log);
  output_variable (WDead, log);
  output_value (WLeaf + WStem + WRoot + WSOrg + WDead, "WCrop", log);
  output_variable (CLeaf, log);
  output_variable (CStem, log);
  output_variable (CRoot, log);
  output_variable (CSOrg, log);
  output_variable (CDead, log);
  output_variable (CCrop, log);
  output_variable (NLeaf, log);
  output_variable (NStem, log);
  output_variable (NRoot, log);
  output_variable (NSOrg, log);
  output_variable (NDead, log);
  output_variable (NCrop, log);
  output_variable (C_AM, log);
  output_variable (N_AM, log);
  output_variable (PotCanopyAss, log);
  output_variable (CanopyAss, log);
  output_variable (NetPhotosynthesis, log);
  output_variable (AccNetPhotosynthesis, log);
  output_variable (Respiration, log);
  output_variable (MaintRespiration, log);
  output_variable (GrowthRespiration, log);
  output_variable (LeafRespiration, log);
  output_variable (StemRespiration, log);
  output_variable (SOrgRespiration, log);
  output_variable (RootRespiration, log);
  output_variable (LeafMaintRespiration, log);
  output_variable (StemMaintRespiration, log);
  output_variable (SOrgMaintRespiration, log);
  output_variable (RootMaintRespiration, log);
  output_variable (LeafGrowthRespiration, log);
  output_variable (StemGrowthRespiration, log);
  output_variable (SOrgGrowthRespiration, log);
  output_variable (RootGrowthRespiration, log);
  output_variable (IncWLeaf, log);
  output_variable (IncWStem, log);
  output_variable (IncWSOrg, log);
  output_variable (IncWRoot, log);
  output_variable (DeadWLeaf, log);
  output_variable (DeadNLeaf, log);
  output_variable (DeadWRoot, log);
  output_variable (DeadNRoot, log);
  output_variable (C_Loss, log);
  output_variable (DailyNetRoot, log);
  output_variable (DailyNetShoot, log);
  if (std::isfinite (water_logged))
    output_variable (water_logged, log);
  
}

void 
Production::load_syntax (Frame& frame)
{

  // Remobilization.
  frame.declare ("ShldResC", Attribute::Fraction (), Attribute::Const,
	      "Capacity of shielded reserves (fraction of stem DM).");
  frame.set ("ShldResC", 0.0);
  frame.declare ("ReMobilEff", Attribute::Fraction (), Attribute::Const,
          "Efficiency of reserve mobilization");
  frame.set ("ReMobilEff", 0.85);
  frame.declare ("ReMobilDS", "DS", Attribute::Const,
	      "Remobilization, Initial DS.");
  frame.set ("ReMobilDS", 1.20);
  frame.declare ("ReMobilRt", "d^-1", Attribute::Const,
	      "Remobilization, release rate.");
  frame.set ("ReMobilRt", 0.1);
  frame.declare ("ReMobil_Mod", "DS", Attribute::None (), Attribute::Const,
        "Modifier of remobilization rate depending on DS.");
  frame.set ("ReMobil_Mod", PLF::always_1 ());
  frame.declare ("StemRes", "g DM/m^2", Attribute::State,
	      "Shielded reserves in stems.");
  frame.set ("StemRes", 0.0);

  // Parameters.
  frame.declare ("CH2OReleaseRate", "h^-1", Attribute::Const,
	      "CH2O Release Rate constant.");
  frame.set ("CH2OReleaseRate", 0.04);
  frame.declare ("E_Root", "g DM-C/g Ass-C", Attribute::Const,
	      "Conversion efficiency, root.");
  frame.set ("E_Root", 0.69);
  frame.declare ("E_Leaf", "g DM-C/g Ass-C", Attribute::Const,
	      "Conversion efficiency, leaf.");
  frame.set ("E_Leaf", 0.68);
  frame.declare ("E_Stem", "g DM-C/g Ass-C", Attribute::Const,
	      "Conversion efficiency, stem.");
  frame.set ("E_Stem", 0.66);
  frame.declare ("E_SOrg", "g DM-C/g Ass-C", Attribute::Const,
	      "Conversion efficiency, storage organ.");
  frame.declare ("MaintResp_DS", "DS", Attribute::None (), Attribute::Const,
          "Reduction of maintenance respiration with crop senescence");
  frame.set ("MaintResp_DS", PLF::always_1 ());
  frame.declare ("r_Root", "d^-1", Attribute::Const,
	      "Maintenance respiration coefficient, root.");
  frame.set ("r_Root", 0.015);
  frame.declare ("r_Leaf", "d^-1", Attribute::Const,
	      "Maintenance respiration coefficient, leaf.");
  frame.declare ("r_Stem", "d^-1", Attribute::Const,
	      "Maintenance respiration coefficient, stem.");
  frame.declare ("r_SOrg", "d^-1", Attribute::Const,
	      "Maintenance respiration coefficient, storage organ.");
  frame.declare ("ExfoliationFac", "h^-1", Attribute::Const,
	      "Exfoliation factor, 0-1.");
  frame.set ("ExfoliationFac", 1.0);
  frame.declare ("LfDR", "DS", "d^-1", Attribute::Const,
	      "Death rate of Leafs.");
  frame.declare ("RtDR", "DS", "d^-1", Attribute::Const,
	      "Death rate of Roots.");
  frame.declare ("Large_RtDR", "d^-1", Attribute::Const,
	      "Extra death rate for large root/shoot.");
  frame.set ("Large_RtDR", 0.05);
  frame.declare ("RtDR_T_factor", "dg C", Attribute::None (), Attribute::Const,
	      "Temperature dependent factor for root death rate.");
  frame.set ("RtDR_T_factor", PLF::always_1 ());
  frame.declare ("water_log_h_limit", "cm", Attribute::Const, "\
Roots are water logged if the soil water potential exceed this threshold.");
  frame.set ("water_log_h_limit", 0.0);
  frame.declare ("water_log_root_limit", "cm/cm^3", Attribute::Const, "\
Soil is part of the root zone if root density is above this threshold. \n\
Set it to zero to use the calculated root depth instead.");
  frame.set ("water_log_root_limit", 0.1);
  frame.declare ("RtDR_water_log_factor",
		 Attribute::Fraction (), Attribute::None (),
		  Attribute::Const, "\
Water logging dependent factor for root death rate.\n\
This is a function of the fraction of the root zone being water logged.");
  frame.set ("RtDR_water_log_factor", PLF::always_1 ());
  frame.declare ("RtDR_water_log_addend",
		 Attribute::Fraction (), "d^-1",
		 Attribute::Const, "\
Water logging dependent addend for root death rate.\n			\
This is a function of the fraction of the root zone being water logged.");
  frame.set ("RtDR_water_log_addend", PLF::always_0 ());
  frame.declare ("IntDSRelRtRes", Attribute::None (), Attribute::Const,
	      "Initial DS for the release of root reserves.");
  frame.set ("IntDSRelRtRes", 0.80);
  frame.declare ("EndDSRelRtRes", Attribute::None (), Attribute::Const,
	      "End DS for the release of root reserves.");
  frame.set ("EndDSRelRtRes", 0.80);
  frame.declare ("RelRateRtRes", "d^-1", Attribute::Const,
	      "Release rate of root reserves.");
  frame.set ("RelRateRtRes", 0.05);
  frame.declare ("LfRtRelRtRes", Attribute::None (), Attribute::Const,
	      "Max Leaf:Root for the release of root res.");
  frame.set ("LfRtRelRtRes", 0.80);

  // Variables.
  frame.declare ("CH2OPool", "g CH2O/m^2", Attribute::State, "CH2O Pool.");
  frame.set ("CH2OPool", 0.001);
  frame.declare ("WLeaf", "g DM/m^2", Attribute::State, "Leaf dry matter weight.");
  frame.set ("WLeaf", 0.001);
  frame.declare ("WStem", "g DM/m^2", Attribute::State, "Stem dry matter weight.");
  frame.set ("WStem", 0.000);
  frame.declare ("WRoot", "g DM/m^2", Attribute::State, "Root dry matter weight.");
  frame.set ("WRoot", 0.001);
  frame.declare ("WSOrg", "g DM/m^2", Attribute::State,
	      "Storage organ dry matter weight.");
  frame.set ("WSOrg", 0.000);
  frame.declare ("WDead", "g DM/m^2", Attribute::State,
	      "Dead leaves dry matter weight.");
  frame.set ("WDead", 0.000);
  frame.declare ("WCrop", "g DM/m^2", Attribute::LogOnly, "\
Total crop dry matter, including roots and dead leaves.");
  frame.declare ("CLeaf", "g C/m^2", Attribute::LogOnly, "Leaf C weight.");
  frame.declare ("CStem", "g C/m^2", Attribute::LogOnly, "Stem C weight.");
  frame.declare ("CRoot", "g C/m^2", Attribute::LogOnly, "Root C weight.");
  frame.declare ("CSOrg", "g C/m^2", Attribute::LogOnly, "Storage organ C weight.");
  frame.declare ("CDead", "g C/m^2", Attribute::LogOnly, "Dead leaves C weight.");
  frame.declare ("CCrop", "g C/m^2", Attribute::LogOnly, "Crop C weight.");
  frame.declare ("NLeaf", "g N/m^2", Attribute::State,
	      "Nitrogen stored in the leaves.");
  frame.set ("NLeaf", 0.000);
  frame.declare ("NStem", "g N/m^2", Attribute::State,
	      "Nitrogen stored in the stem.");
  frame.set ("NStem", 0.000);
  frame.declare ("NRoot", "g N/m^2", Attribute::State,
	      "Nitrogen stored in the roots.");
  frame.set ("NRoot", 0.000);
  frame.declare ("NSOrg", "g N/m^2", Attribute::State,
	      "Nitrogen stored in the storage organ.");
  frame.set ("NSOrg", 0.000);
  frame.declare ("NDead", "g N/m^2", Attribute::State,
	      "Nitrogen stored in dead leaves.");
  frame.set ("NDead", 0.000);
  frame.declare ("NCrop", "g N/m^2", Attribute::OptionalState,
	      "Total crop nitrogen content.\n\
By default, this will start as the amount of N in the seed.");
  frame.declare ("C_AM", "g C/m^2", Attribute::State,
	      "Added C in plant material.");
  frame.set ("C_AM", 0.000);
  frame.declare ("N_AM", "g N/m^2", Attribute::State,
	      "Added N in plant material.");
  frame.set ("N_AM", 0.000);
  
  // Auxiliary.
  frame.declare ("PotCanopyAss", "g CH2O/m^2/h", Attribute::LogOnly,
	      "Potential canopy assimilation, i.e. stressfree production.");
  frame.declare ("CanopyAss", "g CH2O/m^2/h", Attribute::LogOnly,
	      "Canopy assimilation.");
  frame.declare ("NetPhotosynthesis", "g CO2/m^2/h", Attribute::LogOnly,
	      "Net Photosynthesis.");
  frame.declare ("AccNetPhotosynthesis", "g CO2/m^2", Attribute::LogOnly,
	      "Accumulated Net Photosynthesis.");
  frame.declare ("Respiration", "g CH2O/m^2/h", Attribute::LogOnly,
	      "Crop Respiration.");
  frame.declare ("MaintRespiration", "g CH2O/m^2/h", Attribute::LogOnly,
	      "Maintenance Respiration.");
  frame.declare ("GrowthRespiration", "g CH2O/m^2/h", Attribute::LogOnly,
	      "Growth Respiration.");
  frame.declare ("LeafRespiration", "g CO2/m^2/h", Attribute::LogOnly,
	      "Total Leaf Respiration.");
  frame.declare ("StemRespiration", "g CO2/m^2/h", Attribute::LogOnly,
	      "Total Stem Respiration.");
  frame.declare ("SOrgRespiration", "g CO2/m^2/h", Attribute::LogOnly,
	      "Total SOrg Respiration.");
  frame.declare ("RootRespiration", "g CO2/m^2/h", Attribute::LogOnly,
	      "Total Root Respiration.");
  frame.declare ("LeafMaintRespiration", "g CO2/m^2/h", Attribute::LogOnly,
	      "Leaf Maintenance Respiration.");
  frame.declare ("StemMaintRespiration", "g CO2/m^2/h", Attribute::LogOnly,
	      "Stem Maintenance Respiration.");
  frame.declare ("SOrgMaintRespiration", "g CO2/m^2/h", Attribute::LogOnly,
	      "SOrg Maintenance Respiration.");
  frame.declare ("RootMaintRespiration", "g CO2/m^2/h", Attribute::LogOnly,
	      "Root Maintenance Respiration.");
  frame.declare ("LeafGrowthRespiration", "g CO2/m^2/h", Attribute::LogOnly,
	      "Leaf Growth Respiration.");
  frame.declare ("StemGrowthRespiration", "g CO2/m^2/h", Attribute::LogOnly,
	      "Stem Growth Respiration.");
  frame.declare ("SOrgGrowthRespiration", "g CO2/m^2/h", Attribute::LogOnly,
	      "SOrg Growth Respiration.");
  frame.declare ("RootGrowthRespiration", "g CO2/m^2/h", Attribute::LogOnly,
	      "Root Growth Respiration.");
  frame.declare ("IncWLeaf", "g DM/m^2/h", Attribute::LogOnly,
	      "Leaf growth.");
  frame.declare ("IncWStem", "g DM/m^2/h", Attribute::LogOnly,
	      "Stem growth.");
  frame.declare ("IncWSOrg", "g DM/m^2/h", Attribute::LogOnly,
	      "Storage organ growth.");
  frame.declare ("IncWRoot", "g DM/m^2/h", Attribute::LogOnly,
	      "Root growth.");
  frame.declare ("DeadWLeaf", "g DM/m^2/h", Attribute::LogOnly,
	      "Leaf DM removed.");
  frame.declare ("DeadNLeaf", "g N/m2/h", Attribute::LogOnly,
	      "Leaf N removed.");
  frame.declare ("DeadWRoot", "g DM/m^2/h", Attribute::LogOnly,
	      "Root DM removed.");
  frame.declare ("DeadNRoot", "g N/m2/h", Attribute::LogOnly,
	      "Root N removed.");
  frame.declare ("C_Loss", "g C/m^2/h", Attribute::LogOnly,"C lost from the crop");
  frame.declare ("DailyNetRoot", "g DM/m^2", Attribute::State,
	      "Root growth minus root respiration so far this day.");
  frame.set ("DailyNetRoot", 0.0);
  frame.declare ("DailyNetShoot", "g DM/m^2", Attribute::State,
	      "Leaf growth minus leaf respiration so far this day.");
  frame.set ("DailyNetShoot", 0.0);
  frame.declare_fraction ("water_logged", Attribute::LogOnly, "\
Fraction of root zone water logged.");
}

void
Production::initialize (const double SeedN)
{
  if (NCrop <= 0.0)
    NCrop = SeedN;
}

void
Production::initialize (const Metalib& metalib, const symbol name, 
			const std::vector<boost::shared_ptr<const FrameModel>/**/>& root,
			const std::vector<boost::shared_ptr<const FrameModel>/**/>& dead,
			const Geometry& geo,
			OrganicMatter& organic_matter, Treelog& msg)
{
  // Hotstart, find pool in organic matter.
  daisy_assert (AM_root == NULL);
  static const symbol root_symbol ("root");
  AM_root = organic_matter.find_am (name, root_symbol);
  daisy_assert (AM_leaf == NULL);
  static const symbol dead_symbol ("dead");
  AM_leaf = organic_matter.find_am (name, dead_symbol);

  // If not found, we is planting emerged crops.  Create pools.
  if (!AM_root)
    {
      AM_root = &AM::create (metalib, geo, Time (1, 1, 1, 1), root,
			     name, root_symbol, AM::Locked, msg);
      organic_matter.add (*AM_root);
    }
	  
  if (!AM_leaf)
    {
      AM_leaf = &AM::create (metalib, geo, Time (1, 1, 1, 1), dead,
			     name, dead_symbol, AM::Locked, msg);
      organic_matter.add (*AM_leaf);
    }
  daisy_assert (AM_root);
  daisy_assert (AM_leaf);
}

Production::Production (const FrameSubmodel& al)
  : ShldResC (al.number ("ShldResC")),
    ReMobilEff (al.number ("ReMobilEff")),
    ReMobilDS (al.number ("ReMobilDS")),
    ReMobilRt (al.number ("ReMobilRt")),
    ReMobil_Mod (al.plf ("ReMobil_Mod")),
    StemRes (al.number ("StemRes")),
    // Parameters.
    CH2OReleaseRate (al.number ("CH2OReleaseRate")),
    E_Root (al.number ("E_Root")),
    E_Leaf (al.number ("E_Leaf")),
    E_Stem (al.number ("E_Stem")),
    E_SOrg (al.number ("E_SOrg")),
    MaintResp_DS (al.plf ("MaintResp_DS")),
    r_Root (al.number ("r_Root")),
    r_Leaf (al.number ("r_Leaf")),
    r_Stem (al.number ("r_Stem")),
    r_SOrg (al.number ("r_SOrg")),
    ExfoliationFac (al.number ("ExfoliationFac")),
    LfDR (al.plf ("LfDR")),
    RtDR (al.plf ("RtDR")),
    Large_RtDR (al.number ("Large_RtDR")),
    RtDR_T_factor (al.plf ("RtDR_T_factor")),
    water_log_h_limit (al.number ("water_log_h_limit")),
    water_log_root_limit (al.number ("water_log_root_limit")),
    RtDR_water_log_factor (al.plf ("RtDR_water_log_factor")),
    RtDR_water_log_addend (al.plf ("RtDR_water_log_addend")),
    IntDSRelRtRes (al.number ("IntDSRelRtRes")),
    EndDSRelRtRes (al.number ("EndDSRelRtRes")),
    RelRateRtRes (al.number ("RelRateRtRes")),
    LfRtRelRtRes (al.number ("LfRtRelRtRes")),

    // State.
    CH2OPool (al.number ("CH2OPool")),
    WLeaf (al.number ("WLeaf")),
    WStem (al.number ("WStem")),
    WRoot (al.number ("WRoot")),
    WSOrg (al.number ("WSOrg")),
    WDead (al.number ("WDead")),
    CLeaf (WLeaf * DM_to_C_factor (E_Leaf)),
    CStem (WStem * DM_to_C_factor (E_Stem)),
    CRoot (WRoot * DM_to_C_factor (E_Root)),
    CSOrg (WSOrg * DM_to_C_factor (E_SOrg)),
    CDead (WDead * DM_to_C_factor (E_Leaf)),
    CCrop (CLeaf + CStem + CSOrg + CRoot + CDead + CH2OPool * 12./30.),
    NCrop (al.number ("NCrop", -42.42e42)),
    NLeaf (al.number ("NLeaf")),
    NStem (al.number ("NStem")),
    NRoot (al.number ("NRoot")),
    NSOrg (al.number ("NSOrg")),
    NDead (al.number ("NDead")),
    C_AM  (al.number ("C_AM")),
    N_AM  (al.number ("N_AM")),
    AM_root (NULL),
    AM_leaf (NULL),
    // Auxiliary.
    PotCanopyAss (0.0),
    CanopyAss (0.0),
    NetPhotosynthesis (0.0),
    AccNetPhotosynthesis (0.0),
    Respiration (0.0),
    MaintRespiration (0.0),
    GrowthRespiration (0.0),
    LeafRespiration (0.0),
    StemRespiration (0.0),
    SOrgRespiration (0.0),
    RootRespiration (0.0),
    LeafMaintRespiration (0.0),
    StemMaintRespiration (0.0),
    SOrgMaintRespiration (0.0),
    RootMaintRespiration (0.0),
    LeafGrowthRespiration (0.0),
    StemGrowthRespiration (0.0),
    SOrgGrowthRespiration (0.0),
    RootGrowthRespiration (0.0),
    IncWLeaf (0.0),
    IncWStem (0.0),
    IncWSOrg (0.0),
    IncWRoot (0.0),
    DeadWLeaf (0.0),
    DeadNLeaf (0.0),
    DeadWRoot (0.0),
    DeadNRoot (0.0),
    C_Loss (0.0),
    DailyNetRoot  (al.number ("DailyNetRoot")),
    DailyNetShoot  (al.number ("DailyNetShoot")),
    water_logged (NAN)
{ }

Production::~Production ()
{ }

static DeclareSubmodel 
production_submodel (Production::load_syntax, "Production", "\
Crop production in the default crop model.");

// production.C ends here.

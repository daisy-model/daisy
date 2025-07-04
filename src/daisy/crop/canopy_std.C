// canopy_std.C -- Canopy development for standard crop model.
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

#include "daisy/crop/canopy_std.h"
#include "daisy/output/log.h"
#include "object_model/block_submodel.h"
#include "util/mathlib.h"
#include "object_model/librarian.h"

double
CanopyStandard::specific_LAI (const double DS)
{ return SpLAI * LeafAIMod (DS); }

double
CanopyStandard::PhotSOrg (const double DS)
{ return SOrgPhotEff * SOrgEffvsDS (DS); }

double
CanopyStandard::PhotStem (const double DS)
{ return StemPhotEff * StemEffvsDS (DS); }

double
CanopyStandard::CropHeight (double WStem, double DS) const
{
  const double H1 = HvsDS (DS) + Offset;
  const double H2 = HvsDS (1.0) * HvsWStem (WStem);
  return std::max (std::min (H1, H2), 0.01);
}

double
CanopyStandard::DS_at_height (double height) const
{ 
  daisy_assert (height >= 0.0);
  const PLF DSvsH (HvsDS.inverse_safe ());
  const double DS = DSvsH (height);
  daisy_assert (DS >= 0.0);
  daisy_assert (DS <= 2.0);
  return DS;
}

void
CanopyStandard::CropCAI (double WLeaf, double WSOrg, double WStem, double DS)
{
  LeafAI = SpLAI    * LeafAIMod (DS) * WLeaf;
  SOrgAI = SpSOrgAI * SOrgAIMod (DS) * WSOrg;
  StemAI = SpStemAI * StemAIMod (DS) * WStem;

  CAI = LeafAI + StemPhotEff * StemEffvsDS (DS) * StemAI + SOrgPhotEff * SOrgEffvsDS (DS) * SOrgAI;
}

double 
CanopyStandard::corresponding_WLeaf (const double DS) const
{ return CAI / (SpLAI * LeafAIMod (DS)); }

void
CanopyStandard::CanopyStructure (double DS)
{
  // The leaf density is assumed to be zero from the group up to height z0,
  // then increase linearly until height z1, continue at that
  // density until z2, and then decrease linearly until the top of
  // the crop.  The values of z1, z2, and z3 are scaled so the
  // ground height is zero and the top of the crop is 1.

  double z0;			// Height of first leaf [0 - 1].
  double z1;			// Min height of the MaxLAD area [0 - 1].
  double z2;			// Max height of the MaxLAD area [0 - 1].

  daisy_assert (DS > 0.0);
  if (DS <= 1)
    {
      // LAIDist0 is the leaf density at DS = 0, and LAIDist 1
      // is the leaf density at DS = 1.  The leaf density is
      // assumed to develop linearly as a function of DS between
      // DS 0 and DS 1.

      z0 = LAIDist0[0] + (LAIDist1[0] - LAIDist0[0]) * DS;
      z1 = LAIDist0[1] + (LAIDist1[1] - LAIDist0[1]) * DS;
      z2 = LAIDist0[2] + (LAIDist1[2] - LAIDist0[2]) * DS;
      // Area spanned by z0, z1, and z2.
      const double Area = (1.0 + z2 - z1 - z0) / 2.0;
      daisy_assert (Area > 0.0);
      daisy_assert (Height > 0.0);
      LADm = CAI / (Area * Height);
    }
  else
    {
      z0 = LAIDist1[0];
      z1 = LAIDist1[1];
      z2 = LAIDist1[2];
      // Area spanned by z0, z1, and z2.
      const double Area = (1.0 + z2 - z1 - z0) / 2.0;
      const double MaxLAD = CAI / (Area * Height);

      if (MaxLAD > LADm)
	// After DS = 1 CAI may increase for some time, keeping
	// z0, z1, and z2 constant but adding to MaxLAD.
	LADm = MaxLAD;
      else
	{
	  // Then the crop start eating itself.  This is done by
	  // keeping MaxLAD, (z1 - z0), and z2 constant but
	  // increasing z1 and z0.

	  // Need is the Area we want after moving z1 and z0.
	  daisy_assert (Area * 1.0001 >= CAI / Height / LADm);
	  const double Need = std::min (CAI / Height / LADm, Area);
	  daisy_assert (Need <= Area);
	  daisy_assert (Need > 0);

	  if (Area - Need > z2 - z1)
	    // We have to move z1 beyond z2.
	    {
	      // Let x0 be the height where the canopy starts, x1 the
	      // height where the canopy top, and y1 the canopy at x1.
	      //
	      // We know the area of the triangle:
	      // (1):  Need = (1 - x0) * y1 / 2
	      //
	      // We assume that the slope of the canopy increase and
	      // descrease is unchanged:
	      // (2):   (1 - x1) / y1 =  (1 - z2) / 1
	      // (3):  (x1 - x0) / y1 = (z1 - z0) / 1
	      //
	      // This gives us three equations with three unknown.
	      // We can solve them to get x0, x1, and y1.

	      const double x0 = 1.0 - sqrt (2.0 * Need * (z1 - z2 - z0 + 1.0));
	      const double x1
		= 1.0 + (z2 - 1) * sqrt (2.0 * Need / (z1 - z2 - z0 + 1.0));
	      const double y1 = sqrt (2.0 * Need / (z1 - z2 - z0 + 1.0));

	      // Check the results.
	      daisy_assert (approximate (Need, (1.0 - x0) * y1 / 2.0));
	      daisy_assert (approximate ((1.0 - x1) / y1, (1.0 - z2)));
	      daisy_assert (approximate ((x1 - x0) / y1, (z1 - z0)));

	      // Insert this special distribution, and return.
	      PLF LADvsH;
	      LADvsH.add (x0 * Height, 0.0);
	      LADvsH.add (x1 * Height, y1 * LADm);
	      LADvsH.add (     Height, 0.0);
	      LAIvsH = LADvsH.integrate_stupidly ();
	      return;
	    }
	  // It is enough to z1 closer to z2.
	  z1 += Area - Need;
	  z0 += Area - Need;
	}
    }

  // Create PLF for standard "z0, z1, z2" distribution.
  PLF LADvsH;
  LADvsH.add (z0 * Height, 0.0);
  LADvsH.add (z1 * Height, LADm);
  LADvsH.add (z2 * Height, LADm);
  LADvsH.add (     Height, 0.0);
  LAIvsH = LADvsH.integrate_stupidly ();
  const double CAIm = - log (PARrel) / PARext;
  CAImRat = std::max (0.0, (CAI - CAIm) / CAIm);
}

void
CanopyStandard::cut (double WStem, double DS, double stub_length)
{
  Offset = std::min (stub_length, Height) - HvsDS (DS);
  Height = CropHeight (WStem, DS);
}

void
CanopyStandard::tick (const double WLeaf, const double WSOrg,
                      const double WStem, const double DS,
		      const double force_CAI)
{
  Height = CropHeight (WStem, DS);
  CropCAI (WLeaf, WSOrg, WStem, DS);

  // Forced CAI.
  ForcedCAI = force_CAI;
  SimCAI = CAI;
  if (force_CAI >= 0.0)
    CAI = force_CAI;
}

void
CanopyStandard::output (Log& log) const
{
  CanopySimple::output (log);
  
  output_variable (Offset, log);
  output_variable (LeafAI, log);
  output_variable (StemAI, log);
  output_variable (SOrgAI, log);
  output_variable (LADm, log);
  if (ForcedCAI >= 0.0)
    output_variable (ForcedCAI, log);
  if (SimCAI >= 0.0)
    output_variable (SimCAI, log);
  output_variable (CAImRat, log);
}

void 
CanopyStandard::load_syntax (Frame& frame)
{
  CanopySimple::load_syntax (frame);

  // Parameters.
  frame.declare ("SpLAI", "(m^2/m^2)/(g DM/m^2)", Attribute::Const,
	      " Specific leaf weight.");
  frame.declare ("LeafAIMod", "DS", Attribute::None (), Attribute::Const,
	      "Specific leaf weight modifier.\n\
Used only after the intital phase.");
  PLF AIDef;
  AIDef.add (0.00, 1.00);
  AIDef.add (2.00, 1.00);
  frame.set ("LeafAIMod", AIDef);
  frame.declare ("SpSOrgAI", "(m^2/m^2)/(g DM/m^2)", Attribute::Const,
	      "Specific storage organ weight.\n\
Used only after the intital phase.");
  frame.set ("SpSOrgAI", 0.0);
  frame.declare ("SOrgAIMod", "DS", Attribute::None (), Attribute::Const,
	      "Specific storage organ weight modifier.\n\
Used only after the intital phase.");
  frame.set ("SOrgAIMod", AIDef);
  frame.declare ("SOrgPhotEff", Attribute::None (), Attribute::Const,
	      "Relative photosynthetic efficiency of storage organ.\n\
Used only after the intital phase.");
  frame.set ("SOrgPhotEff", 1.0);
  frame.declare ("SOrgEffvsDS", "DS", Attribute::None (), Attribute::Const,
        "SOrg photosynthetic efficiency as function of DS.");
  frame.set ("SOrgEffvsDS", PLF::always_1 ());
  frame.declare ("SpStemAI", "(m^2/m^2)/(g DM/m^2)", Attribute::Const,
	      "Specific stem weight.\n\
Used only after the intital phase.");
  frame.set ("SpStemAI", 0.0);
  frame.declare ("StemAIMod", "DS", Attribute::None (), Attribute::Const,
	      "Specific stem weight modifier.\n\
Used only after the intital phase.");
  frame.set ("StemAIMod", AIDef);
  frame.declare ("StemPhotEff", Attribute::None (), Attribute::Const,
	      "Relative photosynthetic efficiency of stem.\n\
Used only after the intital phase.");
  frame.set ("StemPhotEff", 1.0);
  frame.declare ("StemEffvsDS", "DS", Attribute::None (), Attribute::Const,
        "Stem photosynthetic efficiency as function of DS.");
  frame.set ("StemEffvsDS", PLF::always_1 ());
  frame.declare ("HvsDS", "DS", "cm", Attribute::Const,
	      "Crop height as function of DS.");
  PLF HvsStem;
  HvsStem.add (0.00 , 0.10);
  HvsStem.add (200.0, 1.00);
  frame.declare ("HvsWStem", "g DM/m^2", Attribute::Fraction (), Attribute::Const,
	      "Relative crop height as function of stem weight.\n\
By default, it needs 200 g DM/m^2 to reach full height.");
  frame.set ("HvsWStem", HvsStem);
  frame.declare ("LAIDist0", Attribute::None (), Attribute::Const, 3,
	      "Relative CAI distribution at DS=0.");
  frame.declare ("LAIDist1", Attribute::None (), Attribute::Const, 3,
	      "Relative CAI distribution at DS=1.");
  frame.declare ("PARrel", Attribute::None (), Attribute::Const,
	      "Relative PAR below the canopy.\n\
If the relative PAR get below this, the bottom leaves will start dying.");
  frame.set ("PARrel", 0.05);

  // Variables.
  frame.declare ("Offset", "cm", Attribute::State, "Extra height after harvest.");
  frame.set ("Offset", 0.0);
  frame.declare ("LeafAI", "m^2/m^2", Attribute::State, "Leaf Area Index.");
  frame.set ("LeafAI", 0.0);
  frame.declare ("StemAI", "m^2/m^2", Attribute::State, "Stem Area Index.");
  frame.set ("StemAI", 0.0);
  frame.declare ("SOrgAI", "m^2/m^2", Attribute::State, "Storage Organ Area Index.");
  frame.set ("SOrgAI", 0.0);
  frame.declare ("LADm", "cm^2/cm^3", Attribute::State,
	      "Maximal Leaf Area Density.");
  frame.set ("LADm", -9999.99);

  // Log Variables.
  frame.declare ("ForcedCAI", "m^2/m^2", Attribute::LogOnly,
	      "CAI forced upon us by vegetation module.");
  frame.declare ("SimCAI", "m^2/m^2", Attribute::LogOnly,
	      "CAI simulated by crop model.");
  frame.declare ("CAImRat", Attribute::None (), Attribute::LogOnly,
	      "(CAIm - CAI) / CAIm.");
}

CanopyStandard::CanopyStandard (const BlockSubmodel& vl)
  : CanopySimple (vl),
    SpLAI (vl.number ("SpLAI")),
    LeafAIMod (vl.plf ("LeafAIMod")),
    SpSOrgAI (vl.number ("SpSOrgAI")),
    SOrgAIMod (vl.plf ("SOrgAIMod")),
    SOrgPhotEff (vl.number ("SOrgPhotEff")),
    SOrgEffvsDS (vl.plf ("SOrgEffvsDS")),
    SpStemAI (vl.number ("SpStemAI")),
    StemAIMod (vl.plf ("StemAIMod")),
    StemPhotEff (vl.number ("StemPhotEff")),
    StemEffvsDS (vl.plf ("StemEffvsDS")),
    HvsDS (vl.plf ("HvsDS")),
    HvsWStem (vl.plf ("HvsWStem")),
    LAIDist0 (vl.number_sequence ("LAIDist0")),
    LAIDist1 (vl.number_sequence ("LAIDist1")),
    PARrel (vl.number ("PARrel")),
    Offset (vl.number ("Offset")),
    LeafAI (vl.number ("LeafAI")),
    StemAI (vl.number ("StemAI")),
    SOrgAI (vl.number ("SOrgAI")),
    LADm (vl.number ("LADm")),
    ForcedCAI (-1.0),
    SimCAI (-1.0),
    CAImRat (0.0)
{ }

CanopyStandard::~CanopyStandard ()
{ }

static DeclareSubmodel 
canopy_standard_submodel (CanopyStandard::load_syntax, "CanopyStandard", "\
Standard canopy model.");

// geometry1d.C  -- One dimensional discretization.
// 
// Copyright 1996-2001, 2006 Per Abrahamsen and Søren Hansen
// Copyright 2000-2001, 2006 KVL.
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

#include "daisy/soil/transport/geometry1d.h"
#include "daisy/soil/transport/volume.h"
#include "object_model/block.h"
#include "object_model/frame.h"
#include "util/mathlib.h"
#include "object_model/check.h"
#include "object_model/vcheck.h"
#include "object_model/librarian.h"
#include "util/assertion.h"
#include "object_model/treelog.h"
#include <sstream>

std::string
Geometry1D::edge_name (const size_t e) const
{
  if (e == 0)
    return "0";

  std::ostringstream tmp;
  tmp << zplus (e-1U);
  return tmp.str ();
}

size_t 
Geometry1D::cell_at (const double z, double, double) const
{
  for (size_t cell = 1; cell < cell_size (); cell++)
    if (zplus (cell-1U) >= z)
      return cell;
  return cell_size () - 1;
}

double 
Geometry1D::fraction_in_z_interval (const size_t i, 
                                    const double from, const double to) const
{ return fraction_within (zplus (i), zminus (i), to, from); }

double 
Geometry1D::fraction_in_volume (size_t n, const Volume& volume) const
{ return volume.box_fraction (zplus (n), zminus (n)); }

bool 
Geometry1D::contain_x (size_t, double x) const
{ return  left () <= x  && x <= right (); }

bool 
Geometry1D::contain_y (size_t, double y) const
{ return  front () <= y  && y <= back (); }

size_t 
Geometry1D::interval_plus (double z) const
{
  size_t i;
  for (i = 0; i < size_; i++)
    {
      if (zplus (i) <= z)
	return i;
    }
  daisy_notreached ();
}

size_t
Geometry1D::interval_border (double z) const
{
  double best = fabs (z - 0.0);
  
  for (size_t i = 1; i <= size_; i++)
    {
      const double dist = fabs (z - zplus (i-1U));
      if (dist > best)
	return i - 1;
      best = dist;
    }
  return size_;
}

void
Geometry1D::add_soil (std::vector<double>& v, 
                      const double top, const double bottom, 
                      const double left, const double right,
                      const double amount) const
// This function must be fast, as it is called once for each biopore
// row and class.
{
  // Pre-conditions.
  daisy_assert (std::isfinite (bottom));
  daisy_assert (std::isfinite (top));
  if (!(bottom < top))
    {
      std::ostringstream tmp;
      tmp << "top = " << top << " bottom = " << bottom;
      Assertion::message (tmp.str ());
    }
  daisy_assert (bottom < top);
  daisy_assert (left < right);

  // Find col.
  const double cell_right = this->right ();
  const double cell_left = this->left ();
  const double cover_left = std::max (left, cell_left);
  const double cover_right = std::min (right, cell_right);
  const double col_factor = (cover_right - cover_left) 
    / (right - left);
  daisy_assert (col_factor >= 0.0);
  daisy_assert (col_factor <= 1.0);

  // Find rows.
  const size_t rows = cell_size ();
  std::vector<double> row_factor (rows, 0.0);
  size_t row_first = rows;
  size_t row_last = 0;
  for (size_t row = 0; row < rows; row++)
    {
      const size_t cell = row;
      const double cell_top = zminus (cell);
      const double cell_bottom = zplus (cell);

      if (row < row_first)      // There yet?
        {
          if (cell_bottom >= top) // No.
            {
              continue;
            }

          row_first = row;
        }
      const double cover_bottom = std::max (bottom, cell_bottom);
      const double cover_top = std::min (top, cell_top);
      const double cover = (cover_top - cover_bottom) 
        / (cell_top - cell_bottom);
      daisy_assert (cover >= 0.0);
      daisy_assert (cover <= 1.0);
      row_factor[row] = cover;

      if (cell_bottom <= bottom)  // Done?
        {
          row_last = row;
          break;
        }
    }
  
  // Add it.

  const double volume = (bottom - top) * (right - left) * (front () - back ());
  const double col_add = amount * col_factor / volume;
  for (size_t row = row_first; row <= row_last; row++)
    {
      v[row] += col_add * row_factor[row];
    }
}

void 
Geometry1D::fill_xplus (std::vector<double>& result) const
{ 
  result.clear ();
  result.push_back (right ()); 
}

void 
Geometry1D::build_edges ()
{
  // Distance from surface to first cell center.
  edge_length_.push_back (-cell_z (0));

  // Distance between internal cell centers.
  for (int c = 1; c < cell_size (); c++)
    edge_length_.push_back (cell_z (c-1) - cell_z (c));

  // Distance from last cell center to bottom.
  edge_length_.push_back (cell_z (cell_size () - 1) - zplus (cell_size () - 1));

  // Check result.
  daisy_assert (edge_length_.size () == edge_size ());
  for (size_t e = 0; e < edge_size (); e++)
    daisy_assert (edge_length (e) > 0.0);

  // Edge area.
  edge_area_.insert (edge_area_.end (), edge_size (), 1.0);
  daisy_assert (edge_area_.size () == edge_size ());
}

bool 
Geometry1D::check (Treelog&) const
{
  daisy_assert (edge_length_.size () == edge_size ());
  daisy_assert (cell_edges_.size () == cell_pseudo_size ());
  bool ok = true;
  return ok;
}

bool 
Geometry1D::check_x_border (const double, Treelog& err) const
{
  err.warning ("Logging on x-axis on a 1D geometry is meaningless");
  return false;
}

bool 
Geometry1D::check_y_border (const double, Treelog& err) const
{
  err.warning ("Logging on y-axis on a 1D geometry is meaningless");
  return false;
}

static bool 
check_alist (const Metalib&, const Frame&, Treelog&)
{
  bool ok = true;
  return ok;
}

void
Geometry1D::load_syntax (Frame& frame)
{ 
  frame.add_check (check_alist);
  frame.declare ("zplus", "cm", Check::negative (), 
                 Attribute::OptionalConst, Attribute::SoilCells,
                 "Depth of each numeric layer (a negative number).\n\
The end points are listed descending from the surface to the bottom.");
  static VCheck::All zplus_check (VCheck::decreasing (), 
				  VCheck::min_size_1 ());
  frame.set_check ("zplus", zplus_check);
}
  
Geometry1D::Geometry1D ()
  : GeometryVert ()
{ }


Geometry1D::Geometry1D (const Block& al)
  : GeometryVert (al)
{ 
  if (al.check ("zplus"))
    zplus_ = al.number_sequence ("zplus");
}

void
Geometry1D::initialize_zplus (const bool volatile_bottom,
                              const std::vector<double>& fixed,
                              const double max_rooting_depth,
                              const double max_interval,
                              Treelog& msg)
{
  if (zplus_.empty ())
    {
      Treelog::Open nest (msg, "Geometry");
      
      bool warn_about_small_intervals = true;
      double last = 0.0;
      double last_fixed = 0.0;
      for (size_t i = 0; i < fixed.size ();)
	{
	  const double current = fixed[i];

	  // We divide the soil into zones with desired interval sizes.
	  double zone_end;
	  double zone_size;
	  
	  if (last > -5.0)
	    {
	      zone_end = -5.0;
	      zone_size = 2.5;
	    }
	  else if (volatile_bottom)
	    {
	      zone_end = current;
	      zone_size = 5.0;
	    }
	  else if (last > -10.0)
	    {
	      zone_end = -10.0;
	      zone_size = 5.0;
	    }
	  else if (last > max_rooting_depth - 50.0)
	    {
	      zone_end = max_rooting_depth - 50.0;
	      zone_size = 10.0;
	    }
	  else
	    {
	      zone_end = current;
	      zone_size = 20.0;
	    }

          zone_size = std::min (zone_size, (last_fixed - current) / 3.0);

          if (warn_about_small_intervals && zone_size < 0.99)
            {
              warn_about_small_intervals = false;
              std::ostringstream tmp;
              tmp << "\
Can't automatically make discretizations less than 1 [cm], needed at " 
                     << last << " [cm].\nPlease set zplus manually.";
              msg.warning (tmp.str ());
            }

	  // Dispersivity limit.
	  if (zone_size > max_interval)
	    zone_size = max_interval;

	  if (current < zone_end - zone_size + 1e-8)
	    // The zone ends before the next fixed interval limit.
	    while (last > zone_end + 1e-8)
	      // Just add intervals to the end of the zone.
	      {
		last -= zone_size;
		zplus_.push_back (last);
	      }
	  else
	    // The next fixed interval limit is before the end of the zone.
	    {
	      // Find approximate number of intervals until fixed limit.
	      const int intervals 
                = double2int ((last - current) / zone_size + 0.499);
	      if (intervals > 1)
		{
		  // Add interior intervals.
		  const double step 
		    = (last - current) / int2double (intervals);
		  const double first = last;
		  for (int j = 1; j < intervals; j++)
		    {
		      const double next = first - step * j;
                      if (!approximate (double2int (next), last))
                        {
                          last = double2int (next);
                          zplus_.push_back (last);
                        }
		    }
		}
	      // Add fixed limit.
	      if (!approximate (last, current))
		{
		  last = current;
		  zplus_.push_back (current);
		}
	      // Next fixed limit.
              last_fixed = current;
	      i++;
	    }
	}

      // Debug messages.
      std::ostringstream tmp;
      tmp << "(zplus";
      for (size_t i = 0; i < zplus_.size (); i++)
	tmp << " " << zplus_[i];
      tmp << "); " << zplus_.size () << " cells.";
      msg.debug (tmp.str ());
      // Check that zplus is strictly decreasing.
      last = 0.0;
      for (size_t i = 0; i < zplus_.size (); i++)
	{
	  daisy_assert (zplus_[i] < last);
	  last = zplus_[i];
	}
    }

  // Test all members of 'fixed' are in 'zplus'
  size_t z = 0;
  for (size_t f = 0; f < fixed.size (); f++)
    {
      while (z < zplus_.size () && zplus_[z] > fixed[f] + 0.00001)
	z++;
      if (z == zplus_.size () || !approximate (zplus_[z], fixed[f]))
	{
	  std::ostringstream tmp;
	  tmp << "No zplus near " << fixed[f] << " [cm]";
	  msg.warning (tmp.str ());
	}
    }

  // Update z and dz from zplus.
  initialize_intervals (zplus_, z_, dz_);

  // Initialize base!
  size_ = zplus_.size ();

  // Geometry structures.
  build_edges ();
  build_common ();
}

Geometry1D::~Geometry1D ()
{ }

static DeclareSubmodel
geometry1d_submodel (Geometry1D::load_syntax, "Geometry1D", "\
A one dimensional discretization of the soil.");

// geometry1d.C ends here.

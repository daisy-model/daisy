// plf.h
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

// 
// A PLF is a piecewise liniear function defined by a number of points.  
//
// The points must be added by increasing x value, and the function is
// defined by drawing a straight line from each added point to the next.

#ifndef PLF_H
#define PLF_H

#include <vector>

class Log;

class PLF
{
  // Content.
  struct Implementation;
  friend struct Implementation;
  Implementation& impl;
public:
  // Use.
  // [Forall x < plf.x(0)](plf(x) = plf.y(0))
  // [Forall x > plf.x(plf.size() - 1)](plf(x) = plf.y(plf.size() - 1))
  double operator ()(double x) const;
  PLF inverse () const;
  PLF inverse_safe () const;
  double first_interesting () const;
  double last_interesting () const;
  double min () const;
  double max () const;
  double max_at () const; // plf(plt.max_at()) == plf.max()
  double integrate (double from, double to) const; // Area under curve connecting x=from and x=to
  PLF integrate_stupidly () const;
  PLF integrate_stupidly_2 (const double C = 0.0) const;
  void offset (double offset);	// Add 'offset' to all y values.

  // Utilities.
  static double find (const std::vector<double>& x, 
		      const std::vector<double>& y,
		      double value);

  // Simulation.
  void output (Log&) const;

  // Print
  unsigned int size () const;
  double x (unsigned int i) const;
  double y (unsigned int i) const;

  // Compare.
  bool operator == (const PLF&) const;

  // Create and Destroy.
  static const PLF& empty ();	// An empty PLF.
  static const PLF& always_1 (); // A PLF that is always 1.
  static const PLF& always_0 (); // A PLF that is always 0.
  void clear ();
  void add (double, double); // Add a point to the plf
  void operator += (const PLF&);
  void operator = (const PLF&);
  PLF (const PLF&);
  // Move operator gives memory corruption with ForcedLAI...
  //  PLF (PLF&&) = default;
  PLF ();
  ~PLF ();
};

bool operator< (const PLF& a, const PLF& b);
bool operator<= (const PLF& a, const PLF& b);

#endif // PLF_H

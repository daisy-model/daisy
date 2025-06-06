// xysource_combine.C -- Combine data sources for gnuplot interface 
// 
// Copyright 2005 Per Abrahamsen and KVL.
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
#include "gnuplot/xysource.h"
#include "object_model/block_model.h"
#include "gnuplot/gnuplot_utils.h"
#include "object_model/parameter_types/number.h"
#include "util/scope_sources.h"
#include "gnuplot/source.h"
#include "util/assertion.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"

struct XYSourceCombine : public XYSource
{
  // Content.
  ScopeSources scope;
  const std::unique_ptr<Number> x_expr;
  const std::unique_ptr<Number> y_expr;
  const symbol title_;
  symbol x_dimension_;
  symbol y_dimension_;
  symbol with_;
  const int style_;
  std::vector<double> xs;
  std::vector<double> ys;

  // Interface.
public:
  symbol with () const
  { return with_; }
  int style () const 
  { return style_; }
  const std::vector<double>& x () const
  { return xs; }
  const std::vector<double>& y () const
  { return ys; }
  symbol title () const
  { return title_; }
  symbol x_dimension () const 
  { return x_dimension_; }
  symbol y_dimension () const 
  { return y_dimension_; }

  // Read. 
public:
 bool load (const Units&, Treelog& msg);

  // Create and Destroy.
public:
  explicit XYSourceCombine (const BlockModel& al);
  ~XYSourceCombine ()
  { }
};

bool
XYSourceCombine::load (const Units& units, Treelog& msg)
{
  // Propagate.
  scope.load (msg);

  // Scope
  {
    bool ok = true;
    if (!x_expr->initialize (units, scope, msg) 
        || !x_expr->check (units, scope, msg))
      ok = false;
    x_expr->tick (units, scope, msg);
    if (!y_expr->initialize (units, scope, msg) 
        || !y_expr->check (units, scope, msg))
      ok = false;
    y_expr->tick (units, scope, msg);
    if (!ok)
      return false;
  }

  // Extract.
  x_dimension_ = x_expr->dimension (scope);
  y_dimension_ = y_expr->dimension (scope);
  if (with_ == "")
    with_ = scope.with ();
  if (with_ == "errorbars")
    with_ = "points";

  // Read data.
  for (scope.first (); !scope.done (); scope.next ())
    {
      if (!x_expr->missing (scope) && !y_expr->missing (scope))
        {
          xs.push_back (x_expr->value (scope));
          ys.push_back (y_expr->value (scope));
        }
    }
  daisy_assert (xs.size () == ys.size ());

  // Done.
  return true;
}

XYSourceCombine::XYSourceCombine (const BlockModel& al)
  : XYSource (al),
    scope (Librarian::build_vector<Source> (al, "source")),
    x_expr (Librarian::build_item<Number> (al, "x")),
    y_expr (Librarian::build_item<Number> (al, "y")),
    title_ (al.name ("title", y_expr->title () + " vs " + x_expr->title ())),
    x_dimension_ ("UNINITIALIZED"),
    y_dimension_ ("UNINITIALIZED"),
    with_ (al.name ("with", "")),
    style_ (al.integer ("style", -1))
{ }

static struct XYSourceCombineSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new XYSourceCombine (al); }

  XYSourceCombineSyntax ()
    : DeclareModel (XYSource::component, "combine", "\
Combine data from multiple time series with a single expression.\n\
Data from times series are matched by date.")
  { }
  void load_frame (Frame& frame) const
  { 
    GnuplotUtil::load_style (frame, "\
By default, let the first source decide.", "\
By default a combination of the x and y objects.");
    frame.declare_object ("source", Source::component, 
                       Attribute::State, Attribute::Variable, "\
List of sources for data.\n\
The style information for the sources is ignored, but the dates, title\n\
and value is used as specified by 'expr' to calculate the combined\n\
date and value pairs.");
    frame.declare_object ("x", Number::component, 
                       Attribute::Const, Attribute::Singleton, "\
Expression for calculating the x value for this source for each row.\n\
A row is any date found in any of the member of 'source'.  The\n\
expression may refer to the value of each source by its title.");
    frame.declare_object ("y", Number::component, 
                       Attribute::Const, Attribute::Singleton, "\
Expression for calculating the y value for this source for each row.\n\
A row is any date found in any of the member of 'source'.  The\n\
expression may refer to the value of each source by its title.");
  }
} XYSourceCombine_syntax;

// xysource_combine.C ends here.

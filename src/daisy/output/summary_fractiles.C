// summary_fractiles.C --- Show fractiles of data.
// 
// Copyright 2011 KU.
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

#include "daisy/output/summary.h"
#include "daisy/output/destination.h"
#include "daisy/output/select.h"
#include "object_model/block_submodel.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "daisy/daisy_time.h"
#include "util/memutils.h"
#include "util/mathlib.h"
#include "object_model/vcheck.h"
#include <numeric>
#include <sstream>
#include <map>

struct SummaryFractiles : public Summary
{
  struct Data : public Destination
  {
    const symbol tag;
    symbol dimension;
    mutable std::vector<double> data;
    
    // Destination.
    void missing ()
    { }
	NORETURN void add (const std::vector<double>& value)
    { daisy_notreached (); }
    void add (const double value)
    { data.push_back (value); }
	NORETURN void add (const symbol value)
    { daisy_notreached (); }

    explicit Data (const symbol key)
      : tag (key),
        dimension (Attribute::Unknown ())
    { }
  };
  std::vector<Data> data;
  const std::vector<double> fractiles;
  const symbol first;

  
  // Create and Destroy.
  void clear ();
  void initialize (std::vector<Select*>&, Treelog&);
  bool check (Treelog& msg) const;
  static std::vector<Data> find_data (const BlockModel& al);
  explicit SummaryFractiles (const BlockModel& al)
    : Summary (al),
      data (find_data (al)),
      fractiles (al.number_sequence ("fractiles")),
      first (al.name ("first", Attribute::None ()))
  { }
  static double find_fractile (const double f, const std::vector<double>& data);
  static double find_average (const std::vector<double>& data);
  void summarize (Treelog&) const;
};

void
SummaryFractiles::clear ()
{ }

void
SummaryFractiles::initialize (std::vector<Select*>& select, Treelog& msg)
{
  std::map<symbol, Data*> all;
  for (auto& datum : data)
    all[datum.tag] = &datum;

  std::set<symbol> found;
  for (auto& s : select)
    {
      const symbol tag = s->tag ();
      const auto i = all.find (tag);
      if (i == all.end ())
        continue;
      if (found.find (tag) != found.end ())
        msg.warning ("Duplicate tag '" + tag + "'");
      else
        found.insert (tag);
      i->second->dimension = s->dimension ();
      s->add_dest (i->second);
      all.erase (i);
    }
  if (all.size () == 0)
    return;
  for (auto& i : all)
    msg.warning ("tag: '" + i.first + "' not found");
}

bool 
SummaryFractiles::check (Treelog& msg) const
{
  TREELOG_MODEL (msg);
  bool ok = true;
  return ok;
}

std::vector<SummaryFractiles::Data>
SummaryFractiles::find_data (const BlockModel& al)
{
  std::vector<Data> data;
  std::vector<symbol> tags = al.name_sequence ("tags");
  for (auto tag : tags)
    data.push_back (Data (tag));
  return data;
}

double
SummaryFractiles::find_fractile (const double f,
                                 const std::vector<double>& data)
{
  daisy_assert (f >= 0.0 && f <= 1.0);
  daisy_assert (data.size () > 0);
  const double last = data.size () - 1.0;
  const size_t i = (f < 1.0) ? double2int(f * last) : (data.size () - 1);
  daisy_assert (i < data.size ());
  return data[i];
}

double
SummaryFractiles::find_average (const std::vector<double>& data)
{
  const double n = data.size ();
  if (n < 0.5)
    return NAN;
  const double sum = std::accumulate (data.begin (), data.end (), 0.0);
  return sum / n;
}

void 
SummaryFractiles::summarize (Treelog& msg) const
{
  TREELOG_MODEL (msg);
  std::ostringstream tmp;
  if (first != Attribute::None ())
    tmp << first << "\t";
  tmp << "Name\tDim\tSample\tAvg";
  for (auto fractile : fractiles)
    tmp << "\tF" << (fractile * 100.0) << " %";
  for (auto& datum : data)
    {
      std::sort (datum.data.begin (), datum.data.end ());
      const size_t n = datum.data.size ();
      tmp << "\n";
      if (first != Attribute::None ())
        tmp << first << "\t";
      tmp << datum.tag << "\t" << datum.dimension << "\t" << n
          << "\t" << find_average (datum.data);
      
      for (size_t i = 0; i < fractiles.size (); i++)
        if (n > 0)
          tmp << "\t" << find_fractile (fractiles[i], datum.data);
        else
          tmp << "\tn/a";
    }

  msg.message (tmp.str ());
}

static struct SummaryFractilesSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new SummaryFractiles (al); }
  SummaryFractilesSyntax ()
    : DeclareModel (Summary::component, "fractiles", "\
Show fractiles for specified tags.")
  { }
  void load_frame (Frame& frame) const
  { 
    frame.declare_string ("tags", Attribute::Const, Attribute::Variable, "\
List of tags to summarize.");
    frame.declare_fraction ("fractiles",
                            Attribute::Const, Attribute::Variable, "\
List of fractiles to summarize.");
    frame.declare_string ("first", Attribute::OptionalConst, "\
If set, put this in the first column of the summary.");
  }
} SummaryFractiles_syntax;

// summary_fractiles.C ends here.

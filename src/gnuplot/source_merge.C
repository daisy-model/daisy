// source_merge.C -- Merge two timeseries.
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
#include "gnuplot/source.h"
#include "object_model/block_model.h"
#include "gnuplot/gnuplot_utils.h"
#include "object_model/units.h"
#include "object_model/vcheck.h"
#include "util/mathlib.h"
#include "util/memutils.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"
#include <numeric>
#include <sstream>

struct SourceMerge : public Source
{
  // Content.
  const Units& units;
  const std::vector<Source*> source;
  const symbol title_;
  symbol dimension_;
  symbol with_;
  const int style_;
  const bool accumulate_;
  void add_entry (const Time& time, const std::vector<double>& vals);
  std::vector<Time> times;
  std::vector<double> values;
  std::vector<double> ebars;

  // Interface.
public:
  symbol with () const
  { return with_; }
  int style () const 
  { return style_; }
  bool accumulate () const
  { return accumulate_; }
  const std::vector<Time>& time () const
  { return times; }
  const std::vector<double>& value () const
  { return values; }
  const std::vector<double>& ebar () const
  { return ebars; }
  symbol title () const
  { return title_; }
  symbol dimension () const 
  { return dimension_; }

  // Read. 
public:
 bool load (Treelog& msg);

  // Create and Destroy.
public:
  explicit SourceMerge (const BlockModel& al);
  ~SourceMerge ()
  { sequence_delete (source.begin (), source.end ()); }
};

void
SourceMerge::add_entry (const Time& time, const std::vector<double>& vals)
{
  if (vals.size () > 1 && with_ == "")
    with_ = "errorbars";
  const double total = std::accumulate (vals.begin (), vals.end (), 0.0);
  const double N = vals.size ();
  const double mean = total / N;
  double variance = 0;
  for (size_t i = 0; i < vals.size (); i++)
    { 
      const double diff = vals[i] - mean;
      variance += diff * diff;
    }
  variance /= N;
  const double std_deviation = sqrt (variance);
  times.push_back (time);
  values.push_back (mean);
  ebars.push_back (std_deviation);
  daisy_assert (times.size () == values.size ());
  daisy_assert (values.size () == ebars.size ());
}

bool
SourceMerge::load (Treelog& msg)
{
  // Propagate.
  bool ok = true;
  for (size_t i = 0; i < source.size (); i++)
    {
      // Load.
      std::ostringstream tmp;
      tmp << "[" << i << "] " << source[i]->title ();
      Treelog::Open nest (msg, tmp.str ());
      if (!source[i]->load (msg))
        {
          ok = false;
          continue;
        }
      
      // Set or check dimension.
      if (dimension_ == Attribute::Unknown ())
        dimension_ = source[i]->dimension ();
      else if (!units.can_convert (source[i]->dimension (), dimension_))
        {
          msg.error ("Cannot convert dimension from [" 
                     + source[i]->dimension () + "] to ["
                     + dimension_ + "]");
          ok = false;
        }
    }
  if (!ok)
    return false;
  
  // Keep track of each source.
  std::vector<size_t> index (source.size (), 0);

  // Merge them.
  while (true)
    {
      
      // Find first time.
      Time first_time (9999, 12, 31, 23);
      for (size_t i = 0; i < source.size (); i++)
        {
          const size_t cur = index[i];
          const std::vector<Time> times = source[i]->time ();
          
          if (times.size () == cur)
            // No more data.
            continue;

          const Time time = times[cur];
          if (time < first_time)
            // First time
            first_time = time;
        }

      // Find vals;
      std::vector<double> vals;
      for (size_t i = 0; i < source.size (); i++)
        {
          const size_t cur = index[i];
          const std::vector<Time> times = source[i]->time ();

          if (times.size () == cur)
            // No more data.
            continue;

          const Time time = times[cur];
          if (time == first_time)
            {
              // Add this element.
              vals.push_back (source[i]->value ()[cur]);
              index[i]++;
            }
          else
            daisy_assert (time > first_time);
        }
      
      // Done yet?
      if (vals.size () == 0)
        break;
      
      // Add it.
      add_entry (first_time, vals);
    }
          
  // Inherit style.
  if (with_ == "")
    with_ = source[0]->with ();

  // Done.
  return true;
}

SourceMerge::SourceMerge (const BlockModel& al)
  : Source (al),
    units (al.units ()),
    source (Librarian::build_vector<Source> (al, "source")),
    title_ (al.name ("title")),
    dimension_ (al.name ("dimension", Attribute::Unknown ())),
    with_ (al.name ("with", "")),
    style_ (al.integer ("style", -1)),
    accumulate_ (al.flag ("accumulate"))
{ }

static struct SourceMergeSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new SourceMerge (al); }

  SourceMergeSyntax ()
    : DeclareModel (Source::component, "merge", 
	       "Merge multiple timeseries into one.\n\
Any errorbars on the original timeseries are ignored, but the merged\n\
timeseries may have errorbars if there are multiple values for the\n\
same time.")
  { }
  void load_frame (Frame& frame) const
  { 
    GnuplotUtil::load_style (frame, "\
By default, let the first source decide.", "");
    frame.declare_boolean ("accumulate", Attribute::Const, "\
Accumulate values.");
    frame.set ("accumulate", false);
    frame.declare_object ("source", Source::component, 
                       Attribute::State, Attribute::Variable, "\
List of timeseries to merge.");
    frame.set_check ("source", VCheck::min_size_1 ());
    frame.declare_string ("dimension", Attribute::OptionalConst, "\
Dimension of data to plot.\n\
By default use the first source with a known dimension.");

    
  }
} SourceMerge_syntax;

// source_merge.C ends here.

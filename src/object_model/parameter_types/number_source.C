// number_source.C -- Extract a single number from a time series.
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

#include "object_model/parameter_types/number.h"
#include "object_model/block_model.h"
#include "gnuplot/source.h"
#include "util/assertion.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"
#include <sstream>
#include <memory>

symbol
NumberSource::title () const
{
  daisy_assert (state_ != uninitialized);
  return source_->title ();
}

void
NumberSource::tick (const Units&, const Scope&, Treelog&)
{ }

bool
NumberSource::missing (const Scope&) const
{
  daisy_assert (state_ != uninitialized);
  return state_ != has_value;
}

double
NumberSource::value (const Scope&) const
{
  daisy_assert (state_ == has_value);
  return val_;
}

symbol
NumberSource::dimension (const Scope&) const
{
  daisy_assert (state_ != uninitialized);
  return symbol (source_->dimension ());
}

bool
NumberSource::initialize (const Units&, const Scope&, Treelog& msg)
{
  TREELOG_MODEL (msg);
  msg.touch ();
  daisy_assert (state_ == uninitialized);
  daisy_assert (source_.get ());
  if (!source_->load (msg))
    state_ = error;
  else
    initialize_derived (msg);
  return state_ != error;
}

bool
NumberSource::check (const Units&, const Scope&, Treelog&) const
{
  daisy_assert (state_ != uninitialized);
  return state_ != error;
}

NumberSource::NumberSource (symbol objid,
                            std::unique_ptr<Source> source,
                            std::unique_ptr<const Time> begin,
                            std::unique_ptr<const Time> end)
  : Number (objid),
    source_ (std::move (source)),
    begin_ (std::move (begin)),
    end_ (std::move (end)),
    state_ (uninitialized),
    val_ (-42.42e42)
{ }

NumberSource::NumberSource (const BlockModel& al)
  : Number (al),
    source_ (Librarian::build_item<Source> (al, "source")),
    begin_ (al.check ("begin") ? new Time (al.submodel ("begin")) : NULL),
    end_ (al.check ("end") ? new Time (al.submodel ("end")) : NULL),
    state_ (uninitialized),
    val_ (-42.42e42)
{ }

NumberSource::~NumberSource ()
{ }

static struct NumberSourceSyntax : public DeclareBase
{
  NumberSourceSyntax ()
    : DeclareBase (Number::component, "source", 
                   "Extract information from a time series.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_object ("source", Source::component, "\
The time series we want to extract a number from.");
    frame.declare_submodule ("begin", Attribute::OptionalConst,
			  "Ignore values before or at this date.", 
                          Time::load_syntax);
    frame.declare_submodule ("end", Attribute::OptionalConst,
			  "Ignore values after this date.", Time::load_syntax);
  }
} NumberSource_syntax;


void
NumberSourceUnique::initialize_derived (Treelog& msg)
{
  const std::vector<Time>& time = source_->time ();
  const size_t size = time.size ();
  int count = 0;
  for (size_t i = 0; i < size; i++)
    if ((!begin_.get () || time[i] > *begin_)
        && (!end_.get () || time[i] <= *end_))
      {
        val_ = source_->value ()[i];
        count++;
      }

  if (count == 1U)
    state_ = has_value;
  else if (count == 0U)
    {
      msg.warning ("Got zero elements, expected one");
      state_ = is_missing;
    }
  else
    {
      std::ostringstream tmp;
      tmp << "Got " << count << " elements, expected 1";
      msg.error (tmp.str ());
      state_ = error;
    }
}

NumberSourceUnique::NumberSourceUnique (std::unique_ptr<Source> source,
                                        std::unique_ptr<const Time> begin,
                                        std::unique_ptr<const Time> end)
  : NumberSource ("source_unique", std::move (source), std::move (begin), std::move (end))
{ }

NumberSourceUnique::NumberSourceUnique (const BlockModel& al)
  : NumberSource (al)
{ }

static struct NumberSourceUniqueSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberSourceUnique (al); }
  NumberSourceUniqueSyntax ()
    : DeclareModel (Number::component, "source_unique", "source",
	       "Find unique number in time series.")
  { }
  void load_frame (Frame&) const
  { }
} NumberSourceUnique_syntax;

void
NumberSourceAverage::initialize_derived (Treelog& msg)
{
  const std::vector<Time>& time = source_->time ();
  const size_t size = time.size ();
  int count = 0;
  val_ = 0.0;
  for (size_t i = 0; i < size; i++)
    if ((!begin_.get () || time[i] > *begin_)
        && (!end_.get () || time[i] <= *end_))
      {
        val_ += source_->value ()[i];
        count++;
      }

  if (count == 0U)
    {
      msg.warning ("Can't take average of zero elements");
      state_ = is_missing;
    }
  else
    {
      val_ /= (count + 0.0);
      state_ = has_value;
    }
}

NumberSourceAverage::NumberSourceAverage (std::unique_ptr<Source> source,
                                          std::unique_ptr<const Time> begin,
                                          std::unique_ptr<const Time> end)
  : NumberSource ("source_average", std::move (source), std::move (begin), std::move (end))
{ }

NumberSourceAverage::NumberSourceAverage (const BlockModel& al)
  : NumberSource (al)
{ }

static struct NumberSourceAverageSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberSourceAverage (al); }
  NumberSourceAverageSyntax ()
    : DeclareModel (Number::component, "source_average", "source",
                    "Find average number in time series.")
  { }
  void load_frame (Frame&) const
  { }
} NumberSourceAverage_syntax;

void
NumberSourceSum::initialize_derived (Treelog&)
{
  const std::vector<Time>& time = source_->time ();
  const size_t size = time.size ();
  val_ = 0.0;
  state_ = has_value;
  for (size_t i = 0; i < size; i++)
    if ((!begin_.get () || time[i] > *begin_)
        && (!end_.get () || time[i] <= *end_))
      val_ += source_->value ()[i];
}

NumberSourceSum::NumberSourceSum (std::unique_ptr<Source> source,
                                  std::unique_ptr<const Time> begin,
                                  std::unique_ptr<const Time> end)
  : NumberSource ("source_sum", std::move (source), std::move (begin), std::move (end))
{ }

NumberSourceSum::NumberSourceSum (const BlockModel& al)
  : NumberSource (al)
{ }

static struct NumberSourceSumSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberSourceSum (al); }
  NumberSourceSumSyntax ()
    : DeclareModel (Number::component, "source_sum", "source", 
	       "Calculate the sum of the values in a time series.")
  { }
  void load_frame (Frame&) const
  { }
} NumberSourceSum_syntax;

void
NumberSourceIncrease::initialize_derived (Treelog& msg)
{
  const std::vector<Time>& time = source_->time ();
  const std::vector<double>& value = source_->value ();
  const size_t size = time.size ();
  daisy_assert (value.size () == size);
  state_ = has_value;
  if (size < 2)
    {
      msg.warning ("Need two elements to make a difference");
      val_ = 0;
      return;
    }
  double first = value[0];
  double last = value[0];
  for (size_t i = 1; i < size; i++)
    {
      if (begin_.get () && time[i] < *begin_)
        first = value[i];
      if (!end_.get () || time[i] <= *end_)
        last = value[i];
    }
  val_ = last - first;
}

NumberSourceIncrease::NumberSourceIncrease (std::unique_ptr<Source> source,
                                            std::unique_ptr<const Time> begin,
                                            std::unique_ptr<const Time> end)
  : NumberSource ("source_increase", std::move (source), std::move (begin), std::move (end))
{ }

NumberSourceIncrease::NumberSourceIncrease (const BlockModel& al)
  : NumberSource (al)
{ }

static struct NumberSourceIncreaseSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberSourceIncrease (al); }
  NumberSourceIncreaseSyntax ()
    : DeclareModel (Number::component, "source_increase", "source",
	       "Find increase in value during time series.")
  { }
  void load_frame (Frame&) const
  { }
} NumberSourceIncrease_syntax;

// number_source.C ends here

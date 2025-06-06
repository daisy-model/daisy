// wsource_base.C -- Weather data that never changes.
// 
// Copyright 2010 KU
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

#include "daisy/upper_boundary/weather/wsource_base.h"
#include "daisy/upper_boundary/weather/weatherdata.h"
#include "daisy/daisy_time.h"
#include "util/assertion.h"
#include "util/mathlib.h"
#include "object_model/librarian.h"
#include "object_model/frame_submodel.h"
#include "object_model/block_model.h"
#include "object_model/submodeler.h"
#include <map>

#include <sstream>

struct WSourceBase::Implementation : public FrameSubmodelValue
{
  const std::unique_ptr<Time> begin;
  const std::unique_ptr<Time> end;

  Implementation (const BlockModel& al)
    : FrameSubmodelValue (*Librarian::submodel_frame 
                          /**/ (Weatherdata::load_syntax), 
                          Frame::parent_link),
      begin (al.check ("begin") ? ::submodel<Time> (al, "begin") : NULL),
      end (al.check ("end") ? ::submodel<Time> (al, "end") : NULL)
  { 
    std::set<symbol> all;
    entries (all);
    for (std::set<symbol>::iterator i = all.begin ();
         i != all.end ();
         i++)
      {
        const symbol key = *i;
        if (!al.check (key))
          continue;

        Attribute::type type = al.lookup (key);
        if (type == Attribute::Error)
          {
            al.msg ().message ("'" + key + "': not found");
            continue;
          }
        
        if (al.type_size (key) == Attribute::Singleton)
          switch (type)
            {
            case Attribute::Number:
              set (key, al.number (key));
              break;
            case Attribute::String:
              set (key, al.name (key));
              break;
            default:
              break;
            }
        else
          switch (type)
            {
            case Attribute::Number:
              set (key, al.number_sequence (key));
              break;
            case Attribute::String:
              set (key, al.name_sequence (key));
              break;
            default:
              break;
            }
      }
  }
};

void 
WSourceBase::entries (std::set<symbol>& e) const
{ impl->entries (e); }

Attribute::type 
WSourceBase::lookup (const symbol key) const
{ return impl->lookup (key); }

symbol 
WSourceBase::dimension (const symbol key) const
{ return impl->dimension (key); }

symbol 
WSourceBase::description (const symbol key) const
{ return impl->description (key); }
  
int 
WSourceBase::type_size (const symbol key) const
{ return impl->type_size (key); }

bool 
WSourceBase::check (const symbol key) const
{ return impl->check (key); }

double 
WSourceBase::number (const symbol key) const
{ return impl->number (key); }

symbol
WSourceBase::name (const symbol key) const
{ return impl->name (key); }

int 
WSourceBase::value_size (const symbol key) const
{ return impl->value_size (key); }

const Time& 
WSourceBase::data_begin () const
{ 
  if (impl->begin.get ())
    return *impl->begin;
  return Time::null (); 
}

const Time& 
WSourceBase::data_end () const
{ 
  if (impl->end.get ())
    return *impl->end;
  return Time::null (); 
}

const Time& 
WSourceBase::begin () const
{ 
  if (impl->begin.get ())
    return *impl->begin;
  static const Time time (1, 1, 1, 0); 
  return time; 
}

const Time& 
WSourceBase::end () const
{ 
  if (impl->end.get ())
    return *impl->end;
  static const Time time (9999, 12, 31, 23); 
  return time; 
}

const std::vector<double>& 
WSourceBase::number_sequence (const symbol key) const
{ return impl->number_sequence (key); }

const std::vector<double>& 
WSourceBase::end_number_sequence (const symbol key) const
{ return number_sequence (key); }

double 
WSourceBase::meta_timestep (const symbol) const
{ return timestep (); }

bool
WSourceBase::meta_check (const symbol, const symbol meta) const
{ return check (meta); }

double 
WSourceBase::meta_number (const symbol, const symbol meta) const
{ return number (meta); }

symbol 
WSourceBase::meta_name (const symbol, const symbol meta) const
{ return name (meta); }

bool
WSourceBase::meta_end_check (const symbol, const symbol meta) const
{ return end_check (meta); }

double 
WSourceBase::meta_end_number (const symbol, const symbol meta) const
{ return end_number (meta); }

symbol 
WSourceBase::meta_end_name (const symbol, const symbol meta) const
{ return end_name (meta); }

WSourceBase::WSourceBase (const BlockModel& al)
  : WSourceWeather (al),
    impl (new Implementation (al))
{ }

WSourceBase::~WSourceBase ()
{ }

static struct WSourceBaseSyntax : public DeclareBase
{
  WSourceBaseSyntax ()
    : DeclareBase (WSource::component, "base", "weather",
                   "Weather that does not change during the simulation.")
  { }
  void load_frame (Frame& frame) const
  {
    Weatherdata::load_syntax (frame); 
    daisy_assert (frame.lookup ("GlobRad") == Attribute::Number);
    daisy_assert (frame.type_size ("GlobRad") == Attribute::Singleton);
    frame.declare_submodule ("begin", Attribute::OptionalConst,
                             "Only use data after this date.", 
                             Time::load_syntax);
    frame.declare_submodule ("end", Attribute::OptionalConst,
                             "Only use data before this date.",
                             Time::load_syntax);
  }
} WSourceBase_syntax;

// wsource_base.C ends here.


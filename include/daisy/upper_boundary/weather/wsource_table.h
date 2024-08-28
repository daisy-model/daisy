// wsource_table.h -- Weather data read from table.
// 
// Copyright 2011 KU
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

#ifndef WSOURCE_TABLE_H
#define WSOURCE_TABLE_H

#include "daisy/upper_boundary/weather/wsource_base.h"
#include "daisy/daisy_time.h"
#include "util/lexer_table.h"
#include "object_model/frame_submodel.h"
#include <map>

class Units;

class WSourceTable : public WSourceBase
{
  typedef WSourceBase super;
  const Units& units;
protected:
  LexerTable lex;
  bool ok;
private:
  FrameSubmodelValue keywords;

  std::map<symbol, size_t> columns;
  std::map<symbol, double> values;
  std::map<symbol, double> next_values;
  Time my_data_begin;
  Time my_data_end;
  Time timestep_begin;
  Time timestep_end;
  double timestep_hours;

  // Monthly modifications.
  double lookup_month (const Time&, const symbol, double default_value) const;
  double lookup_month (const Time&, const std::vector<double>&) const;
  double precip_correct (const Time&) const;
  double globrad_scale (const Time&) const;
  double temp_offset (const Time&) const;

  // Scope.
  bool check (const symbol key) const;
  double raw_number (const symbol key) const;
  double number (const symbol key) const;
  symbol name (const symbol key) const;

  // WSource.
public:
  symbol title () const;
protected:
  const Time& data_begin () const;
  const Time& data_end () const;
private:
  const Time& begin () const;
  const Time& end () const;
  double timestep () const;
  bool end_check (const symbol key) const;
  double raw_end_number (const symbol key) const;
  double end_number (const symbol key) const;
  symbol end_name (const symbol key) const;
  void read_line ();
  void source_tick (Treelog& msg);
  bool done () const
  { return !ok; }

  void source_initialize (Treelog& msg);
  void skip_ahead (const Time&, Treelog&);
protected:
  bool source_check (Treelog&) const;
  void rewind (const Time&, Treelog&);
public:
  WSourceTable (const BlockModel& al);
  ~WSourceTable ();
};

#endif // WSOURCE_TABLE_H

// depth.h --- Depth as a function of time.
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


#ifndef DEPTH_H
#define DEPTH_H

#include "daisy/daisy_time.h"
#include "util/lexer_table.h"
#include "object_model/model.h"
#include "object_model/parameter_types/number.h"
#include "object_model/plf.h"
#include "object_model/symbol.h"

class Treelog;
class BlockModel;
class LexerData;
class Scope;

class Depth : public Model
{
  // Content.
public:
  const symbol name;
  static const char *const component;
  symbol library_id () const;

  // Simulation.
public:
  virtual void tick (const Time&, const Scope&, Treelog&) = 0;
  virtual double operator()() const = 0;

  // Create and Destroy.
public:
  virtual void initialize (const Time&, const Scope&, Treelog&) = 0;
  virtual bool check (const Scope&, Treelog&) const = 0;
  static Depth* create (double height);
protected:
  Depth (symbol name);
  Depth (const BlockModel& al);
public:
  ~Depth ();
};

struct DepthConst : public Depth
{
  const double value;

  void tick (const Time&, const Scope&, Treelog&);
  double operator()() const;
  void initialize (const Time&, const Scope&, Treelog&);
  virtual bool check (const Scope&, Treelog&) const;
  DepthConst (const BlockModel& al);
  DepthConst (const double height);
  ~DepthConst ();
};

struct DepthExtern : public Depth
{
  // Content.
  const Units& units;
  const std::unique_ptr<Number> expr;
  double value;

  void tick (const Time&, const Scope& scope, Treelog& msg);
  double operator()() const;
  void initialize (const Time&, const Scope& scope, Treelog& msg);
  virtual bool check (const Scope& scope, Treelog& msg) const;
  DepthExtern (symbol name, const Units& units, Number* expr, double value);
  DepthExtern (const BlockModel& al);
  ~DepthExtern ();
};

struct DepthPLF : public Depth
{
  const Time start;
  PLF value;
  double current_value;

  void  tick (const Time& time, const Scope&, Treelog&);
  double operator()() const;
  void initialize (const Time& time, const Scope& scope, Treelog& msg);
  virtual bool check (const Scope&, Treelog&) const;
  static PLF convert_to_plf (const std::vector<boost::shared_ptr<const FrameSubmodel>/**/>& table);
  DepthPLF (const BlockModel& al);
  ~DepthPLF ();
};

struct DepthSeason : public Depth
{
  const PLF season;
  double current_value;

  void  tick (const Time& time, const Scope&, Treelog&);
  double operator()() const;
  void initialize (const Time& time, const Scope& scope, Treelog& msg);
  virtual bool check (const Scope&, Treelog&) const;
  DepthSeason (const BlockModel& al);
  ~DepthSeason ();
};

namespace State
{
  enum type { uninitialized, ok, error = -1 };
}

struct DepthFile : public Depth
{
  Path& path;
  const symbol file;
  State::type state;
  Time start;
  PLF value;
  double current_value;

  void tick (const Time& time, const Scope&, Treelog&);
  double operator()() const;
  bool read_date (LexerData& lex, Time& time);
  void initialize (const Time& time, const Scope& scope, Treelog& msg);
  virtual bool check (const Scope&, Treelog&) const;
  DepthFile (const BlockModel& al);
  ~DepthFile ();
};


struct DepthTable : public Depth
{
  const Units& units;
  const double offset;		// [cm]
  LexerTable lex;
  int level_c;			// Column with level data.
  symbol file_unit;		// Unit specified for level in file.
  int entry_count;	        // Number of entries read. -1 uninitialized.

  // Interpolate.
  Time prev_time;
  double prev_value;
  Time next_time;
  double next_value;
  double current_value;

  void tick (const Time& time, const Scope&, Treelog& msg);
  double operator()() const;
  void read_line (const Time& time);
  void initialize (const Time& time, const Scope& scope, Treelog& msg);
  virtual bool check (const Scope&, Treelog&) const;
  DepthTable (const BlockModel& al);
  ~DepthTable ();
};

#endif // DEPTH_H

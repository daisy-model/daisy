// unit_model.h -- The 'unit' component.
// 
// Copyright 2007, 2008 Per Abrahamsen and KVL.
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

#ifndef UNIT_MODEL_H
#define UNIT_MODEL_H

#include "object_model/unit.h"
#include "object_model/model.h"

class BlockModel;

// Component 'unit'.

class MUnit : public Model, public Unit
{
  // Identity.
public:
  const symbol name;
  static const char *const component;
  symbol library_id () const;
private:
  const symbol base_name_;
public:

  // Unit interface.
public:
  symbol base_name () const
  { return base_name_; }
  symbol native_name () const
  { return name; }

  // Convert.
public:
  const Convert* create_convertion (const Unit& to) const;

  // Create and Destroy.
protected:
  MUnit (symbol name, symbol base);
  MUnit (const BlockModel& al, symbol base);
public:
  virtual ~MUnit ();
};

class UnitSI : public MUnit
{
public:
  struct base_unit_type
  {
    symbol unit;
    symbol dimension;
  };
  static const base_unit_type base_unit[];
  static const size_t base_unit_size;
  static symbol find_base (const BlockModel&);
protected:
  UnitSI (symbol name, symbol base);
public:
  explicit UnitSI (const BlockModel& al);
  ~UnitSI ();
};

class UnitSIFactor : public UnitSI
{
  const double factor_;
public:
  double to_base (double value) const;
  double to_native (double value) const;
  bool in_native (double value) const;
  bool in_base (double value) const;
  const Convert* create_convertion (const Unit& to) const;
  UnitSIFactor (symbol name, symbol base, double factor);
  explicit UnitSIFactor (const BlockModel& al);
};

class UnitpF : public MUnit
{
public:
  double to_base (double value) const;
  double to_native (double value) const;
  bool in_native (double value) const;
  bool in_base (double value) const;
  UnitpF ();
  explicit UnitpF (const BlockModel& al);
};

class UnitBase : public MUnit
{
public:
  double to_base (double value) const;
  double to_native (double value) const;
  bool in_native (double value) const;
  bool in_base (double value) const;
  explicit UnitBase (symbol name);
  explicit UnitBase (const BlockModel& al);
};

class UnitFactor : public MUnit
{
  const double factor_;
public:
  double to_base (double value) const;
  double to_native (double value) const;
  bool in_native (double value) const;
  bool in_base (double value) const;
  UnitFactor (symbol name, symbol base, double factor);
  explicit UnitFactor (const BlockModel& al);
};

class UnitOffset : public MUnit
{
  const double factor_;
  const double offset_;
public:
  double to_base (double value) const;
  double to_native (double value) const;
  bool in_native (double value) const;
  bool in_base (double value) const;
  UnitOffset (symbol name, symbol base, double factor, double offset);
  explicit UnitOffset (const BlockModel& al);
};

#endif // UNIT_MODEL_H

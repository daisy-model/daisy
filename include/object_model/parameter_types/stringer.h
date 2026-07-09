// stringer.h --- Strings in Daisy.
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


#ifndef STRINGER_H
#define STRINGER_H

#include "object_model/model.h"
#include "object_model/symbol.h"
#include <memory>

class Scope;
class BlockModel;
class Treelog;
class Units;
class Number;

class Stringer : public Model
{
  // Content.
public:
  static const char *const component;
  symbol library_id () const;
  const symbol name;
  virtual const std::string& title () const;

  // Simulation.
public:
  virtual void tick (const Units&, const Scope& scope, Treelog& msg) = 0;
  virtual bool missing (const Scope& scope) const = 0;
  virtual symbol value (const Scope&) const = 0; 

  // Create and Destroy.
public:
  virtual bool initialize (const Units&, const Scope&, Treelog& msg) = 0;
  virtual bool check (const Units&, const Scope&, Treelog&) const = 0;
protected:
  explicit Stringer (symbol name);
  explicit Stringer (const BlockModel&);
public:
  ~Stringer ();
};

class StringerNumber : public Stringer
{
protected:
  std::unique_ptr<Number> number_;
  StringerNumber (symbol name, std::unique_ptr<Number> number);
public:
  void tick (const Units& units, const Scope& scope, Treelog& msg);
  bool missing (const Scope& scope) const;
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  bool check (const Units& units, const Scope& scope, Treelog& msg) const;
  explicit StringerNumber (const BlockModel&);
  ~StringerNumber ();
};

class StringerValue : public StringerNumber
{
  const int precision_;
public:
  symbol value (const Scope& scope) const;
  StringerValue (std::unique_ptr<Number> number, int precision = -1);
  explicit StringerValue (const BlockModel&);
};

class StringerDimension : public StringerNumber
{
public:
  symbol value (const Scope& scope) const;
  explicit StringerDimension (std::unique_ptr<Number> number);
  explicit StringerDimension (const BlockModel&);
};

class StringerIdentity : public Stringer
{
  const symbol val_;
public:
  void tick (const Units&, const Scope&, Treelog&);
  bool missing (const Scope&) const;
  symbol value (const Scope&) const;
  bool initialize (const Units&, const Scope&, Treelog&);
  bool check (const Units&, const Scope&, Treelog&) const;
  explicit StringerIdentity (symbol value);
  explicit StringerIdentity (const BlockModel&);
  ~StringerIdentity ();
};

#endif // STRINGER_H

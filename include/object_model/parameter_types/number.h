// number.h --- Numbers in Daisy.
// 
// Copyright 2002 Per Abrahamsen and KVL.
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


#ifndef NUMBER_H
#define NUMBER_H

#include "object_model/function.h"
#include "object_model/symbol.h"
#include "object_model/model.h"
#include "object_model/plf.h"
#include <memory>
#include <vector>

class Scope;
class Treelog;
class BlockModel;
class Units;
class Unit;

class Number : public Model
{
  // Content.
public:
  static const char *const component;
  symbol library_id () const;
  const symbol objid;
  virtual symbol title () const;

  // Simulation.
protected:
  static bool known (const symbol);
public:
  bool tick_value (const Units&, 
                   double& value, symbol dim, const Scope& , Treelog&);
  virtual void tick (const Units&, const Scope& scope, Treelog& msg) = 0;
  virtual bool missing (const Scope& scope) const = 0;
  virtual double value (const Scope&) const = 0; 
  virtual symbol dimension (const Scope&) const = 0;
  virtual const Unit& unit () const;

  // Create and Destroy.
public:
  virtual bool initialize (const Units&, const Scope&, Treelog& msg) = 0;
  virtual bool check (const Units&, const Scope&, Treelog&) const = 0;
  bool check_dim (const Units&, const Scope&, symbol dim, Treelog&) const;
protected:
  explicit Number (symbol objid);
  explicit Number (const BlockModel&);
public:
  ~Number ();
};

class NumberConst : public Number
{
  const double val_;
  const Unit& unit_;
public:
  void tick (const Units&, const Scope&, Treelog&);
  bool missing (const Scope&) const;
  double value (const Scope&) const;
  symbol dimension (const Scope&) const;
  const Unit& unit () const;
  bool initialize (const Units&, const Scope&, Treelog&);
  bool check (const Units& units, const Scope&, Treelog& msg) const;
  NumberConst (double value, const Unit& unit);
  explicit NumberConst (const BlockModel&);
  explicit NumberConst (const BlockModel& al, symbol key);
};

class NumberX : public Number
{
  static const symbol name_;
public:
  symbol title () const;
  void tick (const Units&, const Scope&, Treelog&);
  symbol dimension (const Scope& scope) const;
  bool missing (const Scope& scope) const;
  double value (const Scope& scope) const;
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  bool check (const Units& units, const Scope& scope, Treelog& msg) const;
  NumberX ();
  explicit NumberX (const BlockModel&);
};

class NumberGet : public Number
{
  const Unit& unit_;
  const Unit* scope_unit_;
  const symbol name_;
public:
  symbol title () const;
  void tick (const Units&, const Scope&, Treelog&);
  symbol dimension (const Scope&) const;
  const Unit& unit () const;
  bool missing (const Scope& scope) const;
  double value (const Scope& scope) const;
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  bool check (const Units& units, const Scope& scope, Treelog& msg) const;
  NumberGet (symbol name, const Unit& unit);
  explicit NumberGet (const BlockModel&);
  explicit NumberGet (const BlockModel& al, symbol key);
};

class NumberFetchGet : public Number
{
  const Unit* scope_unit_;
  const symbol name_;
public:
  symbol title () const;
  void tick (const Units&, const Scope&, Treelog&);
  symbol dimension (const Scope&) const;
  const Unit& unit () const;
  bool missing (const Scope& scope) const;
  double value (const Scope& scope) const;
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  bool check (const Units& units, const Scope& scope, Treelog& msg) const;
  explicit NumberFetchGet (symbol name);
  NumberFetchGet (const BlockModel& al, symbol key);
};

class NumberFetch : public Number
{
  const std::unique_ptr<Number> child_;
  static std::unique_ptr<Number> fetch_child (const BlockModel& al, symbol key);
public:
  symbol title () const;
  void tick (const Units& units, const Scope& scope, Treelog& msg);
  bool missing (const Scope& scope) const;
  double value (const Scope& scope) const;
  symbol dimension (const Scope& scope) const;
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  bool check (const Units& units, const Scope& scope, Treelog& msg) const;
  explicit NumberFetch (std::unique_ptr<Number> child);
  explicit NumberFetch (const BlockModel&);
};

class NumberApply : public Number
{
  const std::unique_ptr<Function> function_;
  const double operand_;
  const symbol range_;
public:
  void tick (const Units&, const Scope&, Treelog&);
  bool missing (const Scope& scope) const;
  double value (const Scope& scope) const;
  symbol dimension (const Scope&) const;
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  bool check (const Units& units, const Scope& scope, Treelog& msg) const;
  NumberApply (std::unique_ptr<Function> function, double operand, symbol range);
  explicit NumberApply (const BlockModel&);
};

class NumberChild : public Number
{
protected:
  const std::unique_ptr<Number> child_;
  explicit NumberChild (symbol objid, std::unique_ptr<Number> child);
public:
  void tick (const Units& units, const Scope& scope, Treelog& msg);
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  explicit NumberChild (const BlockModel&);
};

class NumberIdentity : public NumberChild
{
  const Units& units_;
  const symbol dim_;
public:
  bool missing (const Scope& scope) const;
  double value (const Scope& scope) const;
  symbol dimension (const Scope& scope) const;
  bool check (const Units& units, const Scope& scope, Treelog& msg) const;
  NumberIdentity (std::unique_ptr<Number> child, const Units& units);
  NumberIdentity (std::unique_ptr<Number> child, const Units& units, symbol dimension);
  explicit NumberIdentity (const BlockModel&);
};

class NumberConvert : public NumberChild
{
  const Units& units_;
  const symbol dim_;
public:
  bool missing (const Scope& scope) const;
  double value (const Scope& scope) const;
  symbol dimension (const Scope&) const;
  bool check (const Units& units, const Scope& scope, Treelog& msg) const;
  NumberConvert (std::unique_ptr<Number> child, const Units& units, symbol dimension);
  explicit NumberConvert (const BlockModel&);
};

class NumberDim : public NumberChild
{
  const symbol dim_;
  const bool warn_known_;
public:
  bool missing (const Scope& scope) const;
  double value (const Scope& scope) const;
  symbol dimension (const Scope&) const;
  bool check (const Units& units, const Scope& scope, Treelog& msg) const;
  NumberDim (std::unique_ptr<Number> child, symbol dimension, bool warn_known = true);
  explicit NumberDim (const BlockModel&);
};

class NumberOperand : public Number
{
protected:
  const std::unique_ptr<Number> operand_;
  NumberOperand (symbol objid, std::unique_ptr<Number> operand);
public:
  void tick (const Units& units, const Scope& scope, Treelog& msg);
  bool missing (const Scope& scope) const;
  symbol dimension (const Scope& scope) const;
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  bool check (const Units& units, const Scope& scope, Treelog& msg) const;
  explicit NumberOperand (const BlockModel&);
};

class NumberLog10 : public NumberOperand
{
public:
  double value (const Scope& scope) const;
  explicit NumberLog10 (std::unique_ptr<Number> operand);
  explicit NumberLog10 (const BlockModel&);
};

class NumberLn : public NumberOperand
{
public:
  double value (const Scope& scope) const;
  explicit NumberLn (std::unique_ptr<Number> operand);
  explicit NumberLn (const BlockModel&);
};

class NumberExp : public NumberOperand
{
public:
  double value (const Scope& scope) const;
  explicit NumberExp (std::unique_ptr<Number> operand);
  explicit NumberExp (const BlockModel&);
};

class NumberSqrt : public NumberOperand
{
public:
  double value (const Scope& scope) const;
  explicit NumberSqrt (std::unique_ptr<Number> operand);
  explicit NumberSqrt (const BlockModel&);
};

class NumberSqr : public NumberOperand
{
public:
  double value (const Scope& scope) const;
  symbol dimension (const Scope& scope) const;
  explicit NumberSqr (std::unique_ptr<Number> operand);
  explicit NumberSqr (const BlockModel&);
};

class NumberPow : public Number
{
  const std::unique_ptr<Number> base_;
  const std::unique_ptr<Number> exponent_;
public:
  void tick (const Units& units, const Scope& scope, Treelog& msg);
  bool missing (const Scope& scope) const;
  double value (const Scope& scope) const;
  symbol dimension (const Scope&) const;
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  bool check (const Units& units, const Scope& scope, Treelog& msg) const;
  NumberPow (std::unique_ptr<Number> base, std::unique_ptr<Number> exponent);
  explicit NumberPow (const BlockModel&);
};

class NumberOperands : public Number
{
protected:
  std::vector<std::unique_ptr<Number>> operands_;
  symbol unique_dimension (const Scope& scope) const;
  NumberOperands (symbol objid, std::vector<std::unique_ptr<Number>> operands);
public:
  void tick (const Units& units, const Scope& scope, Treelog& msg);
  bool missing (const Scope& scope) const;
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  bool check (const Units& units, const Scope& scope, Treelog& msg) const;
  explicit NumberOperands (const BlockModel&);
};

class NumberMax : public NumberOperands
{
public:
  double value (const Scope& scope) const;
  symbol dimension (const Scope& scope) const;
  explicit NumberMax (std::vector<std::unique_ptr<Number>> operands);
  explicit NumberMax (const BlockModel&);
};

class NumberMin : public NumberOperands
{
public:
  double value (const Scope& scope) const;
  symbol dimension (const Scope& scope) const;
  explicit NumberMin (std::vector<std::unique_ptr<Number>> operands);
  explicit NumberMin (const BlockModel&);
};

class NumberProduct : public NumberOperands
{
public:
  double value (const Scope& scope) const;
  symbol dimension (const Scope& scope) const;
  explicit NumberProduct (std::vector<std::unique_ptr<Number>> operands);
  explicit NumberProduct (const BlockModel&);
};

class NumberSum : public NumberOperands
{
public:
  double value (const Scope& scope) const;
  symbol dimension (const Scope& scope) const;
  explicit NumberSum (std::vector<std::unique_ptr<Number>> operands);
  explicit NumberSum (const BlockModel&);
};

class NumberSubtract : public NumberOperands
{
public:
  double value (const Scope& scope) const;
  symbol dimension (const Scope& scope) const;
  explicit NumberSubtract (std::vector<std::unique_ptr<Number>> operands);
  explicit NumberSubtract (const BlockModel&);
};

class NumberDivide : public NumberOperands
{
public:
  double value (const Scope& scope) const;
  symbol dimension (const Scope& scope) const;
  explicit NumberDivide (std::vector<std::unique_ptr<Number>> operands);
  explicit NumberDivide (const BlockModel&);
};

class NumberPLF : public Number
{
  const std::unique_ptr<Number> operand_;
  const symbol domain_;
  const symbol range_;
  const PLF plf_;
  double operand_value_;
  bool operand_missing_;
public:
  void tick (const Units& units, const Scope& scope, Treelog& msg);
  bool missing (const Scope& scope) const;
  double value (const Scope& scope) const;
  symbol dimension (const Scope&) const;
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  bool check (const Units& units, const Scope& scope, Treelog& msg) const;
  NumberPLF (std::unique_ptr<Number> operand, symbol domain, symbol range, const PLF& plf);
  explicit NumberPLF (const BlockModel&);
};

#endif // NUMBER_H

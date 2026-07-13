// integer.h --- Integers in Daisy.
// 
// Copyright 2006 Per Abrahamsen and KVL.
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


#ifndef INTEGER_H
#define INTEGER_H

#include "object_model/parameter_types/boolean.h"
#include "object_model/model.h"
#include "object_model/symbol.h"
#include <memory>
#include <vector>

class Scope;
class Treelog;
class BlockModel;
class Units;
class Frame;
class Block;

class Integer : public Model
{
  // Content.
public:
  static const char *const component;
  symbol library_id () const;
  const symbol name;
  virtual const std::string& title () const;

  // Simulation.
public:
  virtual bool missing (const Scope& scope) const = 0;
  virtual int value (const Scope&) const = 0; 


  // Create and Destroy.
public:
  virtual bool initialize (const Units&, const Scope&, Treelog&) = 0;
  virtual bool check (const Scope&, Treelog&) const = 0;
protected:
  explicit Integer (symbol name);
  explicit Integer (const BlockModel&);
public:
  ~Integer ();
};

class IntegerConst : public Integer
{
  const int val_;
public:
  bool missing (const Scope& scope) const;
  int value (const Scope&) const;
  bool initialize (const Units&, const Scope&, Treelog&);
  bool check (const Scope&, Treelog&) const;
  explicit IntegerConst (int value);
  explicit IntegerConst (const BlockModel&);
};

class IntegerCond : public Integer
{
public:
  class Clause
  {
    std::unique_ptr<Boolean> condition_;
    const int value_;
  public:
    static void load_syntax (Frame& frame);
    Clause (std::unique_ptr<Boolean> condition, int value);
    Clause (const Block& al);
    void initialize (const Units& units, const Scope& scope,
                     Treelog& msg, const symbol& owner_name,
                     size_t index, bool& ok) const;
    bool matches (const Scope& scope) const;
    int value () const;
  };
private:
  std::vector<Clause> clauses_;
public:
  bool missing (const Scope&) const;
  int value (const Scope& scope) const;
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  bool check (const Scope& scope, Treelog& msg) const;
  explicit IntegerCond (std::vector<Clause> clauses);
  explicit IntegerCond (const BlockModel&);
};

class IntegerOperand : public Integer
{
protected:
  const std::unique_ptr<Integer> operand_;
  IntegerOperand (symbol name, std::unique_ptr<Integer> operand);
public:
  bool missing (const Scope& scope) const;
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  bool check (const Scope& scope, Treelog& msg) const;
  explicit IntegerOperand (const BlockModel&);
};

class IntegerSqr : public IntegerOperand
{
public:
  int value (const Scope& scope) const;
  explicit IntegerSqr (std::unique_ptr<Integer> operand);
  explicit IntegerSqr (const BlockModel&);
};

class IntegerOperands : public Integer
{
protected:
  std::vector<std::unique_ptr<Integer>> operands_;
  IntegerOperands (symbol name, std::vector<std::unique_ptr<Integer>> operands);
public:
  bool missing (const Scope& scope) const;
  bool initialize (const Units& units, const Scope& scope, Treelog& msg);
  bool check (const Scope& scope, Treelog& msg) const;
  explicit IntegerOperands (const BlockModel&);
};

class IntegerMax : public IntegerOperands
{
public:
  int value (const Scope& scope) const;
  explicit IntegerMax (std::vector<std::unique_ptr<Integer>> operands);
  explicit IntegerMax (const BlockModel&);
};

class IntegerMin : public IntegerOperands
{
public:
  int value (const Scope& scope) const;
  explicit IntegerMin (std::vector<std::unique_ptr<Integer>> operands);
  explicit IntegerMin (const BlockModel&);
};

class IntegerProduct : public IntegerOperands
{
public:
  int value (const Scope& scope) const;
  explicit IntegerProduct (std::vector<std::unique_ptr<Integer>> operands);
  explicit IntegerProduct (const BlockModel&);
};

class IntegerSum : public IntegerOperands
{
public:
  int value (const Scope& scope) const;
  explicit IntegerSum (std::vector<std::unique_ptr<Integer>> operands);
  explicit IntegerSum (const BlockModel&);
};

class IntegerSubtract : public IntegerOperands
{
public:
  int value (const Scope& scope) const;
  explicit IntegerSubtract (std::vector<std::unique_ptr<Integer>> operands);
  explicit IntegerSubtract (const BlockModel&);
};

class IntegerDivide : public IntegerOperands
{
protected:
  IntegerDivide (symbol name, std::vector<std::unique_ptr<Integer>> operands);
public:
  int value (const Scope& scope) const;
  bool check (const Scope& scope, Treelog& msg) const;
  explicit IntegerDivide (std::vector<std::unique_ptr<Integer>> operands);
  explicit IntegerDivide (const BlockModel&);
};

class IntegerModulo : public IntegerDivide
{
public:
  int value (const Scope& scope) const;
  explicit IntegerModulo (std::vector<std::unique_ptr<Integer>> operands);
  explicit IntegerModulo (const BlockModel&);
};

#endif // INTEGER_H

// integer_arit.C -- Arithmetics on integers.
// 
// Copyright 2004, 2005 Per Abrahamsen and KVL.
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

#include "object_model/parameter_types/integer.h"
#include "object_model/vcheck.h"
#include "util/assertion.h"
#include "object_model/librarian.h"
#include "object_model/object_model_registration_internal.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"
#include "object_model/block_model.h"
#include <sstream>
#include <memory>

namespace
{
std::vector<std::unique_ptr<Integer>>
build_operands (const BlockModel& al)
{
  std::vector<std::unique_ptr<Integer>> result;
  std::vector<Integer*> operands = Librarian::build_vector<Integer> (al, "operands");
  result.reserve (operands.size ());
  for (size_t i = 0; i < operands.size (); ++i)
    result.emplace_back (operands[i]);
  return result;
}
} // namespace

IntegerOperand::IntegerOperand (symbol objid, std::unique_ptr<Integer> operand)
  : Integer (objid),
    operand_ (std::move (operand))
{ }

bool
IntegerOperand::missing (const Scope& scope) const
{ return operand_->missing (scope); }

bool
IntegerOperand::initialize (const Units& units, const Scope& scope, Treelog& err)
{
  Treelog::Open nest (err, name);
  return operand_->initialize (units, scope, err);
}

bool
IntegerOperand::check (const Scope& scope, Treelog& err) const
{
  Treelog::Open nest (err, name);
  return operand_->check (scope, err);
}

IntegerOperand::IntegerOperand (const BlockModel& al)
  : Integer (al),
    operand_ (Librarian::build_item<Integer> (al, "operand"))
{ }

int
IntegerSqr::value (const Scope& scope) const
{
  const int v = operand_->value (scope);
  return v * v;
}

IntegerSqr::IntegerSqr (std::unique_ptr<Integer> operand)
  : IntegerOperand ("sqr", std::move (operand))
{ }

IntegerSqr::IntegerSqr (const BlockModel& al)
  : IntegerOperand (al)
{ }

struct IntegerSqrSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new IntegerSqr (al); }
  IntegerSqrSyntax ()
    : DeclareModel (Integer::component, "sqr", 
	       "Take the square of its argument.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operand", Integer::component,
                       "Operand for this function.");
    frame.order ("operand");
  }
};

IntegerOperands::IntegerOperands (symbol objid,
                                  std::vector<std::unique_ptr<Integer>> operands)
  : Integer (objid),
    operands_ (std::move (operands))
{ }

bool
IntegerOperands::missing (const Scope& scope) const
{
  for (size_t i = 0; i < operands_.size (); i++)
    if (operands_[i]->missing (scope))
      return true;
  return false;
}

bool
IntegerOperands::initialize (const Units& units, const Scope& scope, Treelog& err)
{
  bool ok = true;
  for (size_t i = 0; i < operands_.size (); i++)
    {
      std::ostringstream tmp;
      tmp << name << "[" << i << "]";
      Treelog::Open nest (err, tmp.str ());

      if (!operands_[i]->initialize (units, scope, err))
        ok = false;
    }
  return ok;
}

bool
IntegerOperands::check (const Scope& scope, Treelog& err) const
{
  bool ok = true;
  for (size_t i = 0; i < operands_.size (); i++)
    {
      std::ostringstream tmp;
      tmp << name << "[" << i << "]";
      Treelog::Open nest (err, tmp.str ());

      if (!operands_[i]->check (scope, err))
        ok = false;
    }
  return ok;
}

IntegerOperands::IntegerOperands (const BlockModel& al)
  : Integer (al),
    operands_ (build_operands (al))
{ }

int
IntegerMax::value (const Scope& scope) const
{
  daisy_assert (operands_.size () > 0);
  int max = operands_[0]->value (scope);
  for (size_t i = 1; i < operands_.size (); i++)
    {
      const int value = operands_[i]->value (scope);
      if (value > max)
        max = value;
    }
  return max;
}

IntegerMax::IntegerMax (std::vector<std::unique_ptr<Integer>> operands)
  : IntegerOperands ("max", std::move (operands))
{ }

IntegerMax::IntegerMax (const BlockModel& al)
  : IntegerOperands (al)
{ }

int
IntegerMin::value (const Scope& scope) const
{
  daisy_assert (operands_.size () > 0);
  int min = operands_[0]->value (scope);
  for (size_t i = 1; i < operands_.size (); i++)
    {
      const int value = operands_[i]->value (scope);
      if (value < min)
        min = value;
    }
  return min;
}

IntegerMin::IntegerMin (std::vector<std::unique_ptr<Integer>> operands)
  : IntegerOperands ("min", std::move (operands))
{ }

IntegerMin::IntegerMin (const BlockModel& al)
  : IntegerOperands (al)
{ }

int
IntegerProduct::value (const Scope& scope) const
{
  int product = 1;
  for (size_t i = 0; i < operands_.size (); i++)
    product *= operands_[i]->value (scope);
  return product;
}

IntegerProduct::IntegerProduct (std::vector<std::unique_ptr<Integer>> operands)
  : IntegerOperands ("*", std::move (operands))
{ }

IntegerProduct::IntegerProduct (const BlockModel& al)
  : IntegerOperands (al)
{ }

int
IntegerSum::value (const Scope& scope) const
{
  int sum = 0;
  for (size_t i = 0; i < operands_.size (); i++)
    sum += operands_[i]->value (scope);
  return sum;
}

IntegerSum::IntegerSum (std::vector<std::unique_ptr<Integer>> operands)
  : IntegerOperands ("+", std::move (operands))
{ }

IntegerSum::IntegerSum (const BlockModel& al)
  : IntegerOperands (al)
{ }

int
IntegerSubtract::value (const Scope& scope) const
{
  daisy_assert (operands_.size () > 0);
  int val = operands_[0]->value (scope);
  if (operands_.size () == 1)
    return -val;
  for (size_t i = 1; i < operands_.size (); i++)
    val -= operands_[i]->value (scope);
  return val;
}

IntegerSubtract::IntegerSubtract (std::vector<std::unique_ptr<Integer>> operands)
  : IntegerOperands ("-", std::move (operands))
{ }

IntegerSubtract::IntegerSubtract (const BlockModel& al)
  : IntegerOperands (al)
{ }

int
IntegerDivide::value (const Scope& scope) const
{
  daisy_assert (operands_.size () == 2);
  int v1 = operands_[0]->value (scope);
  int v2 = operands_[1]->value (scope);
  if (v2 == 0)
    throw ("Divide by zero");
  return v1 / v2;
}

bool
IntegerDivide::check (const Scope& scope, Treelog& err) const
{
  bool ok = true;
  for (size_t i = 0; i < operands_.size (); i++)
    {
      std::ostringstream tmp;
      tmp << name << "[" << i << "]";
      Treelog::Open nest (err, tmp.str ());

      if (!operands_[i]->check (scope, err))
        ok = false;
      if (i > 0 && operands_[i]->value (scope) == 0)
        {
          err.error ("Divide by zero");
          ok = false;
        }
    }
  return ok;
}

IntegerDivide::IntegerDivide (symbol objid,
                              std::vector<std::unique_ptr<Integer>> operands)
  : IntegerOperands (objid, std::move (operands))
{ }

IntegerDivide::IntegerDivide (std::vector<std::unique_ptr<Integer>> operands)
  : IntegerDivide ("div", std::move (operands))
{ }

IntegerDivide::IntegerDivide (const BlockModel& al)
  : IntegerOperands (al)
{ }

struct IntegerMaxSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new IntegerMax (al); }
  IntegerMaxSyntax ()
    : DeclareModel (Integer::component, "max", 
	       "Use the largest value of its operands.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operands", Integer::component,
                       Attribute::Const, Attribute::Variable,
                       "The operands for this function.");
    frame.set_check ("operands", VCheck::min_size_1 ());
    frame.order ("operands");
  }
};

struct IntegerMinSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new IntegerMin (al); }
  IntegerMinSyntax ()
    : DeclareModel (Integer::component, "min", 
	       "Use the smallest value of its operands.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operands", Integer::component,
                       Attribute::Const, Attribute::Variable,
                       "The operands for this function.");
    frame.set_check ("operands", VCheck::min_size_1 ());
    frame.order ("operands");
  }
};

struct IntegerProductSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new IntegerProduct (al); }
  IntegerProductSyntax ()
    : DeclareModel (Integer::component, "*", 
	       "Use the product of its operands.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operands", Integer::component,
                       Attribute::Const, Attribute::Variable,
                       "The operands for this function.");
    frame.order ("operands");
  }
};

struct IntegerSumSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new IntegerSum (al); }
  IntegerSumSyntax ()
    : DeclareModel (Integer::component, "+", 
	       "Use the sum of its operands.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operands", Integer::component,
                       Attribute::Const, Attribute::Variable,
                       "The operands for this function.");
#ifdef CHECK_OPERANDS_DIM
    frame.set_check ("operands", IntegerOperands::unique);
#endif // CHECK_OPERANDS_DIM
    frame.order ("operands");
  }
};

struct IntegerSubtractSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new IntegerSubtract (al); }
  IntegerSubtractSyntax ()
    : DeclareModel (Integer::component, "-", 
	       "Negate integer or subtract integers.\n\
With one operand, negates it.  With more than one operand,\n\
subtracts all but the first from the first.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operands", Integer::component,
                       Attribute::Const, Attribute::Variable,
                       "The operands for this function.");
    frame.order ("operands");
  }
};

int
IntegerModulo::value (const Scope& scope) const
{
  daisy_assert (operands_.size () == 2);
  int v1 = operands_[0]->value (scope);
  int v2 = operands_[1]->value (scope);
  if (v2 == 0)
    throw ("Modulo by zero");
  return v1 % v2;
}

IntegerModulo::IntegerModulo (std::vector<std::unique_ptr<Integer>> operands)
  : IntegerDivide ("mod", std::move (operands))
{ }

IntegerModulo::IntegerModulo (const BlockModel& al)
  : IntegerDivide (al)
{ }

struct IntegerModuloSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new IntegerModulo (al); }
  IntegerModuloSyntax ()
    : DeclareModel (Integer::component, "mod", 
                 "Modulo the first operand by the rest.")
  { }
  void load_frame (Frame& frame) const
  {

      frame.declare_object ("operands", Integer::component,
                         Attribute::Const, 2,
                         "The operands for this function.");
      frame.order ("operands");
  }
};

struct IntegerDivideSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new IntegerDivide (al); }
  IntegerDivideSyntax ()
    : DeclareModel (Integer::component, "div", 
                 "Divide the first operand by the rest.")
  { }
  void load_frame (Frame& frame) const
  {

      frame.declare_object ("operands", Integer::component,
                         Attribute::Const, 2,
                         "The operands for this function.");
      frame.order ("operands");
  }
};

void
register_integer_arithmetic_models ()
{
  static IntegerSqrSyntax integer_sqr_syntax;
  static IntegerMaxSyntax integer_max_syntax;
  static IntegerMinSyntax integer_min_syntax;
  static IntegerProductSyntax integer_product_syntax;
  static IntegerSumSyntax integer_sum_syntax;
  static IntegerSubtractSyntax integer_subtract_syntax;
  static IntegerModuloSyntax integer_modulo_syntax;
  static IntegerDivideSyntax integer_divide_syntax;
}

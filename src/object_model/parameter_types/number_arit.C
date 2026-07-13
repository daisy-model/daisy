// number_arit.C -- Arithmetics on numbers.
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

#include "object_model/parameter_types/number.h"
#include "object_model/units.h"
#include "object_model/vcheck.h"
#include "util/mathlib.h"
#include "util/memutils.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "object_model/submodeler.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"
#include <sstream>
#include <memory>

namespace
{
std::vector<std::unique_ptr<Number>>
build_operands (const BlockModel& al)
{
  std::vector<std::unique_ptr<Number>> result;
  std::vector<Number*> operands = Librarian::build_vector<Number> (al, "operands");
  result.reserve (operands.size ());
  for (size_t i = 0; i < operands.size (); ++i)
    result.emplace_back (operands[i]);
  return result;
}
} // namespace

NumberOperand::NumberOperand (const symbol objid, std::unique_ptr<Number> operand)
  : Number (objid),
    operand_ (std::move (operand))
{ }

void
NumberOperand::tick (const Units& units, const Scope& scope, Treelog& msg)
{ operand_->tick (units, scope, msg); }

bool
NumberOperand::missing (const Scope& scope) const
{ return operand_->missing (scope); }

symbol
NumberOperand::dimension (const Scope& scope) const
{
  if (operand_->dimension (scope) == Attribute::None ())
    return Attribute::None ();

  return Attribute::Unknown ();
}

bool
NumberOperand::initialize (const Units& units, const Scope& scope, Treelog& msg)
{
  TREELOG_MODEL (msg);
  return operand_->initialize (units, scope, msg);
}

bool
NumberOperand::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  TREELOG_MODEL (msg);
  return operand_->check (units, scope, msg);
}

NumberOperand::NumberOperand (const BlockModel& al)
  : Number (al),
    operand_ (Librarian::build_item<Number> (al, "operand"))
{ }

double
NumberLog10::value (const Scope& scope) const
{
  const double v = operand_->value (scope);
  daisy_assert (v > 0.0);
  return log10 (v);
}

NumberLog10::NumberLog10 (std::unique_ptr<Number> operand)
  : NumberOperand ("log10", std::move (operand))
{ }

NumberLog10::NumberLog10 (const BlockModel& al)
  : NumberOperand (al)
{ }

static struct NumberLog10Syntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberLog10 (al); }
  NumberLog10Syntax ()
    : DeclareModel (Number::component, "log10", 
	       "Take the base 10 logarithm of its argument.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operand", Number::component,
                       "Operand for this function.");
    frame.order ("operand");
  }
} NumberLog10_syntax;

double
NumberLn::value (const Scope& scope) const
{
  const double v = operand_->value (scope);
  daisy_assert (v > 0.0);
  return log (v);
}

NumberLn::NumberLn (std::unique_ptr<Number> operand)
  : NumberOperand ("ln", std::move (operand))
{ }

NumberLn::NumberLn (const BlockModel& al)
  : NumberOperand (al)
{ }

static struct NumberLnSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberLn (al); }
  NumberLnSyntax ()
    : DeclareModel (Number::component, "ln", 
	       "Take the natural logarithm of its argument.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operand", Number::component,
                       "Operand for this function.");
    frame.order ("operand");
  }
} NumberLn_syntax;

double
NumberExp::value (const Scope& scope) const
{
  const double v = operand_->value (scope);
  return exp (v);
}

NumberExp::NumberExp (std::unique_ptr<Number> operand)
  : NumberOperand ("exp", std::move (operand))
{ }

NumberExp::NumberExp (const BlockModel& al)
  : NumberOperand (al)
{ }

static struct NumberExpSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberExp (al); }
  NumberExpSyntax ()
    : DeclareModel (Number::component, "exp", 
	       "Take the exponential of its argument.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operand", Number::component,
                       "Operand for this function.");
    frame.order ("operand");
  }
} NumberExp_syntax;

double
NumberSqrt::value (const Scope& scope) const
{
  const double v = operand_->value (scope);
  daisy_assert (v >= 0.0);
  return sqrt (v);
}

NumberSqrt::NumberSqrt (std::unique_ptr<Number> operand)
  : NumberOperand ("sqrt", std::move (operand))
{ }

NumberSqrt::NumberSqrt (const BlockModel& al)
  : NumberOperand (al)
{ }

static struct NumberSqrtSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberSqrt (al); }
  NumberSqrtSyntax ()
    : DeclareModel (Number::component, "sqrt", 
	       "Take the square root of its argument.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operand", Number::component,
                       "Operand for this function.");
    frame.order ("operand");
  }
} NumberSqrt_syntax;

double
NumberSqr::value (const Scope& scope) const
{
  const double v = operand_->value (scope);
  return v * v;
}

symbol
NumberSqr::dimension (const Scope& scope) const
{
  const symbol opdim = operand_->dimension (scope);
  return Units::multiply (opdim, opdim);
}

NumberSqr::NumberSqr (std::unique_ptr<Number> operand)
  : NumberOperand ("sqr", std::move (operand))
{ }

NumberSqr::NumberSqr (const BlockModel& al)
  : NumberOperand (al)
{ }

static struct NumberSqrSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberSqr (al); }
  NumberSqrSyntax ()
    : DeclareModel (Number::component, "sqr", 
	       "Take the square of its argument.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operand", Number::component,
                       "Operand for this function.");
    frame.order ("operand");
  }
} NumberSqr_syntax;

void
NumberPow::tick (const Units& units, const Scope& scope, Treelog& msg)
{
  base_->tick (units, scope, msg);
  exponent_->tick (units, scope, msg);
}

bool
NumberPow::missing (const Scope& scope) const
{ return base_->missing (scope) || exponent_->missing (scope); }

double
NumberPow::value (const Scope& scope) const
{
  const double x = base_->value (scope);
  const double y = exponent_->value (scope);
  daisy_assert (x >= 0.0);
  return pow (x, y);
}

symbol
NumberPow::dimension (const Scope&) const
{ return Attribute::Unknown (); }

bool
NumberPow::initialize (const Units& units, const Scope& scope, Treelog& msg)
{
  TREELOG_MODEL (msg);
  bool ok = true;
  if (!base_->initialize (units, scope, msg))
    ok = false;
  if (!exponent_->initialize (units, scope, msg))
    ok = false;
  return ok;
}

bool
NumberPow::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  TREELOG_MODEL (msg);
  bool ok = true;
  if (!base_->check (units, scope, msg))
    ok = false;
  if (!exponent_->check (units, scope, msg))
    ok = false;
  return ok;
}

NumberPow::NumberPow (std::unique_ptr<Number> base,
                      std::unique_ptr<Number> exponent)
  : Number ("pow"),
    base_ (std::move (base)),
    exponent_ (std::move (exponent))
{ }

NumberPow::NumberPow (const BlockModel& al)
  : Number (al),
    base_ (Librarian::build_item<Number> (al, "base")),
    exponent_ (Librarian::build_item<Number> (al, "exponent"))
{ }

static struct NumberPowSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberPow (al); }
  NumberPowSyntax ()
    : DeclareModel (Number::component, "pow", 
	       "Raise 'base' to the power of 'exponent'.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("base", Number::component,
                       "The base operand for this function.");
    frame.declare_object ("exponent", Number::component,
                       "The exponent operand for this function.");
    frame.order ("base", "exponent");
  }
} NumberPow_syntax;

symbol
NumberOperands::unique_dimension (const Scope& scope) const
{
  static const symbol unspecified ("<unspecified>");
  symbol found = unspecified;
  for (size_t i = 0; i < operands_.size (); i++)
    if (known (operands_[i]->dimension (scope)))
      {
        if (found == unspecified)
          found = operands_[i]->dimension (scope);
        else if (operands_[i]->dimension (scope) != found)
          return Attribute::Unknown ();
      }

  return found != unspecified ? found : Attribute::Unknown ();
}

void
NumberOperands::tick (const Units& units, const Scope& scope, Treelog& msg)
{
  for (size_t i = 0; i < operands_.size (); i++)
    operands_[i]->tick (units, scope, msg);
}

bool
NumberOperands::missing (const Scope& scope) const
{
  for (size_t i = 0; i < operands_.size (); i++)
    if (operands_[i]->missing (scope))
      return true;
  return false;
}

bool
NumberOperands::initialize (const Units& units, const Scope& scope, Treelog& msg)
{
  bool ok = true;
  for (size_t i = 0; i < operands_.size (); i++)
    {
      std::ostringstream tmp;
      tmp << objid << "[" << i << "]";
      Treelog::Open nest (msg, tmp.str ());

      if (!operands_[i]->initialize (units, scope, msg))
        ok = false;
    }
  return ok;
}
#ifdef CHECK_OPERANDS_DIM
const struct NumberOperandsUnique : public VCheck
  {
    void check (Metalib&, const Frame& al, const std::string&)
      const throw (std::string)
    {
      typedef std::vector<const Number*> op_x;

      const struct Operands : public  op_x
      {
        Operands (const BlockModel& Block, const std::vector<const Frame*>& as)
          : op_x (Librarian:build_vector_const<Number> (as))
        { }
        ~Operands ()
        { sequence_delete (begin (), end ()); }
      } operands (frame_sequence ("operands"));
      
      const string* found = NULL;
      for (size_t i = 0; i < operands.size (); i++)
        if (known (operands[i]->dimension (scope)))
          if (found)
            {
              if (operands[i]->dimension (scope) != *found)
                {
                  std::ostringstream tmp;
                  tmp << "Dimension [" << operands[i]->dimension (scope) 
                         << "] differ from [" << *found << "]";
                  throw string (tmp.str ());
                }
            }
          else
            found = &operands[i]->dimension (scope);
    }
    NumberOperandsUnique (const Scope& s)
  } number_operands_unique;
#endif // CHECK_OPERANDS_DIM

bool
NumberOperands::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  bool ok = true;
  for (size_t i = 0; i < operands_.size (); i++)
    {
    std::ostringstream tmp;
    tmp << objid << "[" << i << "]";
    Treelog::Open nest (msg, tmp.str ());

    if (!operands_[i]->check (units, scope, msg))
      ok = false;
    }
  return ok;
}

NumberOperands::NumberOperands (const symbol objid,
                              std::vector<std::unique_ptr<Number>> operands)
  : Number (objid),
    operands_ (std::move (operands))
{ }

NumberOperands::NumberOperands (const BlockModel& al)
  : Number (al),
    operands_ (build_operands (al))
{ }

double
NumberMax::value (const Scope& scope) const
{
  daisy_assert (operands_.size () > 0);
  double max = -42.42e42;
  for (size_t i = 0; i < operands_.size (); i++)
    {
    const double value = operands_[i]->value (scope);
    if (i == 0 || value > max)
      max = value;
    }
  return max;
}

symbol
NumberMax::dimension (const Scope& scope) const
{ return unique_dimension (scope); }

NumberMax::NumberMax (std::vector<std::unique_ptr<Number>> operands)
  : NumberOperands ("max", std::move (operands))
{ }

NumberMax::NumberMax (const BlockModel& al)
  : NumberOperands (al)
{ }

static struct NumberMaxSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberMax (al); }
  NumberMaxSyntax ()
    : DeclareModel (Number::component, "max", 
	       "Use the largest value of its operands.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operands", Number::component,
                       Attribute::Const, Attribute::Variable,
                       "The operands for this function.");
#ifdef CHECK_OPERANDS_DIM
    static VCheck::All all (VCheck::min_size_1 (),
                            number_operands_unique);
#endif // CHECK_OPERANDS_DIM
    frame.set_check ("operands", VCheck::min_size_1 ());
    frame.order ("operands");
  }
} NumberMax_syntax;

double
NumberMin::value (const Scope& scope) const
{
  daisy_assert (operands_.size () > 0);
  double min = 42.42e42;
  for (size_t i = 0; i < operands_.size (); i++)
    {
      const double value = operands_[i]->value (scope);
      if (i == 0 || value < min)
        min = value;
    }
  return min;
}

symbol
NumberMin::dimension (const Scope& scope) const
{ return unique_dimension (scope); }

NumberMin::NumberMin (std::vector<std::unique_ptr<Number>> operands)
  : NumberOperands ("min", std::move (operands))
{ }

NumberMin::NumberMin (const BlockModel& al)
  : NumberOperands (al)
{ }

static struct NumberMinSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberMin (al); }
  NumberMinSyntax ()
    : DeclareModel (Number::component, "min", 
	       "Use the smallest value of its operands.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operands", Number::component,
                       Attribute::Const, Attribute::Variable,
                       "The operands for this function.");
#ifdef CHECK_OPERANDS_DIM
    static VCheck::All all (VCheck::min_size_1 (), 
                            number_operands_unique);
    frame.set_check ("operands", all);
#else // !CHECK_OPERANDS_DIM
    frame.set_check ("operands", VCheck::min_size_1 ());
#endif // !CHECK_OPERANDS_DIM
    frame.order ("operands");
  }
} NumberMin_syntax;

double
NumberProduct::value (const Scope& scope) const
{
  double product = 1.0;
  for (size_t i = 0; i < operands_.size (); i++)
    product *= operands_[i]->value (scope);
  return product;
}

symbol
NumberProduct::dimension (const Scope& scope) const
{
  symbol dim = Attribute::None ();
  for (size_t i = 0; i < operands_.size (); i++)
    dim = Units::multiply (dim, operands_[i]->dimension (scope));
  return dim;
}

NumberProduct::NumberProduct (std::vector<std::unique_ptr<Number>> operands)
  : NumberOperands ("*", std::move (operands))
{ }

NumberProduct::NumberProduct (const BlockModel& al)
  : NumberOperands (al)
{ }

static struct NumberProductSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberProduct (al); }
  NumberProductSyntax ()
    : DeclareModel (Number::component, "*", 
	       "Use the product of its operands.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operands", Number::component,
                       Attribute::Const, Attribute::Variable,
                       "The operands for this function.");
    frame.order ("operands");
  }
} NumberProduct_syntax;

double
NumberSum::value (const Scope& scope) const
{
  double sum = 0.0;
  for (size_t i = 0; i < operands_.size (); i++)
    sum += operands_[i]->value (scope);
  return sum;
}

symbol
NumberSum::dimension (const Scope& scope) const
{ return unique_dimension (scope); }

NumberSum::NumberSum (std::vector<std::unique_ptr<Number>> operands)
  : NumberOperands ("+", std::move (operands))
{ }

NumberSum::NumberSum (const BlockModel& al)
  : NumberOperands (al)
{ }

static struct NumberSumSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberSum (al); }
  NumberSumSyntax ()
    : DeclareModel (Number::component, "+", 
	       "Use the sum of its operands.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operands", Number::component,
                       Attribute::Const, Attribute::Variable,
                       "The operands for this function.");
#ifdef CHECK_OPERANDS_DIM
    frame.set_check ("operands", number_operands_unique);
#endif // CHECK_OPERANDS_DIM
    frame.order ("operands");
  }
} NumberSum_syntax;

double
NumberSubtract::value (const Scope& scope) const
{
  daisy_assert (operands_.size () > 0);
  double val = operands_[0]->value (scope);
  if (operands_.size () == 1)
    return -val;
  for (size_t i = 1; i < operands_.size (); i++)
    val -= operands_[i]->value (scope);
  return val;
}

symbol
NumberSubtract::dimension (const Scope& scope) const
{ return unique_dimension (scope); }

NumberSubtract::NumberSubtract (std::vector<std::unique_ptr<Number>> operands)
  : NumberOperands ("-", std::move (operands))
{ }

NumberSubtract::NumberSubtract (const BlockModel& al)
  : NumberOperands (al)
{ }

static struct NumberSubtractSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberSubtract (al); }
  NumberSubtractSyntax ()
    : DeclareModel (Number::component, "-", 
	       "Negate number or subtract numbers.\n\
With one operand, negates it.  With more than one operand,\n\
subtracts all but the first from the first.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operands", Number::component,
                       Attribute::Const, Attribute::Variable,
                       "The operands for this function.");
#ifdef CHECK_OPERANDS_DIM
    static VCheck::All all (VCheck::min_size_1 (), 
                            number_operands_unique);
    frame.set_check ("operands", all);
#endif // CHECK_OPERANDS_DIM
    frame.order ("operands");
  }
} NumberSubtract_syntax;

double
NumberDivide::value (const Scope& scope) const
{
  daisy_assert (operands_.size () > 0);
  double val = operands_[0]->value (scope);
  for (size_t i = 1; i < operands_.size (); i++)
    val /= operands_[i]->value (scope);
  return val;
}

symbol
NumberDivide::dimension (const Scope& scope) const
{
  daisy_assert (operands_.size () > 0);
  std::string name = operands_[0]->dimension (scope).name ();
  for (size_t i = 1; i < operands_.size (); i++)
    {
      const symbol dim = operands_[i]->dimension (scope);
      if (dim == Attribute::None () || dim == Attribute::Fraction ())
        continue;
      if (dim == Attribute::Unknown ())
        return Attribute::Unknown ();
      const std::string dimstr = dim.name ();
      if (dimstr.length () == 0)
        continue;
      if (dimstr == name)
        {
          name = "";
          continue;
        }
      if (dimstr.find_first_of ('/') != std::string::npos)
        name += "/(" + dimstr + ")";
      else
        name += "/" + dimstr;
    }
  if (name.length () == 0)
    return Attribute::None ();

  return symbol (name);
}

NumberDivide::NumberDivide (std::vector<std::unique_ptr<Number>> operands)
  : NumberOperands ("/", std::move (operands))
{ }

NumberDivide::NumberDivide (const BlockModel& al)
  : NumberOperands (al)
{ }

static struct NumberDivideSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberDivide (al); }
  NumberDivideSyntax ()
    : DeclareModel (Number::component, "/", 
	       "Divide the first operand by the rest.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("operands", Number::component,
                       Attribute::Const, Attribute::Variable,
                       "The operands for this function.");
    frame.set_check ("operands", VCheck::min_size_1 ());
    frame.order ("operands");
  }
} NumberDivide_syntax;

// number_arit.C ends here.


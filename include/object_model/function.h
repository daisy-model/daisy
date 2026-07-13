// function.h -- Pure functions of one parameter.
// 
// Copyright 2023 KU
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


#ifndef FUNCTION_H
#define FUNCTION_H

#include "object_model/model.h"
#include "object_model/plf.h"
#include "object_model/symbol.h"
#include <memory>
#include <vector>

class BlockModel;

class Function : public Model
{
  // Content.
public:
  static const char *const component;

  // Simulation.
public:
  virtual double value (const double) const = 0;

  // Utility
public:
  virtual void plot_xy (std::vector<double>& x, std::vector<double>& y) const;

  // Create and Destroy.
protected:
  Function ();
  Function (const BlockModel&);
public:
  ~Function ();
};

class FunctionPlotable : public Function
{
protected:
  void plot_xy (std::vector<double>& x, std::vector<double>& y) const;
  virtual double x_min () const = 0;
  virtual double x_max () const = 0;
  FunctionPlotable (const BlockModel&);
  ~FunctionPlotable ();
};

class FunctionConst : public Function
{
  const double value_;
public:
  double value (const double) const;
  explicit FunctionConst (double value);
  explicit FunctionConst (const BlockModel&);
};

class FunctionPLF : public Function
{
  const PLF plf_;
public:
  double value (const double x) const;
  explicit FunctionPLF (const PLF& plf);
  explicit FunctionPLF (const BlockModel&);
};

#ifdef BUILD_PYTHON
class FunctionPython : public Function
{
  struct Implementation;
  const symbol pmodule_;
  const symbol pname_;
  const symbol domain_;
  const symbol range_;
  mutable std::unique_ptr<Implementation> impl_;
public:
  double value (const double arg) const;
  FunctionPython (symbol module, symbol name, symbol domain, symbol range);
  explicit FunctionPython (const BlockModel&);
  ~FunctionPython ();
};
#endif

#endif // FUNCTION_H

// function.h ends here.

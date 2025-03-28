// xysource.h -- 2D data source for gnuplot interface 
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

#ifndef XYSOURCE_H
#define XYSOURCE_H

#include "object_model/model.h"
#include "object_model/symbol.h"
#include <vector>

class BlockModel;
class Treelog;
class Units;

class XYSource : public Model
{
  // Content.
public:
  const symbol objid;
  static const char *const component;
  symbol library_id () const;

  // Interface.
public:
  virtual symbol title () const = 0;
  virtual symbol x_dimension () const = 0;
  virtual symbol y_dimension () const = 0;
  virtual symbol with () const = 0;
  virtual int style () const = 0;
  virtual const std::vector<double>& x () const = 0;
  virtual const std::vector<double>& y () const = 0;
  virtual const std::vector<double>& xbar () const;
  virtual const std::vector<double>& ybar () const;
  virtual bool load (const Units& units, Treelog& msg) = 0;

  // Utilities.
public:
  virtual void limit (double& xmin, double& xmax,
                      double& ymin, double& ymax) const;
  void distance (double xmin, double xmax, double ymin, double ymax,
		 double& nw, double& ne, double& sw, double& se) const;
  
  // Create and Destroy.
protected:
  explicit XYSource (const BlockModel&);
private:
  XYSource (const XYSource&);
public:
  ~XYSource ();
};

#endif // XYSOURCE_H

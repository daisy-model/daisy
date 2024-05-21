// retention.h --- Retention curves.
// 
// Copyright 2020 KU.
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


#ifndef RETENTION_H
#define RETENTION_H

#include "object_model/model.h"
#include "object_model/symbol.h"

#include <memory>

class BlockModel;
class Treelog;

class Retention : public Model
{
  // Content.
public:
  static const char *const component;
  symbol library_id () const;

  // Simulation.
public:
  virtual double h (double Theta) const = 0;

  // Create and Destroy.
public:
  virtual void initialize (const double Theta_res, const double h_res,
			   const double Theta_sat, Treelog&) = 0;
protected:
  Retention ();
public:
  ~Retention ();
};

struct RetentionPASTIS : public Retention
{
  double Theta_res;
  double h_min;
  double Theta_sat;
  
  double h (const double Theta) const; // []->[cm]
  void initialize (const double Theta_res_, const double h_res_,
		   const double Theta_sat_, Treelog&);
  
  RetentionPASTIS ();
  RetentionPASTIS (const BlockModel&);
  ~RetentionPASTIS ();
};

struct RetentionExponential : public Retention
{
  double k; 			// [cm^-1]
  double Theta_res;		// []
  double h_min;			// [cm]
  double Theta_sat;		// []
  
  double h (const double Theta) const; // []->[cm]
  double Theta (const double h); // [cm]->[]
  void initialize (const double Theta_res_, const double h_res_,
		   const double Theta_sat_, Treelog&);

  RetentionExponential (double k, double Theta_res, double h_min, double Theta_sat);
  RetentionExponential (const BlockModel& al);
  ~RetentionExponential ();
};

#endif // RETENTION_H

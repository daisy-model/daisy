// xref.h -- Find cross referencesa in datastructures.
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


#ifndef XREF_H
#define XREF_H

#include "object_model/symbol.h"
#include <map>
#include <vector>
#include <set>

class Metalib;

class XRef
{
  // Content.
public:
  struct ModelUsed
  {
    const symbol component;
    const symbol model;
    bool operator< (const ModelUsed&) const;
    ModelUsed (symbol comp, symbol mod);
  };

  struct ModelUser
  {
    symbol component;
    symbol model;
    std::vector<symbol> path;
    bool operator< (const ModelUser&) const;
    ModelUser (symbol com, symbol mod, const std::vector<symbol>& p);
  };

  struct SubmodelUser
  {
    symbol submodel;
    std::vector<symbol> path;
    bool operator< (const SubmodelUser&) const;
    SubmodelUser (const symbol sub, const std::vector<symbol>& p);
    SubmodelUser ();
  };

  struct Users
  { 
    std::set<ModelUser> models;
    std::set<SubmodelUser> submodels;
    Users ();
  };

  std::map<symbol, Users> submodels;
  std::map<ModelUsed, Users> models;
  std::map<symbol, Users> components;

  // Create and Destroy.
public:
  XRef (const Metalib& mlib);
  ~XRef ();
};

#endif // XREF_H

// scope_table.h --- A scope based on a table.
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


#ifndef SCOPE_TABLE_H
#define SCOPE_TABLE_H

#include "util/scope.h"
#include <string>
#include <vector>

class LexerTable;

class ScopeTable : public Scope
{
  // Content.
private:
  const LexerTable& lex;
  std::vector<std::string> values;
  
  // Interface.
public:
  void entries (std::set<symbol>&) const;
  Attribute::type lookup (symbol tag) const;
  bool check (symbol tag) const;
  double number (symbol tag) const;
  symbol dimension (symbol tag) const;
  symbol description (symbol tag) const;

  // Use.
public:
  void set (const std::vector<std::string>& entries);

  // Create and Destroy.
private:
  ScopeTable (const ScopeTable&);
  ScopeTable ();
public:
  explicit ScopeTable (const LexerTable& l);
  ~ScopeTable ();
};

#endif // SCOPE_TABLE_H

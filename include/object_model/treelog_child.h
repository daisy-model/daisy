// treelog_child.h -- Modify treelog stream.
// 
// Copyright 2007 Per Abrahamsen and KVL.
// Copyright 2011 KU.
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


#ifndef TREELOG_CHILD_H
#define TREELOG_CHILD_H

#include "object_model/treelog.h"
#include <boost/shared_ptr.hpp>

class TreelogChild : public Treelog
{
  // Content.
protected:
  Treelog& child;

  // Nesting.
public:
  void open (const std::string&);
  void close ();

  // Use.
public:
  void debug (const std::string&);
  void entry (const std::string&);
  void message (const std::string&);
  void warning (const std::string&);
  void error (const std::string&);
  void bug (const std::string&);
  void touch ();
  void flush ();

  // Create and Destroy.
public:
  explicit TreelogChild (Treelog&);
  ~TreelogChild ();
};

class TreelogSilent : public TreelogChild
{ 
  // Use.
public:
  void message (const std::string&);

  // Create and Destroy.
public:
  explicit TreelogSilent (Treelog&);
  ~TreelogSilent ();
};

#endif // TREELOG_CHILD_H

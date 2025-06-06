// destination.h --- Destination for a Select variable.
// 
// Copyright 2003 Per Abrahamsen and KVL.
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


#ifndef DESTINATION_H
#define DESTINATION_H

#include "object_model/symbol.h"
#include <vector>

// Destination
class Destination
{
  // Add data.
public:
  virtual void missing () = 0;
  virtual void add (const std::vector<double>& value) = 0;
  virtual void add (const double value) = 0;
  virtual void add (const symbol value) = 0;
  
  // Create and Destroy
public:
  Destination ();
  virtual ~Destination ();
};

class MultiDest : Destination
{
  // Pass data to these destinations.
  std::vector<Destination*> destinations;
  
public:
  void missing ();
  void add (const std::vector<double>& value);
  void add (const double value);
  void add (const symbol value);
  
  // Create and Destroy
public:
  void add_dest (Destination* dest);
  MultiDest ();
  ~MultiDest ();
};

#endif // DESTINATION_H

// treelog_store.h -- Store messages and distribute to client treelogs.
// 
// Copyright 2007 Per Abrahamsen and KVL.
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


#ifndef TREELOG_STORE_H
#define TREELOG_STORE_H

#include "object_model/treelog.h"
#include <boost/shared_ptr.hpp>
#include <memory>

class TreelogStore : public Treelog
{
  // Content.
private:
  struct Implementation;
  std::unique_ptr<Implementation> impl;

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

public:
  bool running () const;

  // Clients.
public:
  void add_client (boost::shared_ptr<Treelog>);
  void propagate (Treelog&) const;
  void propagate_debug (Treelog&) const;
  void no_more_clients ();
  bool has_unhandled_events () const;

  // Create and Destroy.
public:
  TreelogStore ();
  ~TreelogStore ();
};

class TreelogServer : public TreelogStore
{ 
public:
  TreelogServer ();
  ~TreelogServer ();
};

#endif // TREELOG_STORE_H

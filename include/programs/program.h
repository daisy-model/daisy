// program.h --- Program to run.
// 
// Copyright Per Abrahamsen and KVL.
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


#ifndef PROGRAM_H
#define PROGRAM_H

#include "object_model/model.h"
#include "object_model/symbol.h"
#include "ui/run.h"

#include <vector>

class Treelog;
class BlockModel;
class Block;
class Metalib;
class Log;
class Scope;

class Program : public Model
{
  // Content.
public:
  const symbol objid;
  EXPORT static const char *const component;
  symbol library_id () const;

  // Simulation.
public:
  virtual bool run (Treelog&) = 0;

  // User interface communication.
private:
  Run* ui;
  std::vector<Log*> logs;  
public:
  virtual void attach_ui (Run*, const std::vector<Log*>&);
protected:
  void propagate_ui (Program *const child); 
  bool ui_running () const;
  void ui_set_progress (double val);

  // Extract information.
public:
  virtual const std::vector<const Scope*>& scopes () const;

  // Create and Destroy.
public:
  virtual void initialize (Block&) = 0;
  virtual bool check (Treelog& err) = 0;
protected:
  explicit Program (const BlockModel&);
public:
  ~Program ();
};

#endif // PROGRAM_H

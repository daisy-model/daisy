// frame_model.h -- Model and parameterizations.
// 
// Copyright 2008 Per Abrahamsen and KVL.
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

#ifndef FRAME_MODEL_H
#define FRAME_MODEL_H

#include "frame.h"

class Model;
class Block;
class Declare;
class DeclareModel;

class FrameModel : public Frame
{
  // Inheritance.
private:
  const FrameModel* parent_;
  const FrameModel* parent () const;

  // Construct.
private:
  const DeclareModel *const declaration;
public:
  bool buildable () const;
private:
  Model* construct (Block& context, const symbol key, const FrameModel&) const;
public:
  Model* construct (Block& context, const symbol key) const;

  // Create and Destroy.
private:
  FrameModel ();
public:
  static const FrameModel& root ();
  enum parent_link_t { parent_link };
  enum parent_copy_t { parent_copy }; // OLD: For cloning a library.
  FrameModel (const FrameModel&, parent_link_t);
  FrameModel (const FrameModel&, parent_copy_t); // OLD: For cloning a library.
  FrameModel (const FrameModel&, const AttributeList&); // build_alist, add_d)
  FrameModel (const FrameModel&, const Syntax&, const AttributeList&); // add_d
  FrameModel (const Declare&);  // Declared.
  ~FrameModel ();
};

#endif // FRAME_MODEL_H
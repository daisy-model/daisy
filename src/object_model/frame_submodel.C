// frame_submodel.C -- Submodel parameterizations.
// 
// Copyright 2009 Per Abrahamsen and KVL.
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

#define BUILD_DLL

#include "object_model/frame_submodel.h"
#include "util/assertion.h"

bool 
FrameSubmodel::subset (const Metalib& metalib,
                       const FrameSubmodel& other) const
{
  return subset_elements (metalib, other);
}

FrameSubmodel::FrameSubmodel (const FrameSubmodel& frame, const parent_clone_t) 
  : Frame (frame)
{ }

FrameSubmodel::FrameSubmodel (load_syntax_t load_syntax)
  : Frame ()
{ load_syntax (*this); }

FrameSubmodel& 
FrameSubmodel::clone () const
{ return *new FrameSubmodel (*this, parent_clone); }

FrameSubmodel::FrameSubmodel (const FrameSubmodel& frame, const parent_link_t) 
  : Frame ()
{ }

FrameSubmodel::~FrameSubmodel ()
{ }

const Frame* 
FrameSubmodelValue::parent () const
{ return parent_; }

void 
FrameSubmodelValue::replace_parent (const Frame* new_parent) const
{ parent_ = new_parent; }

FrameSubmodelValue::FrameSubmodelValue (const FrameSubmodelValue& frame, 
                                        const parent_clone_t)
  : FrameSubmodel (frame, parent_clone),
    parent_ (frame.parent ())
{ 
  if (this->parent ())
    this->parent ()->register_child (this); 
}

FrameSubmodelValue& 
FrameSubmodelValue::clone () const
{ return *new FrameSubmodelValue (*this, parent_clone); }

FrameSubmodelValue::FrameSubmodelValue (const FrameSubmodel& frame,
                                        parent_link_t)
  : FrameSubmodel (frame, parent_link),
    parent_ (&frame)
{ 
  daisy_assert (this->parent ());
  this->parent ()->register_child (this); 
}

FrameSubmodelValue::~FrameSubmodelValue ()
{
  if (parent ())
    parent ()->unregister_child (this);
}

// frame_submodel.C ends here.

